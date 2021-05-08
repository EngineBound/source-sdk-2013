#include <windows.h>

#include "appframework/tier2app.h"
#include "tier0/ICommandLine.h"
#include "materialsystem/imesh.h"

#include "bitmap/imageformat.h"
#include "shaderapi/ishaderapi.h"
#include "shaderapi/IShaderDevice.h"
#include "shaderapi/ishadershadow.h"
#include "shaderapi/ishaderutil.h"
#include "materialsystem/materialsystem_config.h"

// So ShaderAPI doesn't scream about no shaderutil
class CShaderUtilSkeleton : public CBaseAppSystem<IShaderUtil>
{
public:
	// Method to allow clients access to the MaterialSystem_Config
	virtual MaterialSystem_Config_t& GetConfig()
	{
		static MaterialSystem_Config_t tmp;
		return tmp;
	}

	// Allows us to convert image formats
	virtual bool ConvertImageFormat(unsigned char *src, enum ImageFormat srcImageFormat,
		unsigned char *dst, enum ImageFormat dstImageFormat,
		int width, int height, int srcStride = 0, int dstStride = 0)
	{
		return false;
	}

	// Figures out the amount of memory needed by a bitmap
	virtual int GetMemRequired(int width, int height, int depth, ImageFormat format, bool mipmap) { return 0; }

	// Gets image format info
	virtual const ImageFormatInfo_t& ImageFormatInfo(ImageFormat fmt) const
	{
		static ImageFormatInfo_t tmp;
		return tmp;
	}

	// Bind standard textures
	virtual void BindStandardTexture(Sampler_t sampler, StandardTextureId_t id) { }

	// What are the lightmap dimensions?
	virtual void GetLightmapDimensions(int *w, int *h) { }

	// These methods are called when the shader must eject + restore HW memory
	virtual void ReleaseShaderObjects() { }
	virtual void RestoreShaderObjects(CreateInterfaceFn shaderFactory, int nChangeFlags = 0) { }

	// Used to prevent meshes from drawing.
	virtual bool IsInStubMode() { return false; }
	virtual bool InFlashlightMode() const { return false; }

	// For the shader API to shove the current version of aniso level into the
	// "definitive" place (g_config) when the shader API decides to change it.
	// Eventually, we should have a better system of who owns the definitive
	// versions of config vars.
	virtual void NoteAnisotropicLevel(int currentLevel) { }

	// NOTE: Stuff after this is added after shipping HL2.

	// Are we rendering through the editor?
	virtual bool InEditorMode() const { return false; }

	// Gets the bound morph's vertex format; returns 0 if no morph is bound
	virtual MorphFormat_t GetBoundMorphFormat() { return 0; }

	virtual ITexture *GetRenderTargetEx(int nRenderTargetID) { return NULL; }

	// Tells the material system to draw a buffer clearing quad
	virtual void DrawClearBufferQuad(unsigned char r, unsigned char g, unsigned char b, unsigned char a, bool bClearColor, bool bClearAlpha, bool bClearDepth) { }

#if defined( _X360 )
	virtual void ReadBackBuffer(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pData, ImageFormat dstFormat, int nDstStride) { }
#endif

	// Calls from meshes to material system to handle queing/threading
	virtual bool OnDrawMesh(IMesh *pMesh, int firstIndex, int numIndices) { return false; }
	virtual bool OnDrawMesh(IMesh *pMesh, CPrimList *pLists, int nLists) { return false; }
	virtual bool OnSetFlexMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes) { return false; }
	virtual bool OnSetColorMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes) { return false; }
	virtual bool OnSetPrimitiveType(IMesh *pMesh, MaterialPrimitiveType_t type) { return false; }
	virtual bool OnFlushBufferedPrimitives() { return false; }


	virtual void SyncMatrices() { }
	virtual void SyncMatrix(MaterialMatrixMode_t) { }

	virtual void BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id) { }
	virtual void GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id) { }

	virtual int MaxHWMorphBatchCount() const { return 0; }

	// Interface for mat system to tell shaderapi about color correction
	virtual void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
	{
		pInfo->m_bIsEnabled = false;
		pInfo->m_flDefaultWeight = 0.0f;
		pInfo->m_nLookupCount = 0;
		ZeroMemory(pInfo->m_pLookupWeights, sizeof(*pInfo->m_pLookupWeights));
	}
	// received an event while not in owning thread, handle this outside
	virtual void OnThreadEvent(uint32 threadEvent) { }

	virtual MaterialThreadMode_t	GetThreadMode() { return MATERIAL_SINGLE_THREADED;  }
	virtual bool					IsRenderThreadSafe() { return true; }

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) { }
};

static CShaderUtilSkeleton s_ShaderUtilSkeleton;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderUtilSkeleton, IShaderUtil,
	SHADER_UTIL_INTERFACE_VERSION, s_ShaderUtilSkeleton)

class CShaderAPITest : public CTier2SteamApp
{
	typedef CTier2SteamApp BaseClass;

public:
	// IAppSystemGroup

	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate.
	// Return false if there's any problems and the app will abort
	virtual bool Create();

	// Allow the application to do some work after AppSystems are connected but 
	// they are all Initialized.
	// Return false if there's any problems and the app will abort
	virtual bool PreInit();

	// Main loop implemented by the application
	virtual int Main();

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown();

	// Call an installed application destroy function, occurring after all modules
	// are unloaded
	virtual void Destroy();

private:

	bool CreateTestWindow(int width, int height);
	bool WaitForKeypress();
	bool WaitForQuit();

	bool RunTests();
	void RunDynamicBufferTest(bool bTestFallback);

	void CreateShaders();

private:
	HWND m_hWnd;

	IShaderDeviceMgr *m_pShaderDeviceMgr;

	IShaderAPI *m_pShaderAPI;
	IShaderDevice *m_pShaderDevice;
	IShaderShadow *m_pShaderShadow;

	VertexShaderHandle_t m_hVertexShader;
	PixelShaderHandle_t m_hPixelShader;
};

DEFINE_WINDOWED_STEAM_APPLICATION_OBJECT(CShaderAPITest);



bool CShaderAPITest::Create()
{
	const char *pShaderAPI = CommandLine()->ParmValue("-shaderapi");
	if (!pShaderAPI)
	{
		pShaderAPI = "shaderapidx11.dll";
	}

	AppModule_t appModule = LoadModule(pShaderAPI);
	if (appModule == APP_MODULE_INVALID) return false;

	m_pShaderDeviceMgr = (IShaderDeviceMgr *)AddSystem(appModule, SHADER_DEVICE_MGR_INTERFACE_VERSION);

	appModule = LoadModule(Sys_GetFactoryThis());
	AddSystem(appModule, SHADER_UTIL_INTERFACE_VERSION);

	return (m_pShaderDeviceMgr != NULL);
}

bool CShaderAPITest::PreInit()
{
	if (!BaseClass::PreInit())
		return false;

	if (!g_pFullFileSystem)
		return false;

// 	if (!SetupSearchPaths(NULL, false, true))
// 		return false;

	if (!CreateTestWindow(1024, 768))
		return false;

	return true;

}

static bool s_bWindowClosed = false;
static LRESULT CALLBACK ShaderAPITestWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		s_bWindowClosed = true;
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool CShaderAPITest::CreateTestWindow(int width, int height)
{

	WNDCLASS wndClass;
	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.style = CS_OWNDC | CS_DBLCLKS;
	wndClass.lpfnWndProc = ShaderAPITestWndProc;
	wndClass.hInstance = (HINSTANCE)GetAppInstance();
	wndClass.lpszClassName = "SHADERAPITEST";
	RegisterClass(&wndClass);

	DWORD style = WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;

	RECT windowRect;
	windowRect.top = 0;
	windowRect.left = 0;
	windowRect.right = width;
	windowRect.bottom = height;
	AdjustWindowRect(&windowRect, style, FALSE);

	m_hWnd = CreateWindow(wndClass.lpszClassName, "ShaderAPI Test", style, 0, 0,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, (HINSTANCE)GetAppInstance(), NULL);

	if (!m_hWnd)
		return false;

	int screenCenterX, screenCenterY;

	screenCenterX = Max((GetSystemMetrics(SM_CXSCREEN) - width) / 2, 0);
	screenCenterY = Max((GetSystemMetrics(SM_CYSCREEN) - height) / 2, 0);

	SetWindowPos(m_hWnd, NULL, screenCenterX, screenCenterY, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_DRAWFRAME);

	return true;
}

bool CShaderAPITest::WaitForKeypress()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_KEYDOWN)
			return true;
	}
	return false;
}

bool CShaderAPITest::WaitForQuit()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (s_bWindowClosed)
			return true;
	}
	return false;
}

int CShaderAPITest::Main()
{
	ShaderDeviceInfo_t mode;
	mode.m_DisplayMode.m_nWidth = 1024;
	mode.m_DisplayMode.m_nHeight = 768;
	mode.m_DisplayMode.m_Format = IMAGE_FORMAT_BGRA8888;
	mode.m_DisplayMode.m_nRefreshRateNumerator = 60;
	mode.m_DisplayMode.m_nRefreshRateDenominator = 1;
	mode.m_bWindowed = true;
	mode.m_nBackBufferCount = 1;

	CreateInterfaceFn factory = m_pShaderDeviceMgr->SetMode(m_hWnd, 0, mode);
	if (!factory) return false;

	m_pShaderAPI = (IShaderAPI *)factory(SHADERAPI_INTERFACE_VERSION, NULL);
	m_pShaderDevice = (IShaderDevice *)factory(SHADER_DEVICE_INTERFACE_VERSION, NULL);
	m_pShaderShadow = (IShaderShadow *)factory(SHADERSHADOW_INTERFACE_VERSION, NULL);

	if (m_pShaderAPI == NULL || m_pShaderDevice == NULL || m_pShaderShadow == NULL)
	{
		return 1;
	}

	if (!RunTests())
		return 1;

	return 0;
}

void CShaderAPITest::PostShutdown()
{
	BaseClass::PostShutdown();
}

void CShaderAPITest::Destroy()
{
	return;
}

#define NextTest(title) if (!WaitForKeypress()) \
	{  \
		Warning("Keypress check failed\n"); \
		return false; \
	} \
	SetWindowText(m_hWnd, title);

bool CShaderAPITest::RunTests()
{
	SetWindowText(m_hWnd, "Test 1: Buffer clear");

	m_pShaderAPI->ClearColor3ub(RandomInt(0, 255), RandomInt(0, 255), RandomInt(0, 255));
	m_pShaderAPI->ClearBuffers(true, false, false, -1, -1);
	m_pShaderDevice->Present();

	NextTest("Test 2: Dynamic Mesh - No fallback");

	int w, h;
	m_pShaderDevice->GetWindowSize(w, h);

	ShaderViewport_t viewport;
	viewport.Init(0, 0, w, h);
	m_pShaderAPI->SetViewports(1, &viewport);

	RunDynamicBufferTest(false);

	NextTest("Test 3: Dynamic Mesh - Fallback");

	RunDynamicBufferTest(true);

	NextTest("Tests Done! Close window to exit.");

	WaitForQuit();

	return true;
}

void CShaderAPITest::RunDynamicBufferTest(bool bTestFallback)
{
	VertexFormat_t fmt = VERTEX_POSITION | VERTEX_COLOR;

	if (!bTestFallback)
		fmt |= VERTEX_NORMAL;

	IVertexBuffer* pVertexBuffer = m_pShaderDevice->CreateVertexBuffer(
		SHADER_BUFFER_TYPE_DYNAMIC, fmt, 256, "");
	IIndexBuffer* pIndexBuffer = m_pShaderDevice->CreateIndexBuffer(
		SHADER_BUFFER_TYPE_DYNAMIC, MATERIAL_INDEX_FORMAT_16BIT, 30, "");

	CreateShaders();

	m_pShaderAPI->ClearBuffers(true, false, false, -1, -1);

	const int nNumRects = 8;
	float flRectWidth = 2.0f / nNumRects;
	for (int i = 0; i < nNumRects; ++i)
	{
		float flXOffset = i * flRectWidth;

		CVertexBuilder vertexBuilder(pVertexBuffer, fmt);
		vertexBuilder.Lock(4);

		vertexBuilder.Position3f(-1.0f + flXOffset, -1.0f, 0.5f);
		if (!bTestFallback) 
			vertexBuilder.Normal3f(0.0f, 0.0f, 1.0f);
		vertexBuilder.Color4ub(255, 0, 0, 255);
		vertexBuilder.AdvanceVertex();

		vertexBuilder.Position3f(-1.0f + flXOffset + flRectWidth, -1.0f, 0.5f);
		if (!bTestFallback) 
			vertexBuilder.Normal3f(0.0f, 1.0f, 0.0f);
		vertexBuilder.Color4ub(0, 255, 0, 255);
		vertexBuilder.AdvanceVertex();

		vertexBuilder.Position3f(-1.0f + flXOffset + flRectWidth, 1.0f, 0.5f);
		if (!bTestFallback) 
			vertexBuilder.Normal3f(1.0f, 0.0f, 0.0f);
		vertexBuilder.Color4ub(0, 0, 255, 255);
		vertexBuilder.AdvanceVertex();

		vertexBuilder.Position3f(-1.0f + flXOffset, 1.0f, 0.5f);
		if (!bTestFallback) 
			vertexBuilder.Normal3f(1.0f, 1.0f, 1.0f);
		vertexBuilder.Color4ub(0, 0, 0, 255);
		vertexBuilder.AdvanceVertex();

		vertexBuilder.Unlock();

		CIndexBuilder indexBuilder(pIndexBuffer, MATERIAL_INDEX_FORMAT_16BIT);
		indexBuilder.Lock(6, vertexBuilder.GetFirstVertex());

		indexBuilder.FastIndex(0);
		indexBuilder.FastIndex(2);
		indexBuilder.FastIndex(1);
		indexBuilder.FastIndex(0);
		indexBuilder.FastIndex(3);
		indexBuilder.FastIndex(2);
		indexBuilder.SpewData();

		indexBuilder.Unlock();

		m_pShaderAPI->BindVertexBuffer(0, pVertexBuffer, vertexBuilder.Offset(), vertexBuilder.GetFirstVertex(), vertexBuilder.TotalVertexCount(), fmt);
		m_pShaderAPI->BindIndexBuffer(pIndexBuffer, indexBuilder.Offset());
		m_pShaderAPI->Draw(MATERIAL_TRIANGLES, indexBuilder.GetFirstIndex(), indexBuilder.TotalIndexCount());
	}

	m_pShaderDevice->Present();

	m_pShaderDevice->DestroyVertexShader(m_hVertexShader);
	m_pShaderDevice->DestroyPixelShader(m_hPixelShader);

	m_hVertexShader = VERTEX_SHADER_HANDLE_INVALID;
	m_hPixelShader = PIXEL_SHADER_HANDLE_INVALID;

	m_pShaderDevice->DestroyVertexBuffer(pVertexBuffer);
	m_pShaderDevice->DestroyIndexBuffer(pIndexBuffer);
}

static const char s_pDebugVertexShader[] =
"struct VS_INPUT {"
"	float3 vPos : POSITION;"
"   float4 vColor : COLOR;"
"   float3 vNormal : NORMAL;"
"};"
""
"struct VS_OUTPUT {"
"	float4 projPos : SV_POSITION;"
"   float4 vColor : COLOR;"
"   float3 vNormal : NORMAL;"
"};"
""
"VS_OUTPUT main( const VS_INPUT v ) {"
"	VS_OUTPUT o = (VS_OUTPUT)0;"
"	o.projPos = float4(v.vPos, 1.0);"
"	o.vColor = v.vColor;"
"	o.vNormal = v.vNormal;"
"	return o;"
"}"
"";

static const char s_pDebugPixelShader[] =
"struct PS_INPUT"
"{"
"	float4 projPos : SV_POSITION;"
"	float4 vColor : COLOR;"
"   float3 vNormal : NORMAL;"
"};"
""
"float4 main( const PS_INPUT i ) : SV_TARGET"
"{"
"	return i.vColor + float4(i.vNormal, 0.0);"
"}"
"";


void CShaderAPITest::CreateShaders()
{
	m_hVertexShader = m_pShaderDevice->CreateVertexShader(s_pDebugVertexShader, sizeof(s_pDebugVertexShader), "vs_5_0");
	Assert(m_hVertexShader != VERTEX_SHADER_HANDLE_INVALID);

	m_hPixelShader = m_pShaderDevice->CreatePixelShader(s_pDebugPixelShader, sizeof(s_pDebugPixelShader), "ps_5_0");
	Assert(m_hPixelShader != PIXEL_SHADER_HANDLE_INVALID);

	m_pShaderAPI->BindVertexShader(m_hVertexShader);
	m_pShaderAPI->BindGeometryShader(GEOMETRY_SHADER_HANDLE_INVALID);
	m_pShaderAPI->BindPixelShader(m_hPixelShader);

}