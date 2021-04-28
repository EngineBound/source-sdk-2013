#ifndef DX11GLOBAL_H
#define DX11GLOBAL_H

#ifdef _WIN32
#pragma once
#endif

class IShaderUtil;
class IShaderAPIDX11;
class IShaderDeviceMgrDX11;
class IShaderDeviceDX11;
class IMaterialSystemHardwareConfigDX11;

extern IShaderUtil* g_pShaderUtil;
extern IShaderAPIDX11* g_pShaderAPI;
extern IShaderDeviceMgrDX11* g_pShaderDeviceMgr;
extern IShaderDeviceDX11* g_pShaderDevice;
extern IMaterialSystemHardwareConfigDX11* g_pHardwareConfig;

#endif