
#include "shaderdevicemgrdx11.h"
#include "dx11global.h"

#include "ishaderutil.h"
#include "filesystem.h"
#include "shadershadowdx11.h"
#include "shaderapidx11.h"
#include "shaderdevicedx11.h"
#include "apitexturedx11.h"

#include <d3d11.h>

#include "memdbgon.h"

CThreadFastMutex g_ShaderAPIMutex;

static CShaderDeviceMgrDX11 s_ShaderDeviceMgrDX11;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11 = &s_ShaderDeviceMgrDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceMgrDX11, IShaderDeviceMgrDX11,
	SHADER_DEVICE_MGR_INTERFACE_VERSION, s_ShaderDeviceMgrDX11)

IShaderDeviceMgrDX11 *g_pShaderDeviceMgr = g_pShaderDeviceMgrDX11;
IShaderUtil *g_pShaderUtil;
IFileSystem *g_pFullFileSystem; // We only want this for now so no tier 2

// Here's where the app systems get to learn about each other 
bool CShaderDeviceMgrDX11::Connect(CreateInterfaceFn factory)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ConnectTier1Libraries(&factory, 1);

	HRESULT hr = CreateDXGIFactory(IID_IDXGIFactory, (void**)&m_pDXGIFactory);
	if (FAILED(hr))
		return false;

	// Get shaderutil, filesystem
	g_pShaderUtil = (IShaderUtil*)factory(SHADER_UTIL_INTERFACE_VERSION, NULL);
	g_pFullFileSystem = (IFileSystem *)factory(FILESYSTEM_INTERFACE_VERSION, NULL);

	return true;
}

void CShaderDeviceMgrDX11::Disconnect()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	DisconnectTier1Libraries();
	g_pShaderUtil = NULL;

	if (m_pDXGIFactory)
	{
		m_pDXGIFactory->Release();
		m_pDXGIFactory = NULL;
	}

	m_vAdapterInfo.RemoveAll();
}


// Here's where systems can access other interfaces implemented by this object
// Returns NULL if it doesn't implement the requested interface
void *CShaderDeviceMgrDX11::QueryInterface(const char *pInterfaceName)
{
	if (!Q_stricmp(pInterfaceName, SHADER_DEVICE_MGR_INTERFACE_VERSION))
		return static_cast<IShaderDeviceMgr*>(this);
	return NULL;
}


// Init, shutdown
InitReturnVal_t CShaderDeviceMgrDX11::Init()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	m_vAdapterInfo.RemoveAll();

	IDXGIAdapter *pAdapter;

	for (UINT i = 0; m_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		HWInfo_t *info = &m_vAdapterInfo[m_vAdapterInfo.AddToTail()];
		IDXGIOutput* pOutput = GetAdapterOutput(i);

		if (!PopulateHWInfo(info, pAdapter, pOutput))
			continue;

		// Read from dxsupport.cfg
	}

	return INIT_OK;
}

void CShaderDeviceMgrDX11::Shutdown()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	if (g_pShaderDeviceDX11)
	{
		g_pShaderDeviceDX11->Shutdown();
		g_pShaderDevice = NULL;
	}
}

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

	pHWInfo->m_bPreferDynamicTextures = true;

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



// Gets the number of adapters...
int	 CShaderDeviceMgrDX11::GetAdapterCount() const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

IDXGIAdapter *CShaderDeviceMgrDX11::GetAdapter(int nAdapter) const
{
	IDXGIAdapter *pAdapter;
	HRESULT hr = m_pDXGIFactory->EnumAdapters(nAdapter, &pAdapter);

	if (FAILED(hr))
		return NULL;

	return pAdapter;
}

IDXGIOutput *CShaderDeviceMgrDX11::GetAdapterOutput(int nAdapter) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

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


// Returns info about each adapter
void CShaderDeviceMgrDX11::GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const
{
	memset(&info, 0, sizeof(info));
	info.m_nDXSupportLevel = 110;

	ALERT_INCOMPLETE();
}


// Gets recommended configuration for a particular adapter at a particular dx level
bool CShaderDeviceMgrDX11::GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration)
{
	ALERT_BADLY_IMPLEMENTED();
	return true;
}


// Returns the number of modes
int	 CShaderDeviceMgrDX11::GetModeCount(int nAdapter) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	IDXGIOutput *pOutput = GetAdapterOutput(nAdapter);
	if (!pOutput)
		return 0;

	UINT count = 0;

	HRESULT hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_ENUM_MODES_INTERLACED, &count, NULL);
	if (FAILED(hr))
		return 0;

	return count;
}


// Returns mode information..
void CShaderDeviceMgrDX11::GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->m_Format = IMAGE_FORMAT_UNKNOWN;

	IDXGIOutput *pOutput = GetAdapterOutput(nAdapter);
	if (!pOutput)
		return;

	UINT count = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	HRESULT hr = pOutput->GetDisplayModeList(format,
		flags, &count, NULL);
	Assert(SUCCEEDED(hr));

	if ((UINT)nMode >= count)
		return;

	DXGI_MODE_DESC *pModeDescs = (DXGI_MODE_DESC *)_alloca(count * sizeof(DXGI_MODE_DESC));
	hr = pOutput->GetDisplayModeList(format,
		flags, &count, pModeDescs);
	Assert(SUCCEEDED(hr));

	DXGI_MODE_DESC *modeDesc = &pModeDescs[nMode];

	pInfo->m_Format = CAPITextureDX11::GetImageFormat(modeDesc->Format); // REPLACE THIS
	pInfo->m_nHeight = modeDesc->Height;
	pInfo->m_nWidth = modeDesc->Width;
	pInfo->m_nRefreshRateDenominator = modeDesc->RefreshRate.Denominator;
	pInfo->m_nRefreshRateNumerator = modeDesc->RefreshRate.Numerator;
}


// Returns the current mode info for the requested adapter
void CShaderDeviceMgrDX11::GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const
{
	GetModeInfo(pInfo, g_pShaderDeviceDX11->m_nAdapter, nAdapter);
}


// Initialization, shutdown
bool CShaderDeviceMgrDX11::SetAdapter(int nAdapter, int nFlags)
{
	g_pShaderDeviceDX11->m_nAdapter = nAdapter;
	return true;
}

static void* ShaderInterfaceFactory(const char *pInterfaceName, int *pReturnCode)
{
	if (pReturnCode)
	{
		*pReturnCode = IFACE_OK;
	}

	if (!V_stricmp(pInterfaceName, SHADER_DEVICE_INTERFACE_VERSION))
		return static_cast<IShaderDevice*>(g_pShaderDevice);
	if (!V_stricmp(pInterfaceName, SHADERAPI_INTERFACE_VERSION) || !V_stricmp(pInterfaceName, SHADERDYNAMIC_INTERFACE_VERSION) || !V_stricmp(pInterfaceName, DEBUG_TEXTURE_INFO_VERSION))
		return static_cast<IShaderAPI*>(g_pShaderAPI);
	if (!V_stricmp(pInterfaceName, SHADERSHADOW_INTERFACE_VERSION))
		return static_cast<IShaderShadow*>(g_pShaderShadow);\
	if (!V_stricmp(pInterfaceName, MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION))
		return static_cast<IMaterialSystemHardwareConfig*>(g_pHardwareConfig);

	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}
	return NULL;
}

// Sets the mode
// Use the returned factory to get at an IShaderDevice and an IShaderRender
// and any other interfaces we decide to create.
// A returned factory of NULL indicates the mode was not set properly.
CreateInterfaceFn CShaderDeviceMgrDX11::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t& mode)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	g_pShaderDevice = NULL;
	g_pShaderAPI = NULL;
	g_pShaderShadow = NULL;

	g_pShaderDeviceDX11->Shutdown();
	if (!g_pShaderDeviceDX11->Init(hWnd, nAdapter, mode))
		return NULL;

//	g_pShaderDevice = g_pShaderDeviceDX11;
	g_pShaderAPI = g_pShaderAPIDX11;
	g_pShaderShadow = g_pShaderShadowDX11;

	return ShaderInterfaceFactory;
}


// Installs a callback to get called 
void CShaderDeviceMgrDX11::AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderDeviceMgrDX11::RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	ALERT_NOT_IMPLEMENTED();
}
