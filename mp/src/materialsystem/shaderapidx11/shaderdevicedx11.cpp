
#include "shaderdevicedx11.h"
#include "meshdx11.h"

static CShaderDeviceDX11 s_ShaderDeviceDX11;
CShaderDeviceDX11 *g_pShaderDeviceDX11 = &s_ShaderDeviceDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceDX11, IShaderDeviceDX11,
	SHADER_DEVICE_INTERFACE_VERSION, s_ShaderDeviceDX11)


// Releases/reloads resources when other apps want some memory
void CShaderDeviceDX11::ReleaseResources()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderDeviceDX11::ReacquireResources()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// returns the backbuffer format and dimensions
ImageFormat CShaderDeviceDX11::GetBackBufferFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return IMAGE_FORMAT_UNKNOWN;
}

void CShaderDeviceDX11::GetBackBufferDimensions(int& width, int& height) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the current adapter in use
int CShaderDeviceDX11::GetCurrentAdapter() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Are we using graphics?
bool CShaderDeviceDX11::IsUsingGraphics() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Use this to spew information about the 3D layer 
void CShaderDeviceDX11::SpewDriverInfo() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// What's the bit depth of the stencil buffer?
int CShaderDeviceDX11::StencilBufferBits() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Are we using a mode that uses MSAA
bool CShaderDeviceDX11::IsAAEnabled() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Does a page flip
void CShaderDeviceDX11::Present()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the window size
void CShaderDeviceDX11::GetWindowSize(int &nWidth, int &nHeight) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Gamma ramp control
void CShaderDeviceDX11::SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Creates/ destroys a child window
bool CShaderDeviceDX11::AddView(void* hWnd)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CShaderDeviceDX11::RemoveView(void* hWnd)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Activates a view
void CShaderDeviceDX11::SetView(void* hWnd)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Shader compilation
IShaderBuffer* CShaderDeviceDX11::CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


// Shader creation, destruction
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(IShaderBuffer* pShaderBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyVertexShader(VertexShaderHandle_t hShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(IShaderBuffer* pShaderBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyGeometryShader(GeometryShaderHandle_t hShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(IShaderBuffer* pShaderBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyPixelShader(PixelShaderHandle_t hShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Utility methods to make shader creation simpler
// NOTE: For the utlbuffer version, use a binary buffer for a compiled shader
// and a text buffer for a source-code (.fxc) shader
VertexShaderHandle_t CreateVertexShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

VertexShaderHandle_t CreateVertexShader(CUtlBuffer &buf, const char *pShaderVersion = NULL)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

GeometryShaderHandle_t CreateGeometryShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

GeometryShaderHandle_t CreateGeometryShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

PixelShaderHandle_t CreatePixelShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

PixelShaderHandle_t CreatePixelShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

// NOTE: Deprecated!! Use CreateVertexBuffer/CreateIndexBuffer instead
// Creates/destroys Mesh
IMesh* CShaderDeviceDX11::CreateStaticMesh(VertexFormat_t vertexFormat, const char *pTextureBudgetGroup, IMaterial * pMaterial)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyStaticMesh(IMesh* mesh)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Creates/destroys static vertex + index buffers
IVertexBuffer *CShaderDeviceDX11::CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyVertexBuffer(IVertexBuffer *pVertexBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


IIndexBuffer *CShaderDeviceDX11::CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderDeviceDX11::DestroyIndexBuffer(IIndexBuffer *pIndexBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
IVertexBuffer *CShaderDeviceDX11::GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

IIndexBuffer *CShaderDeviceDX11::GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


// A special path used to tick the front buffer while loading on the 360
void CShaderDeviceDX11::EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderDeviceDX11::RefreshFrontBufferNonInteractive()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderDeviceDX11::HandleThreadEvent(uint32 threadEvent)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Don't do this man we can use dxvk
#ifdef DX_TO_GL_ABSTRACTION
void CShaderDeviceDX11::DoStartupShaderPreloading(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

#endif

char *CShaderDeviceDX11::GetDisplayDeviceName()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}
