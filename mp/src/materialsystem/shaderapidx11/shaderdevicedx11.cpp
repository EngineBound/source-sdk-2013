
#include "shaderdevicedx11.h"
#include "dx11global.h"

#include "meshdx11.h"
#include "shaderdevicemgrdx11.h"

static CShaderDeviceDX11 s_ShaderDeviceDX11;
CShaderDeviceDX11 *g_pShaderDeviceDX11 = &s_ShaderDeviceDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceDX11, IShaderDeviceDX11,
	SHADER_DEVICE_INTERFACE_VERSION, s_ShaderDeviceDX11)

IShaderDeviceDX11 *g_pShaderDevice = g_pShaderDeviceDX11;


extern CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11;

CShaderDeviceDX11::CShaderDeviceDX11()
{
	m_nAdapter = -1;
	m_pDXGIOutput = NULL;
	m_pDXGISwapChain = NULL;
	m_pD3DDevice = NULL;
	m_pD3DDeviceContext = NULL;

	m_bDeviceInitialized = false;
}

bool CShaderDeviceDX11::Init(void *hWnd, int nAdapter, const ShaderDeviceInfo_t mode)
{
	if (m_bDeviceInitialized) 
		return false;

	IDXGIAdapter *pAdapter = g_pShaderDeviceMgrDX11->GetAdapter(nAdapter);
	if (!pAdapter)
		return false;

	m_pDXGIOutput = g_pShaderDeviceMgrDX11->GetAdapterOutput(nAdapter);
	if (!m_pDXGIOutput)
		return false;
	m_pDXGIOutput->AddRef();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	V_memset(&swapChainDesc, 0, sizeof(swapChainDesc));

	swapChainDesc.BufferDesc.Width = mode.m_DisplayMode.m_nWidth;
	swapChainDesc.BufferDesc.Height = mode.m_DisplayMode.m_nHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // May need to change
	swapChainDesc.BufferDesc.RefreshRate.Denominator = mode.m_DisplayMode.m_nRefreshRateDenominator;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = mode.m_DisplayMode.m_nRefreshRateNumerator;

	swapChainDesc.SampleDesc.Count = max(mode.m_nAASamples, 1);
	swapChainDesc.SampleDesc.Quality = mode.m_nAAQuality;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	swapChainDesc.BufferCount = mode.m_nBackBufferCount;

	swapChainDesc.OutputWindow = (HWND)hWnd;

	swapChainDesc.Windowed = mode.m_bWindowed;

	swapChainDesc.SwapEffect = mode.m_nBackBufferCount > 1 ? DXGI_SWAP_EFFECT_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = mode.m_bWindowed ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &m_pDXGISwapChain, &m_pD3DDevice, NULL, &m_pD3DDeviceContext);
	if (FAILED(hr))
		return false;

	m_nAdapter = nAdapter;

	m_bDeviceInitialized = true;
	return true;
}

void CShaderDeviceDX11::Shutdown()
{
	if (!m_bDeviceInitialized) return;

	m_nAdapter = -1;
	m_pDXGIOutput = NULL;
	m_pDXGISwapChain = NULL;
	m_pD3DDevice = NULL;
	m_pD3DDeviceContext = NULL;

	m_bDeviceInitialized = false;
}

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
	return m_nAdapter;
}


// Are we using graphics?
bool CShaderDeviceDX11::IsUsingGraphics() const
{
	return m_bDeviceInitialized;
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
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(CUtlBuffer &buf, const char *pShaderVersion)
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

bool CShaderDeviceDX11::IsActivated() const
{
	return m_bDeviceInitialized;
}