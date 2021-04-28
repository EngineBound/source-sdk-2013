
#include "shaderdevicemgrdx11.h"
#include "dx11global.h"

#include "ishaderutil.h"


static CShaderDeviceMgrDX11 s_ShaderDeviceMgrDX11;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11 = &s_ShaderDeviceMgrDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceMgrDX11, IShaderDeviceMgrDX11,
	SHADER_DEVICE_MGR_INTERFACE_VERSION, s_ShaderDeviceMgrDX11)

IShaderDeviceMgrDX11 *g_pShaderDeviceMgr = g_pShaderDeviceMgrDX11;
IShaderUtil *g_pShaderUtil;

// Here's where the app systems get to learn about each other 
bool CShaderDeviceMgrDX11::Connect(CreateInterfaceFn factory)
{
	// Get shaderutil
	g_pShaderUtil = (IShaderUtil*)factory(SHADER_UTIL_INTERFACE_VERSION, NULL);

	return true;
}

void CShaderDeviceMgrDX11::Disconnect()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return INIT_FAILED;
}

void CShaderDeviceMgrDX11::Shutdown()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}



// Gets the number of adapters...
int	 CShaderDeviceMgrDX11::GetAdapterCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Returns info about each adapter
void CShaderDeviceMgrDX11::GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Returns mode information..
void CShaderDeviceMgrDX11::GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the current mode info for the requested adapter
void CShaderDeviceMgrDX11::GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Initialization, shutdown
bool CShaderDeviceMgrDX11::SetAdapter(int nAdapter, int nFlags)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Sets the mode
// Use the returned factory to get at an IShaderDevice and an IShaderRender
// and any other interfaces we decide to create.
// A returned factory of NULL indicates the mode was not set properly.
CreateInterfaceFn CShaderDeviceMgrDX11::SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t& mode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
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
