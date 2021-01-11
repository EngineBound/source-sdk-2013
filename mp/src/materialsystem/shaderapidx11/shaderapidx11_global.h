#ifndef SHADERAPIDX11_GLOBAL_H
#define SHADERAPIDX11_GLOBAL_H

#ifdef _WIN32
#pragma once
#endif

class IShaderUtil;
class CShaderDeviceDX11;
class CShaderDeviceMgrDX11;
class CShaderAPIDX11;
class CShaderShadowDX11;
class CHardwareConfigDX11;

#define VENDORID_NVIDIA	0x10DE
#define VENDORID_ATI	0x1002

extern IShaderUtil* g_pShaderUtil;
inline IShaderUtil* ShaderUtil()
{
	return g_pShaderUtil;
}

extern ID3D11Device *g_pD3DDevice;
extern ID3D11DeviceContext *g_pD3DDeviceContext;
extern IDXGISwapChain *g_pD3DSwapChain;

extern CShaderDeviceDX11 *g_pShaderDeviceDX11;
extern CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11;
extern CShaderAPIDX11 *g_pShaderAPIDX11;
extern CShaderShadowDX11 *g_pShaderShadowDX11;
extern CHardwareConfigDX11 *g_pHardwareConfigDX11;



#endif // SHADERAPIDX11_GLOBAL_H