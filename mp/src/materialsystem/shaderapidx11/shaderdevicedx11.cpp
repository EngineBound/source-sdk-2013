
#include "shaderdevicedx11.h"
#include "shaderapidx11.h"
#include "shaderdevicemgrdx11.h"

#include "shaderapidx11_global.h"

#define RELEASE_AND_NULLIFY_P(var) if (var) { \
	var->Release(); \
	var = NULL; \
	}

extern CShaderAPIDX11 *g_pShaderAPIDx11;
extern CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDx11;

static CShaderDeviceDX11 s_ShaderDeviceDx11;
CShaderDeviceDX11 *g_pShaderDeviceDx11 = &s_ShaderDeviceDx11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceDX11, IShaderDevice,
	SHADER_DEVICE_INTERFACE_VERSION, s_ShaderDeviceDx11)

ID3D11Device *g_pD3DDevice = NULL;
ID3D11DeviceContext *g_pD3DDeviceContext = NULL;
IDXGISwapChain *g_pD3DSwapChain = NULL;

CShaderDeviceDX11::CShaderDeviceDX11()
{
	m_nCurrentAdapter = -1;
	m_pDXGISwapChain = NULL;
	m_pDXGIOutput = NULL;
	m_pDXGIDevice = NULL;
	m_pDXGIDeviceContext = NULL;

	m_bDeviceInitialized = false;

	m_CurrenthWnd = NULL;
	m_nWndWidth = -1;
	m_nWndHeight = -1;
}

bool CShaderDeviceDX11::Initialize(void *hWnd, int nAdapter, const ShaderDeviceInfo_t mode)
{
	Msg("Initializing shader device :}\n");

	if (m_bDeviceInitialized)
		return false;

	IDXGIAdapter *pAdapter = g_pShaderDeviceMgrDx11->GetAdapter(nAdapter);
	if (!pAdapter)
		return false;

	m_pDXGIOutput = g_pShaderDeviceMgrDx11->GetAdapterOutput(nAdapter);
	if (!m_pDXGIOutput)
		return false;
	m_pDXGIOutput->AddRef();

	DXGI_SWAP_CHAIN_DESC swapDesc;
	V_memset(&swapDesc, 0, sizeof(swapDesc));
	swapDesc.BufferDesc.Width = mode.m_DisplayMode.m_nWidth;
	swapDesc.BufferDesc.Height = mode.m_DisplayMode.m_nHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapDesc.BufferDesc.RefreshRate.Numerator = mode.m_DisplayMode.m_nRefreshRateNumerator;
	swapDesc.BufferDesc.RefreshRate.Denominator = mode.m_DisplayMode.m_nRefreshRateDenominator;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapDesc.BufferCount = mode.m_nBackBufferCount;
	swapDesc.OutputWindow = (HWND)hWnd;
	swapDesc.Windowed = mode.m_bWindowed;
	swapDesc.Flags = mode.m_bWindowed ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SwapEffect = mode.m_nBackBufferCount > 1 ? DXGI_SWAP_EFFECT_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.SampleDesc.Count = max(mode.m_nAASamples, 1);
	swapDesc.SampleDesc.Quality = mode.m_nAAQuality;

	UINT nDeviceFlags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
		NULL, nDeviceFlags, NULL, 0, D3D11_SDK_VERSION, &swapDesc, &m_pDXGISwapChain,
		&m_pDXGIDevice, NULL, &m_pDXGIDeviceContext);

	if (FAILED(hr))
		return false;

	g_pD3DDevice = m_pDXGIDevice;
	g_pD3DDeviceContext = m_pDXGIDeviceContext;
	g_pD3DSwapChain = m_pDXGISwapChain;

	m_CurrenthWnd = hWnd;
	m_nCurrentAdapter = nAdapter;

	GetWindowSize(m_nWndHeight, m_nWndHeight);

	m_bDeviceInitialized = true;
	return true;
}

void CShaderDeviceDX11::Shutdown()
{
	RELEASE_AND_NULLIFY_P(m_pDXGIDeviceContext);
	RELEASE_AND_NULLIFY_P(m_pDXGIDevice);
	RELEASE_AND_NULLIFY_P(m_pDXGISwapChain);
	RELEASE_AND_NULLIFY_P(m_pDXGIOutput);

	m_CurrenthWnd = NULL;
	m_bDeviceInitialized = false;
}

// Releases/reloads resources when other apps want some memory
void CShaderDeviceDX11::ReleaseResources()
{
	return;
}

void CShaderDeviceDX11::ReacquireResources()
{
	return;
}


// returns the backbuffer format and dimensions
ImageFormat CShaderDeviceDX11::GetBackBufferFormat() const
{
	return IMAGE_FORMAT_RGBA8888; // IMPORTANT: CHANGE LATER
}

void CShaderDeviceDX11::GetBackBufferDimensions(int& width, int& height) const
{
	// IMPORTANT: CHANGE LATER
	width = 1024;
	height = 768;

	return;
}


// Returns the current adapter in use
int CShaderDeviceDX11::GetCurrentAdapter() const
{
	return m_nCurrentAdapter;
}


// Are we using graphics?
bool CShaderDeviceDX11::IsUsingGraphics() const
{
	return false;
}


// Use this to spew information about the 3D layer 
void CShaderDeviceDX11::SpewDriverInfo() const
{
	Msg("Lole\n");

	return;
}


// What's the bit depth of the stencil buffer?
int CShaderDeviceDX11::StencilBufferBits() const
{
	return 0; // IMPORTANT: CHANGE LATER
}


// Are we using a mode that uses MSAA
bool CShaderDeviceDX11::IsAAEnabled() const
{
	return false;
}


// Does a page flip
void CShaderDeviceDX11::Present()
{
	g_pShaderAPIDx11->FlushBufferedPrimitives();

	HRESULT hr = m_pDXGISwapChain->Present(0, 0);
	if (FAILED(hr))
		return;

	return;
}


// Returns the window size
void CShaderDeviceDX11::GetWindowSize(int &nWidth, int &nHeight) const
{
	return;
}


// Gamma ramp control
void CShaderDeviceDX11::SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled)
{
	return;
}


// Creates/ destroys a child window
bool CShaderDeviceDX11::AddView(void* hWnd)
{
	if (!m_pDXGIDevice)
		return false;

	return false;
}

void CShaderDeviceDX11::RemoveView(void* hWnd)
{
	return;
}


// Activates a view
void CShaderDeviceDX11::SetView(void* hWnd)
{
	return;
}


// Shader compilation
IShaderBuffer* CShaderDeviceDX11::CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	return (IShaderBuffer *)0;
}


// Shader creation, destruction
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(IShaderBuffer* pShaderBuffer)
{
	return VERTEX_SHADER_HANDLE_INVALID;
}

void CShaderDeviceDX11::DestroyVertexShader(VertexShaderHandle_t hShader)
{
	return;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(IShaderBuffer* pShaderBuffer)
{
	return GEOMETRY_SHADER_HANDLE_INVALID;
}

void CShaderDeviceDX11::DestroyGeometryShader(GeometryShaderHandle_t hShader)
{
	return;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(IShaderBuffer* pShaderBuffer)
{
	return PIXEL_SHADER_HANDLE_INVALID;
}

void CShaderDeviceDX11::DestroyPixelShader(PixelShaderHandle_t hShader)
{
	return;
}


// Utility methods to make shader creation simpler
// NOTE: For the utlbuffer version, use a binary buffer for a compiled shader
// and a text buffer for a source-code (.fxc) shader
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	return VERTEX_SHADER_HANDLE_INVALID;
}

VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	return VERTEX_SHADER_HANDLE_INVALID;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	return GEOMETRY_SHADER_HANDLE_INVALID;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	return GEOMETRY_SHADER_HANDLE_INVALID;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	return PIXEL_SHADER_HANDLE_INVALID;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	return PIXEL_SHADER_HANDLE_INVALID;
}


// NOTE: Deprecated!! Use CreateVertexBuffer/CreateIndexBuffer instead
// Creates/destroys Mesh
IMesh* CShaderDeviceDX11::CreateStaticMesh(VertexFormat_t vertexFormat, const char *pTextureBudgetGroup, IMaterial * pMaterial)
{
	return (IMesh *)0;
}

void CShaderDeviceDX11::DestroyStaticMesh(IMesh* mesh)
{
	return;
}


// Creates/destroys static vertex + index buffers
IVertexBuffer *CShaderDeviceDX11::CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{
	return (IVertexBuffer *)0;
}

void CShaderDeviceDX11::DestroyVertexBuffer(IVertexBuffer *pVertexBuffer)
{
	return;
}


IIndexBuffer *CShaderDeviceDX11::CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup)
{
	return (IIndexBuffer *)0;
}

void CShaderDeviceDX11::DestroyIndexBuffer(IIndexBuffer *pIndexBuffer)
{
	return;
}


// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
IVertexBuffer *CShaderDeviceDX11::GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered)
{
	return (IVertexBuffer *)0;
}

IIndexBuffer *CShaderDeviceDX11::GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered)
{
	return (IIndexBuffer *)0;
}


// A special path used to tick the front buffer while loading on the 360
void CShaderDeviceDX11::EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo)
{
	return;
}

void CShaderDeviceDX11::RefreshFrontBufferNonInteractive()
{
	return;
}

void CShaderDeviceDX11::HandleThreadEvent(uint32 threadEvent)
{
	return;
}


#ifdef DX_TO_GL_ABSTRACTION
void CShaderDeviceDX11::DoStartupShaderPreloading(void)
{
	return;
}

#endif
char *CShaderDeviceDX11::GetDisplayDeviceName()
{
	return "Lole";
}
