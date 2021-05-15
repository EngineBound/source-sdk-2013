
#include "shaderdevicedx11.h"
#include "dx11global.h"

#include "meshdx11.h"
#include "hardwareconfigdx11.h"
#include "shaderdevicemgrdx11.h"
#include "shaderapidx11.h"
#include "constantbufferdx11.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include "memdbgon.h"

class CShaderBufferDX11 : public IShaderBuffer
{
public:
	CShaderBufferDX11(ID3DBlob *pBlob)
	{
		m_pBlob = pBlob;
	}

	virtual size_t GetSize() const
	{
		if (!m_pBlob) return 0;
		return m_pBlob->GetBufferSize();
	}
	virtual const void* GetBits() const
	{
		if (!m_pBlob) return NULL;
		return m_pBlob->GetBufferPointer();
	}
	virtual void Release()
	{
		if (!m_pBlob) return;
		m_pBlob->Release();
		m_pBlob = NULL;
		delete this;
	}
private:
	ID3DBlob *m_pBlob;
};

static CShaderDeviceDX11 s_ShaderDeviceDX11;
CShaderDeviceDX11 *g_pShaderDeviceDX11 = &s_ShaderDeviceDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderDeviceDX11, IShaderDeviceDX11,
	SHADER_DEVICE_INTERFACE_VERSION, s_ShaderDeviceDX11)

IShaderDeviceDX11 *g_pShaderDevice = g_pShaderDeviceDX11;

extern CHardwareConfigDX11* g_pHardwareConfigDX11;
extern CShaderDeviceMgrDX11* g_pShaderDeviceMgrDX11;

CShaderDeviceDX11::CShaderDeviceDX11()
{
	Shutdown();

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

	AUTO_LOCK_FM(g_ShaderAPIMutex);

	IDXGIAdapter *pAdapter = g_pShaderDeviceMgrDX11->GetAdapter(nAdapter);
	if (!pAdapter)
		return false;

	m_pDXGIOutput = g_pShaderDeviceMgrDX11->GetAdapterOutput(nAdapter);
	if (!m_pDXGIOutput)
		return false;
	m_pDXGIOutput->AddRef();

	m_hWnd = (HWND)hWnd;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	V_memset(&swapChainDesc, 0, sizeof(swapChainDesc));

	swapChainDesc.BufferDesc.Width = mode.m_DisplayMode.m_nWidth;
	swapChainDesc.BufferDesc.Height = mode.m_DisplayMode.m_nHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // May need to change
	swapChainDesc.BufferDesc.RefreshRate.Denominator = mode.m_DisplayMode.m_nRefreshRateDenominator;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = mode.m_DisplayMode.m_nRefreshRateNumerator;

	swapChainDesc.SampleDesc.Count = max(mode.m_nAASamples, 1);
	swapChainDesc.SampleDesc.Quality = mode.m_nAAQuality;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	swapChainDesc.BufferCount = mode.m_nBackBufferCount;

	swapChainDesc.OutputWindow = m_hWnd;

	swapChainDesc.Windowed = mode.m_bWindowed;

	swapChainDesc.SwapEffect = mode.m_nBackBufferCount > 1 ? DXGI_SWAP_EFFECT_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = mode.m_bWindowed ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT deviceFlags = 0;

#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, deviceFlags, NULL, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &m_pDXGISwapChain, &m_pD3DDevice, NULL, &m_pD3DDeviceContext);
	if (FAILED(hr))
		return false;

	m_nAdapter = nAdapter;

	g_pHardwareConfigDX11->SetInfo(g_pShaderDeviceMgrDX11->GetHWInfo(m_nAdapter));

	m_bDeviceInitialized = true;

	g_pShaderDevice = g_pShaderDeviceDX11;
	g_pShaderAPIDX11->OnDeviceInitialised();
	return true;
}

void CShaderDeviceDX11::Shutdown()
{
	if (!m_bDeviceInitialized) return;

	m_nAdapter = -1;

	if (m_pDXGIOutput)
	{
		m_pDXGIOutput->Release();
		m_pDXGIOutput = NULL;
	}

	if (m_pDXGISwapChain)
	{
		m_pDXGISwapChain->Release();
		m_pDXGISwapChain = NULL;
	}

	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if (m_pD3DDeviceContext)
	{
		m_pD3DDeviceContext->Release();
		m_pD3DDeviceContext = NULL;
	}

	for (int i = 0; i < m_VertexShaders.Count(); ++i)
	{
		m_VertexShaders[i].m_pInputLayout->Release();
		m_VertexShaders[i].m_pReflection->Release();
		m_VertexShaders[i].m_pShader->Release();

		free(m_VertexShaders[i].m_pShaderByteCode);
	}

	for (int i = 0; i < m_PixelShaders.Count(); ++i)
	{
		m_PixelShaders[i].m_pReflection->Release();
		m_PixelShaders[i].m_pShader->Release();
	}

	m_bDeviceInitialized = false;

	g_pShaderAPIDX11->OnDeviceShutdown();
}

// Releases/reloads resources when other apps want some memory
void CShaderDeviceDX11::ReleaseResources()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderDeviceDX11::ReacquireResources()
{
	ALERT_NOT_IMPLEMENTED();
}


// returns the backbuffer format and dimensions
ImageFormat CShaderDeviceDX11::GetBackBufferFormat() const
{
	ALERT_INCOMPLETE();
	return IMAGE_FORMAT_RGBA8888;
}

void CShaderDeviceDX11::GetBackBufferDimensions(int& width, int& height) const
{
	DXGI_SWAP_CHAIN_DESC swapDesc;
	m_pDXGISwapChain->GetDesc(&swapDesc);
	height = swapDesc.BufferDesc.Height;
	width = swapDesc.BufferDesc.Width;
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
	ALERT_NOT_IMPLEMENTED();
}


// What's the bit depth of the stencil buffer?
int CShaderDeviceDX11::StencilBufferBits() const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


// Are we using a mode that uses MSAA
bool CShaderDeviceDX11::IsAAEnabled() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Does a page flip
void CShaderDeviceDX11::Present()
{
	ALERT_INCOMPLETE();

#ifdef DEBUG
	HRESULT hr =
#endif
		m_pDXGISwapChain->Present(0, 0);
	Assert(!FAILED(hr));
}


// Returns the window size
void CShaderDeviceDX11::GetWindowSize(int &nWidth, int &nHeight) const
{
	if (!IsIconic(m_hWnd))
	{
		RECT rect;
		GetClientRect((HWND)m_hWnd, &rect);
		nWidth = rect.right - rect.left;
		nHeight = rect.bottom - rect.top;
	}
	else
		nWidth = nHeight = 0;
}


// Gamma ramp control
void CShaderDeviceDX11::SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled)
{
	ALERT_NOT_IMPLEMENTED();
}


// Creates/ destroys a child window
bool CShaderDeviceDX11::AddView(void* hWnd)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CShaderDeviceDX11::RemoveView(void* hWnd)
{
	ALERT_NOT_IMPLEMENTED();
}


// Activates a view
void CShaderDeviceDX11::SetView(void* hWnd)
{
	ALERT_NOT_IMPLEMENTED();
}


// Shader compilation
IShaderBuffer* CShaderDeviceDX11::CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	UINT nCompileFlags = D3DCOMPILE_AVOID_FLOW_CONTROL | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

#ifdef DEBUG
	nCompileFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pCode;
	ID3DBlob* pErrorMsgs;
	HRESULT hr = D3DCompile(pProgram, nBufLen, "", NULL, NULL, "main", pShaderVersion, nCompileFlags, 0, &pCode, &pErrorMsgs);
	if (FAILED(hr))
	{
		if (pErrorMsgs)
		{
			const char *pMsg = (const char *)pErrorMsgs->GetBufferPointer();
			Warning("Shader compilation failed:\n\t%s\n", pMsg);
			pErrorMsgs->Release();
		}
		return NULL;
	}

	CShaderBufferDX11 *pShaderBuffer = new CShaderBufferDX11(pCode);
	if (pErrorMsgs)
	{
		pErrorMsgs->Release();
	}

	return pShaderBuffer;
}


// Shader creation, destruction
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(IShaderBuffer* pShaderBuffer)
{
	return CreateVertexShader((const char *)pShaderBuffer->GetBits(), pShaderBuffer->GetSize(), "vs_5_0");
}

void CShaderDeviceDX11::DestroyVertexShader(VertexShaderHandle_t hShader)
{
	if (hShader == VERTEX_SHADER_HANDLE_INVALID)
		return;

	g_pShaderAPIDX11->UnbindVertexShader(hShader);

	VertexRepIndex_t ind = (VertexRepIndex_t)hShader;
	VertexShaderRep_t &rep = m_VertexShaders[ind];
	rep.m_pShader->Release();
	rep.m_pReflection->Release();
	
	if (rep.m_pInputLayout)
		rep.m_pInputLayout->Release();
	free(rep.m_pShaderByteCode);

	m_VertexShaders.Remove(ind);
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(IShaderBuffer* pShaderBuffer)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

void CShaderDeviceDX11::DestroyGeometryShader(GeometryShaderHandle_t hShader)
{
	ALERT_NOT_IMPLEMENTED();
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(IShaderBuffer* pShaderBuffer)
{
	return CreatePixelShader((const char *)pShaderBuffer->GetBits(), pShaderBuffer->GetSize(), "ps_5_0");
}

void CShaderDeviceDX11::DestroyPixelShader(PixelShaderHandle_t hShader)
{
	if (hShader == PIXEL_SHADER_HANDLE_INVALID)
		return;

	g_pShaderAPIDX11->UnbindPixelShader(hShader);

	PixelRepIndex_t ind = (PixelRepIndex_t)hShader;
	PixelShaderRep_t &rep = m_PixelShaders[ind];
	rep.m_pShader->Release();
	rep.m_pReflection->Release();

	m_PixelShaders.Remove(ind);
}

struct InputElementRep_t
{
	VertexFormat_t m_VertexFormatMask;
	char *m_pSemanticName;
	UINT m_nSemanticIndex;
	DXGI_FORMAT m_Format;
	int m_nElementSize;
	int m_nFallbackOffset;
};

static InputElementRep_t s_pInputElements[]
{
	{ VERTEX_POSITION,			"POSITION",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	12,	0	}, // sizeof won't work here because the number of bits is specified
	{ VERTEX_BONE_WEIGHT_MASK,	"BLENDWEIGHT",	0,	DXGI_FORMAT_UNKNOWN,			0,	12	},
	{ VERTEX_BONE_INDEX,		"BLENDINDICES",	0,	DXGI_FORMAT_R8G8B8A8_UINT,		4,	20 	},
	{ VERTEX_NORMAL,			"NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	12,	24	},
	{ VERTEX_COLOR,				"COLOR",		0,	DXGI_FORMAT_B8G8R8A8_UNORM,		4,	36	},
	{ VERTEX_SPECULAR,			"SPECULAR",		0,	DXGI_FORMAT_B8G8R8A8_UNORM,		4,	40	},
	{ VERTEX_TEXCOORD_MASK(0),	"TEXCOORD",		0,	DXGI_FORMAT_UNKNOWN,			-1,	44	},
	{ VERTEX_TEXCOORD_MASK(1),	"TEXCOORD",		1,	DXGI_FORMAT_UNKNOWN,			-1,	52	},
	{ VERTEX_TEXCOORD_MASK(2),	"TEXCOORD",		2,	DXGI_FORMAT_UNKNOWN,			-1,	60	},
	{ VERTEX_TEXCOORD_MASK(3),	"TEXCOORD",		3,	DXGI_FORMAT_UNKNOWN,			-1,	68	},
	{ VERTEX_TEXCOORD_MASK(4),	"TEXCOORD",		4,	DXGI_FORMAT_UNKNOWN,			-1,	76	},
	{ VERTEX_TEXCOORD_MASK(5),	"TEXCOORD",		5,	DXGI_FORMAT_UNKNOWN,			-1,	84	},
	{ VERTEX_TEXCOORD_MASK(6),	"TEXCOORD",		6,	DXGI_FORMAT_UNKNOWN,			-1,	92	},
	{ VERTEX_TEXCOORD_MASK(7),	"TEXCOORD",		7,	DXGI_FORMAT_UNKNOWN,			-1,	100	},
	{ VERTEX_TANGENT_S,			"TANGENT",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	12,	108	},
	{ VERTEX_TANGENT_T,			"BINORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	12,	120	},
	{ USER_DATA_SIZE_MASK,		"USERDATA",		0,	DXGI_FORMAT_UNKNOWN,			-1,	132	},
};

ID3D11InputLayout *CShaderDeviceDX11::GetInputLayout(VertexShaderHandle_t hVertexShader, VertexFormat_t fmt)
{
	ID3D11InputLayout *pInputLayout = m_VertexShaders[(VertexRepIndex_t)hVertexShader].m_pInputLayout;
	if (!pInputLayout)
	{
		VertexShaderRep_t& rep = m_VertexShaders[(VertexRepIndex_t)hVertexShader];

		D3D11_SHADER_DESC shaderDesc;
		rep.m_pReflection->GetDesc(&shaderDesc);

		D3D11_INPUT_ELEMENT_DESC pDescs[32];

		int nNumDescs = 0;
		int nOffset = 0;
		for (int i = 0; i < sizeof(s_pInputElements) / sizeof(InputElementRep_t); ++i)
		{
			InputElementRep_t &element = s_pInputElements[i];
			D3D11_INPUT_ELEMENT_DESC &desc = pDescs[nNumDescs];

			if (fmt & element.m_VertexFormatMask)
			{
				nOffset += PopulateProvidedDesc(element, fmt, nOffset, desc);
				nNumDescs++;
			}
			else
			{
				for (UINT j = 0; j < shaderDesc.InputParameters; j++)
				{
					D3D11_SIGNATURE_PARAMETER_DESC sigDesc;
					rep.m_pReflection->GetInputParameterDesc(j, &sigDesc);

					if (sigDesc.SemanticIndex == element.m_nSemanticIndex && !V_strcmp(sigDesc.SemanticName, element.m_pSemanticName))
					{
						PopulateFallbackDesc(element, desc);
						nNumDescs++;
						break;
					}
				}
			}
		}

		HRESULT hr = m_pD3DDevice->CreateInputLayout(pDescs, nNumDescs, rep.m_pShaderByteCode, rep.m_nByteCodeSize, &pInputLayout);
		if (FAILED(hr) || !pInputLayout)
		{
			Assert(0);
			rep.m_pReflection->Release();
			rep.m_pShader->Release();
			return NULL;
		}

		m_VertexShaders[(VertexRepIndex_t)hVertexShader].m_pInputLayout = pInputLayout;
	}

	return pInputLayout;
}

static DXGI_FORMAT s_pSizeToFormat[]
{
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
};

int CShaderDeviceDX11::PopulateProvidedDesc(InputElementRep_t &element, VertexFormat_t fmt, int nOffset, D3D11_INPUT_ELEMENT_DESC &desc)
{
	int elementSize = element.m_nElementSize;
	DXGI_FORMAT format = element.m_Format;
	if (elementSize < 0)
	{
		if (element.m_VertexFormatMask & USER_DATA_SIZE_MASK)
		{
			elementSize = UserDataSize(fmt);
		}
		else
		{
			elementSize = TexCoordSize(element.m_nSemanticIndex, fmt);
		}

		format = s_pSizeToFormat[elementSize];
		elementSize *= 4; // 32 bit float
	}

	desc.SemanticName = element.m_pSemanticName;
	desc.SemanticIndex = element.m_nSemanticIndex;
	desc.Format = format;
	desc.InputSlot = 0;
	desc.AlignedByteOffset = nOffset;
	desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc.InstanceDataStepRate = 0;

	return elementSize;
}

void CShaderDeviceDX11::PopulateFallbackDesc(InputElementRep_t &element, D3D11_INPUT_ELEMENT_DESC &desc)
{
	DXGI_FORMAT format = element.m_Format;
	if (format == DXGI_FORMAT_UNKNOWN)
		format = (element.m_VertexFormatMask & USER_DATA_SIZE_MASK) ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32_FLOAT;

	desc.SemanticName = element.m_pSemanticName;
	desc.SemanticIndex = element.m_nSemanticIndex;
	desc.Format = format;
	desc.InputSlot = 15;
	desc.AlignedByteOffset = element.m_nFallbackOffset;
	desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	desc.InstanceDataStepRate = UINT_MAX;
}

// Utility methods to make shader creation simpler
// NOTE: For the utlbuffer version, use a binary buffer for a compiled shader
// and a text buffer for a source-code (.fxc) shader
VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	ID3D11VertexShader* pVertexShader = NULL;
	HRESULT hr = m_pD3DDevice->CreateVertexShader(pProgram, nBufLen, NULL, &pVertexShader);
	if (FAILED(hr) || !pVertexShader)
	{
		Assert(0);
		return VERTEX_SHADER_HANDLE_INVALID;
	}

	ID3D11ShaderReflection* pReflection = NULL;
	hr = D3DReflect(pProgram, nBufLen, IID_ID3D11ShaderReflection, (void **)&pReflection);
	if (FAILED(hr) || !pReflection)
	{
		Assert(0);
		pVertexShader->Release();
		return VERTEX_SHADER_HANDLE_INVALID;
	}

	VertexRepIndex_t ind = m_VertexShaders.AddToTail();
	VertexShaderRep_t& rep = m_VertexShaders[ind];

	rep.m_pShader = pVertexShader;
	rep.m_pReflection = pReflection;
	rep.m_pInputLayout = NULL;

	rep.m_nByteCodeSize = nBufLen;
	rep.m_pShaderByteCode = malloc(rep.m_nByteCodeSize * sizeof(char));
	memcpy(rep.m_pShaderByteCode, pProgram, rep.m_nByteCodeSize);

	return (VertexShaderHandle_t)ind;
}

VertexShaderHandle_t CShaderDeviceDX11::CreateVertexShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

GeometryShaderHandle_t CShaderDeviceDX11::CreateGeometryShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion)
{
	ID3D11PixelShader* pPixelShader = NULL;
	HRESULT hr = m_pD3DDevice->CreatePixelShader(pProgram, nBufLen, NULL, &pPixelShader);
	if (FAILED(hr) || !pPixelShader)
	{
		Assert(0);
		return PIXEL_SHADER_HANDLE_INVALID;
	}

	ID3D11ShaderReflection* pReflection;
	hr = D3DReflect(pProgram, nBufLen, IID_ID3D11ShaderReflection, (void **)&pReflection);
	if (FAILED(hr) || !pReflection)
	{
		Assert(0);
		pPixelShader->Release();
		return PIXEL_SHADER_HANDLE_INVALID;
	}

	PixelRepIndex_t ind = m_PixelShaders.AddToTail();
	PixelShaderRep_t& rep = m_PixelShaders[ind];

	rep.m_pShader = pPixelShader;
	rep.m_pReflection = pReflection;

	return (PixelShaderHandle_t)ind;
}

PixelShaderHandle_t CShaderDeviceDX11::CreatePixelShader(CUtlBuffer &buf, const char *pShaderVersion)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

// NOTE: Deprecated!! Use CreateVertexBuffer/CreateIndexBuffer instead
// Creates/destroys Mesh
IMesh* CShaderDeviceDX11::CreateStaticMesh(VertexFormat_t vertexFormat, const char *pTextureBudgetGroup, IMaterial * pMaterial)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_INCOMPLETE();
	return new CMeshDX11(false, vertexFormat);
}

void CShaderDeviceDX11::DestroyStaticMesh(IMesh* mesh)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_INCOMPLETE();
	delete mesh;
}

// Creates/destroys static vertex + index buffers
IVertexBuffer *CShaderDeviceDX11::CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	CVertexBufferDX11 *outBuf = new CVertexBufferDX11(type, fmt, nVertexCount, pBudgetGroup);
	outBuf->CreateBuffer();
	return outBuf;
}

void CShaderDeviceDX11::DestroyVertexBuffer(IVertexBuffer *pVertexBuffer)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);
	delete pVertexBuffer;
}


IIndexBuffer *CShaderDeviceDX11::CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	CIndexBufferDX11 *outBuf = new CIndexBufferDX11(bufferType, fmt, nIndexCount, pBudgetGroup);
	outBuf->CreateBuffer();

	return outBuf;
}

void CShaderDeviceDX11::DestroyIndexBuffer(IIndexBuffer *pIndexBuffer)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	delete pIndexBuffer;
}

IConstantBufferDX11 *CShaderDeviceDX11::CreateConstantBuffer(ShaderBufferType_t bufferType, int nBufferSize, const char *pBudgetGroup)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	CConstantBufferDX11 *outBuf = new CConstantBufferDX11(bufferType, nBufferSize, pBudgetGroup);
	outBuf->CreateBuffer();

	return outBuf;
}

void CShaderDeviceDX11::DestroyConstantBuffer(IConstantBufferDX11 *pConstantBuffer)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	delete pConstantBuffer;
}

// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
IVertexBuffer *CShaderDeviceDX11::GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

IIndexBuffer *CShaderDeviceDX11::GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return NULL;
}


// A special path used to tick the front buffer while loading on the 360
void CShaderDeviceDX11::EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderDeviceDX11::RefreshFrontBufferNonInteractive()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderDeviceDX11::HandleThreadEvent(uint32 threadEvent)
{
	ALERT_NOT_IMPLEMENTED();
}


// Don't do this man we can use dxvk
#ifdef DX_TO_GL_ABSTRACTION
void CShaderDeviceDX11::DoStartupShaderPreloading(void)
{
	ALERT_NOT_IMPLEMENTED();
}

#endif

char *CShaderDeviceDX11::GetDisplayDeviceName()
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

bool CShaderDeviceDX11::IsActivated() const
{
	return m_bDeviceInitialized;
}
