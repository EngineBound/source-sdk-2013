
#include "shaderdevicemgrdx11.h"
#include "dx11global.h"

#include "ishaderutil.h"
#include "ishadershadowdx11.h"
#include "ishaderapidx11.h"
#include "shaderdevicedx11.h"


static CShaderDeviceMgrDX11 s_ShaderDeviceMgrDX11;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11 = &s_ShaderDeviceMgrDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceMgrDX11, IShaderDeviceMgrDX11,
	SHADER_DEVICE_MGR_INTERFACE_VERSION, s_ShaderDeviceMgrDX11)

IShaderDeviceMgrDX11 *g_pShaderDeviceMgr = g_pShaderDeviceMgrDX11;
IShaderUtil *g_pShaderUtil;

// Here's where the app systems get to learn about each other 
bool CShaderDeviceMgrDX11::Connect(CreateInterfaceFn factory)
{
	ConnectTier1Libraries(&factory, 1);

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDXGIFactory);
	if (FAILED(hr))
		return false;

	// Get shaderutil
	g_pShaderUtil = (IShaderUtil*)factory(SHADER_UTIL_INTERFACE_VERSION, NULL);

	return true;
}

void CShaderDeviceMgrDX11::Disconnect()
{
	DisconnectTier1Libraries();
	g_pShaderUtil = NULL;
}


// Here's where systems can access other interfaces implemented by this object
// Returns NULL if it doesn't implement the requested interface
void *CShaderDeviceMgrDX11::QueryInterface(const char *pInterfaceName)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


// Init, shutdown
InitReturnVal_t CShaderDeviceMgrDX11::Init()
{
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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

	_AssertMsg(0, "Incomplete implementation! " __FUNCTION__, 0, 0);
}


// Gets recommended configuration for a particular adapter at a particular dx level
bool CShaderDeviceMgrDX11::GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
		DXGI_ENUM_MODES_INTERLACED, &count, NULL);
	if (FAILED(hr))
		return 0;

	return count;
}


// Returns mode information..
void CShaderDeviceMgrDX11::GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const
{
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->m_Format = IMAGE_FORMAT_UNKNOWN;

	int nModeCount = GetModeCount(nAdapter);
	if (!nModeCount)
		return;

	IDXGIOutput *pOutput = GetAdapterOutput(nAdapter);
	if (!pOutput)
		return;

	UINT count = 0;
	DXGI_MODE_DESC *pModeDescs = (DXGI_MODE_DESC *)_alloca(nModeCount * sizeof(DXGI_MODE_DESC));
	HRESULT hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_ENUM_MODES_INTERLACED, &count, pModeDescs);
	if (FAILED(hr))
		return;

	DXGI_MODE_DESC *modeDesc = &pModeDescs[nMode];

	pInfo->m_Format = modeDesc->Format != DXGI_FORMAT_UNKNOWN ? IMAGE_FORMAT_RGB888 : IMAGE_FORMAT_UNKNOWN; // REPLACE THIS
	pInfo->m_nHeight = modeDesc->Height;
	pInfo->m_nWidth = modeDesc->Width;
	pInfo->m_nRefreshRateDenominator = modeDesc->RefreshRate.Denominator;
	pInfo->m_nRefreshRateNumerator = modeDesc->RefreshRate.Numerator;

	_AssertMsg(0, "Incomplete implementation! " __FUNCTION__, 0, 0);
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
	if (!V_stricmp(pInterfaceName, SHADERAPI_INTERFACE_VERSION))
		return static_cast<IShaderAPI*>(g_pShaderAPI);
	if (!V_stricmp(pInterfaceName, SHADERSHADOW_INTERFACE_VERSION))
		return static_cast<IShaderShadow*>(g_pShaderShadow);

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
	g_pShaderDeviceDX11->Shutdown();
	if (!g_pShaderDeviceDX11->Init(hWnd, nAdapter, mode))
		return NULL;

	return ShaderInterfaceFactory;
}


// Installs a callback to get called 
void CShaderDeviceMgrDX11::AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderDeviceMgrDX11::RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}
