#ifndef SHADERDEVICEMGRDX11_H
#define SHADERDEVICEMGRDX11_H

#ifdef _WIN32
#pragma once
#endif

#include <d3d11.h>
#include "hardwareconfigdx11.h"
#include "ishaderdevice.h"

class CShaderDeviceMgrDX11 : public IShaderDeviceMgr
{
public:
	CShaderDeviceMgrDX11();
	//~CShaderDeviceMgrDX11();

	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory);
	virtual void Disconnect();

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName);

	// Init, shutdown
	virtual InitReturnVal_t Init();
	virtual void Shutdown();

	// Gets the number of adapters...
	virtual int	 GetAdapterCount() const;

	// Returns info about each adapter
	virtual void GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const;

	// Gets recommended configuration for a particular adapter at a particular dx level
	virtual bool GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues *pConfiguration);

	// Returns the number of modes
	virtual int	 GetModeCount(int nAdapter) const;

	// Returns mode information..
	virtual void GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const;

	// Returns the current mode info for the requested adapter
	virtual void GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const;

	// Initialization, shutdown
	virtual bool SetAdapter(int nAdapter, int nFlags);

	// Sets the mode
	// Use the returned factory to get at an IShaderDevice and an IShaderRender
	// and any other interfaces we decide to create.
	// A returned factory of NULL indicates the mode was not set properly.
	virtual CreateInterfaceFn SetMode(void *hWnd, int nAdapter, const ShaderDeviceInfo_t& mode);

	// Installs a callback to get called 
	virtual void AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func);
	virtual void RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func);

protected:
	IDXGIAdapter *GetAdapter(int nAdapter) const;
	IDXGIOutput *GetAdapterOutput(int nAdapter) const;

	friend class CShaderDeviceDX11;
private:

	IDXGIFactory *m_pDXGIFactory;

	CUtlVector<HWInfo_t> m_vpAdapters;
	bool PopulateHWInfo(HWInfo_t *pHWInfo, IDXGIAdapter *pAdapter, IDXGIOutput *pOutput);

	CUtlVector<ModeChangeCallbackFunc_t> m_vpModeChangeCallbacks;

};

#endif // SHADERDEVICEMGRDX11_H