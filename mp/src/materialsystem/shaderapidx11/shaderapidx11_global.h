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

extern CShaderDeviceDX11 *g_pShaderDevice;
extern CShaderDeviceMgrDX11 *g_pShaderDeviceMgr;
extern CShaderAPIDX11 *g_pShaderAPI;
extern CShaderShadowDX11 *g_pShaderShadow;
extern CHardwareConfigDX11 *g_pHardwareConfig;



#endif // SHADERAPIDX11_GLOBAL_H