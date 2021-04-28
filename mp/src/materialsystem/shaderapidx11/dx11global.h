#ifndef DX11GLOBAL_H
#define DX11GLOBAL_H

#ifdef _WIN32
#pragma once
#endif

#define VENDORID_NVIDIA	0x10DE
#define VENDORID_ATI	0x1002

class IShaderUtil;
class IShaderAPIDX11;
class IShaderDeviceMgrDX11;
class IShaderDeviceDX11;
class IShaderShadowDX11;
class IMaterialSystemHardwareConfigDX11;

extern IShaderUtil* g_pShaderUtil;
extern IShaderAPIDX11* g_pShaderAPI;
extern IShaderDeviceMgrDX11* g_pShaderDeviceMgr;
extern IShaderDeviceDX11* g_pShaderDevice;
extern IShaderShadowDX11* g_pShaderShadow;
extern IMaterialSystemHardwareConfigDX11* g_pHardwareConfig;

#endif