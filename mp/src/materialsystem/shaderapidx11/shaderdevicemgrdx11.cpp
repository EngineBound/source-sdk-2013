#include "shaderdevicemgrdx11.h"
#include "tier2/tier2.h"
#include "ishaderutil.h"

#pragma comment(lib, "dxgi.lib")

static CShaderDeviceMgrDX11 s_ShaderDeviceMgrDx11;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDx11 = &s_ShaderDeviceMgrDx11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceMgrDX11, IShaderDeviceMgr,
	SHADER_DEVICE_MGR_INTERFACE_VERSION, s_ShaderDeviceMgrDx11)


CShaderDeviceMgrDX11::CShaderDeviceMgrDX11()
{
	m_pDXGIFactory = NULL;
}

// Here's where the app systems get to learn about each other 
bool CShaderDeviceMgrDX11::Connect(CreateInterfaceFn factory)
{
	ConnectTier1Libraries(&factory, 1);

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&m_pDXGIFactory);
	if (FAILED(hr))
		return false;

	return true;
}

void CShaderDeviceMgrDX11::Disconnect()
{
	m_pDXGIFactory->Release();

	DisconnectTier1Libraries();

	return;
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
		MaterialAdapterInfo_t &info = m_vpAdapters[m_vpAdapters.AddToTail()];

		PopulateAdapterInfo(pAdapter, info);
	}

	return INIT_OK;
}

void CShaderDeviceMgrDX11::PopulateAdapterInfo(IDXGIAdapter *pAdapter, MaterialAdapterInfo_t &info)
{
	DXGI_ADAPTER_DESC desc;
	pAdapter->GetDesc(&desc);

	V_UnicodeToUTF8(desc.Description, info.m_pDriverName, MATERIAL_ADAPTER_NAME_LENGTH);
	info.m_VendorID = desc.VendorId;
	info.m_DeviceID = desc.DeviceId;
	info.m_SubSysID = desc.SubSysId;
	info.m_Revision = desc.Revision;
	info.m_nDXSupportLevel = 110;
	info.m_nMaxDXSupportLevel = 110;
}

IDXGIOutput *CShaderDeviceMgrDX11::GetAdapterOutput(int nAdapter) const
{
	IDXGIAdapter *pAdapter;
	
	if (m_pDXGIFactory->EnumAdapters(nAdapter, &pAdapter) == DXGI_ERROR_NOT_FOUND)
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

void CShaderDeviceMgrDX11::Shutdown()
{
	return;
}


// Gets the number of adapters...
int	 CShaderDeviceMgrDX11::GetAdapterCount() const
{
	return m_vpAdapters.Count();
}


// Returns info about each adapter
void CShaderDeviceMgrDX11::GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const
{
	info = m_vpAdapters[nAdapter];
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
	return;
}


// Returns the current mode info for the requested adapter
void CShaderDeviceMgrDX11::GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const
{
	return;
}


// Initialization, shutdown
bool CShaderDeviceMgrDX11::SetAdapter(int nAdapter, int nFlags)
{
	return false;
}


// Sets the mode
// Use the returned factory to get at an IShaderDevice and an IShaderRender
// and any other interfaces we decide to create.
// A returned factory of NULL indicates the mode was not set properly.
CreateInterfaceFn CShaderDeviceMgrDX11::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t& mode)
{
	return (CreateInterfaceFn)0;
}


// Installs a callback to get called 
void CShaderDeviceMgrDX11::AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	return;
}

void CShaderDeviceMgrDX11::RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func)
{
	return;
}