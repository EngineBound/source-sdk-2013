
#include "shaderdevicedx11.h"
#include "shaderapidx11.h"
#include "shaderdevicemgrdx11.h"

#include "shaderapidx11_global.h"

#define RELEASE_AND_NULLIFY_P(var) if (var) { \
	var->Release(); \
	var = NULL; \
	}

/*extern CShaderAPIDX11 *g_pShaderAPIDX11;
extern CShaderDeviceMgrDX11 *g_pShaderDeviceMgrDX11;*/

static CShaderDeviceDX11 s_ShaderDeviceDX11;
CShaderDeviceDX11 *g_pShaderDeviceDX11 = &s_ShaderDeviceDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceDX11, IShaderDevice,
	SHADER_DEVICE_INTERFACE_VERSION, s_ShaderDeviceDX11)

/*ID3D11Device *g_pD3DDevice = NULL;
ID3D11DeviceContext *g_pD3DDeviceContext = NULL;
IDXGISwapChain *g_pD3DSwapChain = NULL;*/

// Initialize shaderdevice vars to dummy values
CShaderDeviceDX11::CShaderDeviceDX11() : m_DynamicMesh( true ), m_Mesh( false )
{
	/*
	m_nCurrentAdapter = -1;
	m_pDXGISwapChain = NULL;
	m_pDXGIOutput = NULL;
	m_pDXGIDevice = NULL;
	m_pDXGIDeviceContext = NULL;

	m_bDeviceInitialized = false;

	m_CurrenthWnd = NULL;
	m_nWndWidth = -1;
	m_nWndHeight = -1;*/
}

// Initialize the device properly, given the window, adapter id and device mode.
bool CShaderDeviceDX11::Initialize(void *hWnd, int nAdapter, const ShaderDeviceInfo_t mode)
{
	/*
	// Development only
	Msg("Initializing shader device :}\n");

	// Don't initialize if already done
	if (m_bDeviceInitialized)
		return false;

	// Get adapter from device manager
	IDXGIAdapter *pAdapter = g_pShaderDeviceMgrDX11->GetAdapter(nAdapter);
	if (!pAdapter)
		return false;

	// Get output of adapter, and declare m_pDXGIOutput as a new reference of the output.
	m_pDXGIOutput = g_pShaderDeviceMgrDX11->GetAdapterOutput(nAdapter);
	if (!m_pDXGIOutput)
		return false;
	m_pDXGIOutput->AddRef();

	// Set up swapchain, weird that there isn't just a method for this, unless there is
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

	// Create the device and the swap-chain. Astounding.
	HRESULT hr = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
		NULL, nDeviceFlags, NULL, 0, D3D11_SDK_VERSION, &swapDesc, &m_pDXGISwapChain,
		&m_pDXGIDevice, NULL, &m_pDXGIDeviceContext);

	if (FAILED(hr))
		return false;

	// Update global variables to updated member pointers
	g_pD3DDevice = m_pDXGIDevice;
	g_pD3DDeviceContext = m_pDXGIDeviceContext;
	g_pD3DSwapChain = m_pDXGISwapChain;

	// Set member variables
	m_CurrenthWnd = hWnd;
	m_nCurrentAdapter = nAdapter;

	GetWindowSize(m_nWndHeight, m_nWndHeight);

	m_bDeviceInitialized = true;
	return true;*/
	return true;
}

void CShaderDeviceDX11::Shutdown()
{
	/*// Nullify member pointers
	RELEASE_AND_NULLIFY_P(m_pDXGIDeviceContext);
	RELEASE_AND_NULLIFY_P(m_pDXGIDevice);
	RELEASE_AND_NULLIFY_P(m_pDXGISwapChain);
	RELEASE_AND_NULLIFY_P(m_pDXGIOutput);

	m_CurrenthWnd = NULL;
	m_bDeviceInitialized = false;*/
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
	return IMAGE_FORMAT_RGB888; // IMPORTANT: CHANGE LATER
}

void CShaderDeviceDX11::GetBackBufferDimensions(int& width, int& height) const
{
	/*DXGI_SWAP_CHAIN_DESC desc;
	m_pDXGISwapChain->GetDesc(&desc);
	width = desc.BufferDesc.Width;
	height = desc.BufferDesc.Height;*/

	width = 1024;
	height = 768;
}


// Returns the current adapter in use
int CShaderDeviceDX11::GetCurrentAdapter() const
{
	return 0;// m_nCurrentAdapter;
}


// Are we using graphics?
bool CShaderDeviceDX11::IsUsingGraphics() const
{
	return false;// m_bDeviceInitialized;
}


// Use this to spew information about the 3D layer 
void CShaderDeviceDX11::SpewDriverInfo() const
{
	Msg("Lole Driver DX11\n");

	return;
}


// What's the bit depth of the stencil buffer?
int CShaderDeviceDX11::StencilBufferBits() const
{
	return 0; // IMPORTANT: CHANGE IF WE WANT STENCIL BUFFER
}


// Are we using a mode that uses MSAA
bool CShaderDeviceDX11::IsAAEnabled() const
{
	return false;
}


// Does a page flip, AKA shows the next frame
void CShaderDeviceDX11::Present()
{
	/*g_pShaderAPIDX11->FlushBufferedPrimitives();

	HRESULT hr = m_pDXGISwapChain->Present(0, 0);
	if (FAILED(hr))
		return;

	return;*/
}


// Returns the window size
void CShaderDeviceDX11::GetWindowSize(int &nWidth, int &nHeight) const
{
	// If window minimized (iconic), width = height = 0 :))
	/*if (!IsIconic((HWND)m_CurrenthWnd))
	{
		RECT rect;
		GetClientRect((HWND)m_CurrenthWnd, &rect);
		nWidth = rect.right - rect.left;
		nHeight = rect.bottom - rect.top;
	}
	else
	{
		nWidth = nHeight = 0;
	}*/

	nWidth = nHeight = 0;
}


// Gamma ramp control
void CShaderDeviceDX11::SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled)
{
	/*Assert(m_pDXGIOutput);
	if (!m_pDXGIOutput)
		return;

	// Store hw gamma vals
	float flMin = g_pHardwareConfigDX11->GetInfo().m_flMinGammaControlPoint;
	float flMax = g_pHardwareConfigDX11->GetInfo().m_flMaxGammaControlPoint;
	int nGammaCount = g_pHardwareConfigDX11->GetInfo().m_nGammaControlPointCount;

	// Create gamma ramp
	DXGI_GAMMA_CONTROL gammaControl;
	gammaControl.Scale.Red = gammaControl.Scale.Green = gammaControl.Scale.Red = 1.f;
	gammaControl.Offset.Red = gammaControl.Offset.Green = gammaControl.Offset.Red = 0.f;
	float flGammaDiff = 1.0f / (nGammaCount - 1);

	for (int i = 0; i < nGammaCount; i++)
	{
		float flGammaPos = i * flGammaDiff;
		float flCorrection = pow(flGammaPos, fGamma / 2.2f); // TODO: Update with mat_monitorgamma?
		flCorrection = clamp(flCorrection, flMin, flMax);

		gammaControl.GammaCurve[i].Red = flCorrection;
		gammaControl.GammaCurve[i].Green = flCorrection;
		gammaControl.GammaCurve[i].Blue = flCorrection;
	}

	// Set to created ramp
	HRESULT hr = m_pDXGIOutput->SetGammaControl(&gammaControl);
	if (FAILED(hr))
	{
		Warning("CShaderDeviceDX11::SetHardwareGammaRamp: Unable to set gamma controls!\n");
	}*/
}

// next 2 functions are related to child windows, not used in base source

// Creates/ destroys a child window
bool CShaderDeviceDX11::AddView(void* hWnd)
{
	/*if (!m_pDXGIDevice)
		return false;*/

	// Just do it i guess
	return true;
}

// Remove child window
void CShaderDeviceDX11::RemoveView(void* hWnd)
{

	return;
}


// Activates a view
void CShaderDeviceDX11::SetView(void* hWnd)
{
	/*ShaderViewport_t viewport;
	g_pShaderAPIDX11->GetViewports(&viewport, 1);
	
	// Get window
	m_CurrenthWnd = (HWND)hWnd;
	GetWindowSize(m_nWndWidth, m_nWndHeight);

	// Set viewport
	g_pShaderAPIDX11->SetViewports(1, &viewport);*/
}


// Shader compilation
IShaderBuffer* CShaderDeviceDX11::CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	// Don't worry about this FOR NOW CHANGE CHANGE CHANGE CHANGE
	return NULL;
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
	return &m_Mesh;
}

void CShaderDeviceDX11::DestroyStaticMesh(IMesh* mesh)
{
	return;
}


// Creates/destroys static vertex + index buffers
IVertexBuffer *CShaderDeviceDX11::CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{
	return (type == SHADER_BUFFER_TYPE_STATIC || type == SHADER_BUFFER_TYPE_STATIC_TEMP) ? &m_Mesh : &m_DynamicMesh;
}

void CShaderDeviceDX11::DestroyVertexBuffer(IVertexBuffer *pVertexBuffer)
{
	return;
}


IIndexBuffer *CShaderDeviceDX11::CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup)
{
	switch (bufferType)
	{
	case SHADER_BUFFER_TYPE_STATIC:
	case SHADER_BUFFER_TYPE_STATIC_TEMP:
		return &m_Mesh;
	default:
		Assert(0);
	case SHADER_BUFFER_TYPE_DYNAMIC:
	case SHADER_BUFFER_TYPE_DYNAMIC_TEMP:
		return &m_DynamicMesh;
	}
}

void CShaderDeviceDX11::DestroyIndexBuffer(IIndexBuffer *pIndexBuffer)
{
	return;
}


// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
IVertexBuffer *CShaderDeviceDX11::GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered)
{
	return &m_DynamicMesh;
}

IIndexBuffer *CShaderDeviceDX11::GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered)
{
	return &m_Mesh;
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
