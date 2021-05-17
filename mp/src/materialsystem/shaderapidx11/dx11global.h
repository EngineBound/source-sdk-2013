#ifndef DX11GLOBAL_H
#define DX11GLOBAL_H

#ifdef _WIN32
#pragma once
#endif

#define VENDORID_NVIDIA	0x10DE
#define VENDORID_ATI	0x1002

#define _ALERT_IMPLEMENTATION 0

#if (_ALERT_IMPLEMENTATION == 1)

#define ALERT_NOT_IMPLEMENTED() _AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0)
#define ALERT_INCOMPLETE() _AssertMsg(0, "Incomplete implementation! " __FUNCTION__, 0, 0)
#define ALERT_BADLY_IMPLEMENTED() _AssertMsg(0, "Bad implementation! " __FUNCTION__, 0, 0)

#else

#define ALERT_NOT_IMPLEMENTED()
#define ALERT_INCOMPLETE()
#define ALERT_BADLY_IMPLEMENTED()

#endif

typedef void * ShaderHandle_t;
typedef ShaderHandle_t * StaticShaderHandle_t;

#include "tier0/threadtools.h"

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

extern CThreadFastMutex g_ShaderAPIMutex;

#endif