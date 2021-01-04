#include "shaderdevicemgrdx11.h"
#include "tier2/tier2.h"
#include "ishaderutil.h"
#include "shaderdevicedx11.h"
#include "shaderapidx11.h"
#include "shadershadowdx11.h"
#include "icommandline.h"

#include "shaderapidx11_global.h"

// MUST BE LAST FILE AND WHATEVER
#include "memdbgon.h"

extern CShaderDeviceDX11 *g_pShaderDeviceDx11;
extern CShaderAPIDX11 *g_pShaderAPIDx11;
extern CShaderShadowDX11*g_pShaderShadowDx11;

static CShaderDeviceMgrDX11 s_ShaderDeviceMgrDx11;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDx11 = &s_ShaderDeviceMgrDx11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceMgrDX11, IShaderDeviceMgr,
	SHADER_DEVICE_MGR_INTERFACE_VERSION, s_ShaderDeviceMgrDx11)

// Initialize device manager with dummy values
CShaderDeviceMgrDX11::CShaderDeviceMgrDX11()
{
	m_pDXGIFactory = NULL;
}

// Here's where the app systems get to learn about each other 
bool CShaderDeviceMgrDX11::Connect(CreateInterfaceFn factory)
{
	ConnectTier1Libraries(&factory, 1);

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&m_pDXGIFactory);
	if (FAILED(hr))
		return false;

	return true;
}

// Disconnect the shader device manager from the other systems
void CShaderDeviceMgrDX11::Disconnect()
{
	m_pDXGIFactory->Release();

	DisconnectTier1Libraries();
}


// Here's where systems can access other interfaces implemented by this object
// Returns NULL if it doesn't implement the requested interface
void *CShaderDeviceMgrDX11::QueryInterface(const char *pInterfaceName)
{
	if (!V_stricmp(pInterfaceName, SHADER_DEVICE_MGR_INTERFACE_VERSION))
		return (IShaderDeviceMgr *)this;
	return NULL;
}


// Init, shutdown
InitReturnVal_t CShaderDeviceMgrDX11::Init()
{
	m_vpAdapters.RemoveAll();

	IDXGIAdapter *pAdapter;

	for (UINT i = 0; m_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		HWInfo_t info = m_vpAdapters[m_vpAdapters.AddToTail()];
		IDXGIOutput* pOutput = GetAdapterOutput(i);

		if (!PopulateHWInfo(&info, pAdapter, pOutput))
			continue;

		// Read from dxsupport.cfg
	}

	return INIT_OK;
}

// Setup HWInfo_t with default values
bool CShaderDeviceMgrDX11::PopulateHWInfo(HWInfo_t *pHWInfo, IDXGIAdapter *pAdapter, IDXGIOutput *pOutput)
{
	DXGI_ADAPTER_DESC desc;
	HRESULT hr = pAdapter->GetDesc(&desc);
	if (FAILED(hr))
		return false;

	Q_UnicodeToUTF8(desc.Description, pHWInfo->m_pDriverName, MATERIAL_ADAPTER_NAME_LENGTH);
	pHWInfo->m_VendorID = desc.VendorId;
	pHWInfo->m_DeviceID = desc.DeviceId;
	pHWInfo->m_SubSysID = desc.SubSysId;
	pHWInfo->m_Revision = desc.Revision;
	pHWInfo->m_nDXSupportLevel = 110; // DX11
	pHWInfo->m_nMaxDXSupportLevel = 110;
	//pHWInfo->m_nDriverVersionHigh;
	//pHWInfo->m_nDriverVersionLow;

	pHWInfo->m_nSamplerCount = 16;
	pHWInfo->m_bHasSetDeviceGammaRamp = true; // not actually true, fix this later
	pHWInfo->m_bSupportsCompressedTextures = true;
	pHWInfo->m_VertexCompressionType = VERTEX_COMPRESSION_ON;
	pHWInfo->m_bSupportsNormalMapCompression = true;

	pHWInfo->m_bSupportsVertexAndPixelShaders = true;
	pHWInfo->m_bSupportsPixelShaders_1_4 = true;
	pHWInfo->m_bSupportsPixelShaders_2_0 = true;
	pHWInfo->m_bSupportsPixelShaders_2_b = true;
	pHWInfo->m_bSupportsShaderModel_3_0 = true;
	pHWInfo->m_bSupportsVertexShaders_2_0 = true;

	pHWInfo->m_bSupportsStaticControlFlow = true; // check later
	pHWInfo->m_nMaximumAnisotropicLevel = D3D11_REQ_MAXANISOTROPY;
	pHWInfo->m_nMaxTextureWidth = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	pHWInfo->m_nMaxTextureHeight = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	pHWInfo->m_nTextureMemorySize = desc.DedicatedVideoMemory;
	pHWInfo->m_bSupportsOverbright = true;
	pHWInfo->m_bSupportsCubemaps = true;
	pHWInfo->m_bSupportsMipmappedCubemaps = true;
	pHWInfo->m_bSupportsNonPow2Textures = true;

	pHWInfo->m_nTextureStageCount = 0;
	pHWInfo->m_nNumVertexShaderConstants = 1024; // probably much higher
	pHWInfo->m_nNumPixelShaderConstants = 1024;
	pHWInfo->m_nMaxNumLights = 4;
	pHWInfo->m_bSupportsHardwareLighting = false; // CHECK LATER
	pHWInfo->m_nMaxBlendMatrices = 0;
	pHWInfo->m_nMaxBlendMatrixIndices = 0;
	pHWInfo->m_nMaxTextureAspectRatio = 1024;
	pHWInfo->m_nMaxVertexShaderBlendMatrices = 0; 
	pHWInfo->m_nMaxUserClipPlanes = 6;
	pHWInfo->m_bUseFastClipping = false; // END CHECK LATER

	pHWInfo->m_pShaderDLLName[0] = 0;

	pHWInfo->m_bPreferDynamicTextures= true;

	pHWInfo->m_bSupportsHDR = true;

	pHWInfo->m_bHasProjectedBumpEnv = true;
	pHWInfo->m_bSupportsSpheremapping = true;
	pHWInfo->m_bNeedsAAClamp = false; // CHECK
	pHWInfo->m_bNeedsATICentroidHack = false;

	pHWInfo->m_bSupportsColorOnSecondStream = true;
	pHWInfo->m_bSupportsStaticPlusDynamicLighting = true;

	pHWInfo->m_bPreferReducedFillrate = true; // CHECK

	//pHWInfo->m_nMaxDXSupportLevel;

	pHWInfo->m_bSpecifiesFogColorInLinearSpace = (desc.VendorId == VENDORID_NVIDIA);

	pHWInfo->m_bSupportsSRGB = true;
	pHWInfo->m_bFakeSRGBWrite = false; // CHECK
	pHWInfo->m_bCanDoSRGBReadFromRTs = true; // CHECK

	pHWInfo->m_bSupportsGLMixedSizeTargets = true; // CHECK

	pHWInfo->m_nVertexTextureCount = 16;
	pHWInfo->m_nMaxVertexTextureDimension = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;

	pHWInfo->m_nMaxTextureDepth = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;

	pHWInfo->m_HDRType = HDR_TYPE_INTEGER; // Change later, -floathdr forces float

	pHWInfo->m_bSupportsStreamOffsetActual = true;
	pHWInfo->m_bSupportsStreamOffset = pHWInfo->m_bSupportsStreamOffsetActual;

	pHWInfo->m_nMaxViewports = 4;

	pHWInfo->m_nShadowFilterMode = 0;

	pHWInfo->m_bNeedsShaderSRGBConversion = false;

	pHWInfo->m_bUsesSRGBCorrectBlending = true; // CHECK

	pHWInfo->m_bHasFastVertexTextures = false;

	pHWInfo->m_bActuallySupportsPixelShaders_2_b = true;

	pHWInfo->m_MaxHDRMode = HDR_TYPE_FLOAT;

	pHWInfo->m_bSupportsBorderColor = true;
	pHWInfo->m_bSupportsFetch4 = (desc.VendorId == VENDORID_ATI);

	pHWInfo->m_flMinGammaControlPoint = 0.0f;
	pHWInfo->m_flMaxGammaControlPoint = 65535.0f;
	pHWInfo->m_nGammaControlPointCount = 256;

	return true;
}

// Get adapter given an id
IDXGIAdapter *CShaderDeviceMgrDX11::GetAdapter(int nAdapter) const
{
	IDXGIAdapter *pAdapter;
	HRESULT hr = m_pDXGIFactory->EnumAdapters(nAdapter, &pAdapter);
	
	if (FAILED(hr))
		return NULL;
	return pAdapter;

}

// Get output of adapter given an id
IDXGIOutput *CShaderDeviceMgrDX11::GetAdapterOutput(int nAdapter) const
{
	IDXGIAdapter *pAdapter = GetAdapter(nAdapter);
	
	if (!pAdapter)
		return NULL;

	IDXGIOutput *pOutput;
	for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_OUTPUT_DESC outputDesc;

		HRESULT hr = pOutput->GetDesc(&outputDesc);
		if (FAILED(hr))
			continue;

		if (!outputDesc.AttachedToDesktop)
			continue;

			return pOutput;
	}

	return NULL;
}

// Shutdown the device manager
void CShaderDeviceMgrDX11::Shutdown()
{
	if (g_pShaderDevice)
	{
		g_pShaderDevice->Shutdown();
		g_pShaderDevice = NULL;
	}
}


// Gets the number of adapters...
int	 CShaderDeviceMgrDX11::GetAdapterCount() const
{
	return m_vpAdapters.Count();
}


// Returns info about each adapter
void CShaderDeviceMgrDX11::GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const
{
	info = (MaterialAdapterInfo_t)m_vpAdapters[nAdapter];
}


// Gets recommended configuration for a particular adapter at a particular dx level
bool CShaderDeviceMgrDX11::GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration)
{
	return false;
}


// Returns the number of modes
int	 CShaderDeviceMgrDX11::GetModeCount(int nAdapter) const
{
	IDXGIOutput *pOutput = GetAdapterOutput(nAdapter);
	if (!pOutput)
		return 0;

	UINT count = 0;

	HRESULT hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_ENUM_MODES_INTERLACED, &count, 0);

	if (FAILED(hr))
		return 0;

	return count;
}


// Returns mode information..
void CShaderDeviceMgrDX11::GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const
{
	IDXGIOutput *pOutput = GetAdapterOutput(nAdapter);
	if (!pOutput)
		return;

	UINT count = 0;
	

	HRESULT hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_ENUM_MODES_INTERLACED, &count, 0);

	if (FAILED(hr))
		return;

	if ((int)count < nMode)
		return;


	DXGI_MODE_DESC *pModes = (DXGI_MODE_DESC *)alloca(count * sizeof(DXGI_MODE_DESC));
	hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_ENUM_MODES_INTERLACED, &count, pModes);

	if (FAILED(hr))
		return;
	

	pInfo->m_nVersion = SHADER_DISPLAY_MODE_VERSION;
	pInfo->m_Format = IMAGE_FORMAT_RGBA8888; // IMPORTANT: CHANGE LATER
	pInfo->m_nHeight = pModes[nMode].Height;
	pInfo->m_nWidth = pModes[nMode].Width;
	pInfo->m_nRefreshRateDenominator = pModes[nMode].RefreshRate.Denominator;
	pInfo->m_nRefreshRateNumerator = pModes[nMode].RefreshRate.Numerator;
}


// Returns the current mode info for the requested adapter
void CShaderDeviceMgrDX11::GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const
{
	return;
}


// Initialization, shutdown
bool CShaderDeviceMgrDX11::SetAdapter(int nAdapter, int nFlags)
{
	g_pShaderDevice = g_pShaderDeviceDx11;
	g_pShaderDeviceDx11->m_nCurrentAdapter = nAdapter;

	return false;
}


// Sets the mode
// Use the returned factory to get at an IShaderDevice and an IShaderRender
// and any other interfaces we decide to create.
// A returned factory of NULL indicates the mode was not set properly.
CreateInterfaceFn CShaderDeviceMgrDX11::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t& mode)
{
	int nDXLevel = mode.m_nDXLevel != 0 ? mode.m_nDXLevel : m_vpAdapters[nAdapter].m_nDXSupportLevel;
	nDXLevel = CommandLine()->ParmValue("-dxlevel", nDXLevel);
	nDXLevel = min(nDXLevel, m_vpAdapters[nAdapter].m_nMaxDXSupportLevel);

	if (nDXLevel < 110)
	{
		Error("ERROR: DX11 Unsupported\n");
		return NULL;
	}

	if (g_pShaderDevice)
	{
		g_pShaderDevice->Shutdown();
		g_pShaderDevice = NULL;
	}

	g_pShaderAPI = NULL;
	g_pShaderShadow = NULL;

	if (!g_pShaderDeviceDx11->Initialize(hWnd, nAdapter, mode))
		return NULL;

	g_pShaderDevice = g_pShaderDeviceDx11;
	g_pShaderAPI = g_pShaderAPIDx11;
	g_pShaderShadow = g_pShaderShadowDx11;

	return CreateShaderInterface;
}

// Get shader interface given the name
void *CShaderDeviceMgrDX11::CreateShaderInterface(const char *pName, int *pReturnCode)
{
	if (pReturnCode)
	{
		*pReturnCode = IFACE_OK;
	}
	if (!Q_stricmp(pName, SHADER_DEVICE_INTERFACE_VERSION))
		return static_cast<IShaderDevice*>(g_pShaderDevice);
	if (!Q_stricmp(pName, SHADERAPI_INTERFACE_VERSION))
		return static_cast<IShaderAPI*>(g_pShaderAPI);
	if (!Q_stricmp(pName, SHADERSHADOW_INTERFACE_VERSION))
		return static_cast<IShaderShadow*>(g_pShaderShadow);

	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}
	return NULL;
}

// Installs a callback to get called 
void CShaderDeviceMgrDX11::AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func) // Call when mode is changed
{
	if (m_vpModeChangeCallbacks.Find(func) > 0)
		return;

	m_vpModeChangeCallbacks.AddToTail(func);
}

// Remove callback installed with AddModeChangeCallback
void CShaderDeviceMgrDX11::RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	m_vpModeChangeCallbacks.FindAndRemove(func);
}