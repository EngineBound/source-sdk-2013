#include "shaderapidx11.h"
#include "shaderapidx11_global.h"
#include "shaderdevicedx11.h"
#include "shaderdevicemgrdx11.h"
#include "ishaderutil.h"
#include "hardwareconfigdx11.h"

#include "mathlib/mathlib.h"


static CShaderAPIDX11 s_ShaderAPIDX11;
CShaderAPIDX11 *g_pShaderAPIDX11 = &s_ShaderAPIDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IShaderAPI,
	SHADERAPI_INTERFACE_VERSION, s_ShaderAPIDX11)

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IDebugTextureInfo,
	DEBUG_TEXTURE_INFO_VERSION, s_ShaderAPIDX11)

extern IShaderUtil *g_pShaderUtil;

// This file requires shader states to function, write those first :))

CShaderAPIDX11::CShaderAPIDX11() : m_DynamicMesh(true), m_DynamicState(), m_ShaderState(), m_FlexMesh(true)
{

}
	
// Set nCount viewports to those in pViewports
void CShaderAPIDX11::SetViewports(int nCount, const ShaderViewport_t* pViewports) 
{
	m_DynamicState.m_nNumViewports = min(nCount, MAX_DX11_VIEWPORTS);

	for (int i = 0; i < m_DynamicState.m_nNumViewports; ++i)
	{
		Assert(pViewports[i].m_nVersion == SHADER_VIEWPORT_VERSION);

		m_DynamicState.m_pViewports[i].TopLeftX = pViewports[i].m_nTopLeftX;
		m_DynamicState.m_pViewports[i].TopLeftY = pViewports[i].m_nTopLeftY;
		m_DynamicState.m_pViewports[i].Width = pViewports[i].m_nWidth;
		m_DynamicState.m_pViewports[i].Height = pViewports[i].m_nHeight;
		m_DynamicState.m_pViewports[i].MaxDepth = pViewports[i].m_flMaxZ;
		m_DynamicState.m_pViewports[i].MinDepth = pViewports[i].m_flMinZ;
	}

	g_pD3DDeviceContext->RSSetViewports(m_DynamicState.m_nNumViewports, m_DynamicState.m_pViewports);
}

// Get viewports up to nMax, stored in pViewports array and return num stored
int CShaderAPIDX11::GetViewports(ShaderViewport_t* pViewports, int nMax) const
{
	int nCount = min(nMax, m_DynamicState.m_nNumViewports);

	if (pViewports)
	{
		for (int i = 0; i < nCount; ++i)
		{
			const D3D11_VIEWPORT &viewport = m_DynamicState.m_pViewports[i];
			pViewports[i].m_nVersion = SHADER_VIEWPORT_VERSION;
			pViewports[i].m_nTopLeftX = viewport.TopLeftX;
			pViewports[i].m_nTopLeftY = viewport.TopLeftY;
			pViewports[i].m_nWidth = viewport.Width;
			pViewports[i].m_nHeight = viewport.Height;
			pViewports[i].m_flMaxZ = viewport.MaxDepth;
			pViewports[i].m_flMinZ = viewport.MinDepth;
		}
	}

	return nCount;
}

// Return system time
double CShaderAPIDX11::CurrentTime() const
{
	return Sys_FloatTime();
}

// Get dimensions of lightmap, REQUIRES SHADERUTIL TO BE INITIALIZED
void CShaderAPIDX11::GetLightmapDimensions(int *w, int *h)
{
	g_pShaderUtil->GetLightmapDimensions(w, h);
}

// Get fog mode
MaterialFogMode_t CShaderAPIDX11::GetSceneFogMode()
{
	return MATERIAL_FOG_NONE;
}

// Get fog color of the scene
void CShaderAPIDX11::GetSceneFogColor(unsigned char *rgb)
{
	return;
}

// stuff related to matrix stacks

// Get current matrix 'mode', which matrix is being updated
void CShaderAPIDX11::MatrixMode(MaterialMatrixMode_t matrixMode)
{
	m_MatrixMode = matrixMode;
	m_pCurMatrix = &m_ShaderState.m_pMatrixStacks[matrixMode].Top();
}

// Evidently, push and pop in this context don't mean what I thought they did

// Push a matrix, meaning add an item and call constructor
void CShaderAPIDX11::PushMatrix()
{
	CUtlStack<DirectX::XMMATRIX> &targetStack = m_ShaderState.m_pMatrixStacks[m_MatrixMode];
	targetStack.Push();
	m_pCurMatrix = &targetStack.Top();
}

// Pop a matrix, meaning remove an item
void CShaderAPIDX11::PopMatrix()
{
	CUtlStack<DirectX::XMMATRIX> &targetStack = m_ShaderState.m_pMatrixStacks[m_MatrixMode];
	targetStack.Pop();
	m_pCurMatrix = &targetStack.Top();
}

// Load m into current matrix, fulfilling the other end of push
void CShaderAPIDX11::LoadMatrix(float *m)
{
	DirectX::XMFLOAT4X4 newMat(m);
	*m_pCurMatrix = DirectX::XMLoadFloat4x4(&newMat);
}

// curMat = curMat x m
void CShaderAPIDX11::MultMatrix(float *m)
{
	DirectX::XMFLOAT4X4 newMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(*m_pCurMatrix, DirectX::XMLoadFloat4x4(&newMat));
}

// curMat = m x curMat
void CShaderAPIDX11::MultMatrixLocal(float *m)
{
	DirectX::XMFLOAT4X4 newMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&newMat), *m_pCurMatrix);
}

// dst = matrix in matrixMode slot, e.g. technically fulfilling other end of pop
void CShaderAPIDX11::GetMatrix(MaterialMatrixMode_t matrixMode, float *dst)
{
	DirectX::XMFLOAT4X4 tmpMat4x4;
	DirectX::XMStoreFloat4x4(&tmpMat4x4, m_ShaderState.m_pMatrixStacks[matrixMode].Top());
	
	memcpy(dst, &tmpMat4x4, sizeof(DirectX::XMFLOAT4X4));
}

// Set curMat to identity matrix
void CShaderAPIDX11::LoadIdentity(void)
{
	DirectX::XMMATRIX tmpMatrix = DirectX::XMMatrixIdentity();
	*m_pCurMatrix = tmpMatrix;
}

// Set curMat to camera2world matrix (Probably)
void CShaderAPIDX11::LoadCameraToWorld(void)
{
	// Cam2World = view inverse without translation
	DirectX::XMMATRIX invView;
	invView = DirectX::XMMatrixInverse(NULL, m_ShaderState.m_pMatrixStacks[MATERIAL_VIEW].Top());

	DirectX::XMFLOAT4X4 invView4x4;
	DirectX::XMStoreFloat4x4(&invView4x4, invView);

	invView4x4.m[3][0] = invView4x4.m[3][1] = invView4x4.m[3][2] = 0.0f;
	invView = DirectX::XMLoadFloat4x4(&invView4x4);

	*m_pCurMatrix = invView;
}

// Multiply current matrix with ortho matrix given view 'cube' (Probably)
void CShaderAPIDX11::Ortho(double left, double right, double bottom, double top, double zNear, double zFar)
{
	DirectX::XMMATRIX ortho = DirectX::XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(ortho, *m_pCurMatrix);
}

// Multiply current matrix with perspective matrix given clip planes, aspect ratio and x angle
void CShaderAPIDX11::PerspectiveX(double fovx, double aspect, double zNear, double zFar)
{
	// Get dimensions of near 'face'
	float width = 2 * tan( (fovx / 2) * (M_PI / 180.0) ) * zNear;
	float height = width / aspect;

	DirectX::XMMATRIX persp = DirectX::XMMatrixPerspectiveRH(width, height, zNear, zFar);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(persp, *m_pCurMatrix);
}

// Multiply current matrix with matrix given a subrect of the screen (Probably)
void CShaderAPIDX11::PickMatrix(int x, int y, int width, int height)
{
	// I'll do this later

	return;
}

// Rotate the current matrix by angle around axis x,y,z
void CShaderAPIDX11::Rotate(float angle, float x, float y, float z)
{
	DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(axis, angle * (M_PI  / 180.0f)), *m_pCurMatrix);
}

// Translate current matrix by x,y,z in absolute coordinates (Probably)
void CShaderAPIDX11::Translate(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(x, y, z), *m_pCurMatrix);
}

// Scale current matrix by x,y,z in absolute axes (Probably)
void CShaderAPIDX11::Scale(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, z), *m_pCurMatrix);
}

// Scale current matrix by x,y,1 in absolute axes (probably)
void CShaderAPIDX11::ScaleXY(float x, float y)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, 1.f), *m_pCurMatrix);
}

// Sets the color to modulate by
void CShaderAPIDX11::Color3f(float r, float g, float b)
{
	return;
}

void CShaderAPIDX11::Color3fv(float const* pColor)
{
	return;
}

void CShaderAPIDX11::Color4f(float r, float g, float b, float a)
{
	return;
}

void CShaderAPIDX11::Color4fv(float const* pColor)
{
	return;
}


void CShaderAPIDX11::Color3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

void CShaderAPIDX11::Color3ubv(unsigned char const* pColor)
{
	return;
}

void CShaderAPIDX11::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return;
}

void CShaderAPIDX11::Color4ubv(unsigned char const* pColor)
{
	return;
}


// Sets the constant register for vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderConstant(int var, float const* pVec, int numConst/* = 1*/, bool bForce/* = false*/)
{
	return;
}

void CShaderAPIDX11::SetPixelShaderConstant(int var, float const* pVec, int numConst/* = 1*/, bool bForce/* = false*/)
{
	return;
}


// Sets the default *dynamic* state
void CShaderAPIDX11::SetDefaultState()
{
	return;
}


// Get the current camera position in world space.
void CShaderAPIDX11::GetWorldSpaceCameraPosition(float* pPos) const
{
	// Stored in view matrix
	DirectX::XMFLOAT4X4 matView;
	DirectX::XMStoreFloat4x4(&matView, m_ShaderState.m_pMatrixStacks[MATERIAL_VIEW].Top());

	memcpy(pPos, &matView, sizeof(DirectX::XMFLOAT4X4));
}

// Number of bones for current rendered item? (maybe just everything but i doubt it)
int CShaderAPIDX11::GetCurrentNumBones(void) const
{
	return 0;
}

// Doesn't do anything I think
int CShaderAPIDX11::GetCurrentLightCombo(void) const
{
	return 0;
}

// Get fog type of scene
MaterialFogMode_t CShaderAPIDX11::GetCurrentFogType(void) const
{
	return MATERIAL_FOG_NONE;
}


// fixme: move this to shadow state

// Deprecated in dx11 I believe
void CShaderAPIDX11::SetTextureTransformDimension(TextureStage_t textureStage, int dimension, bool projected)
{
	return;
}

// Deprecated in dx11 I believe
void CShaderAPIDX11::DisableTextureTransform(TextureStage_t textureStage)
{
	return;
}

// Deprecated in dx11 I believe
void CShaderAPIDX11::SetBumpEnvMatrix(TextureStage_t textureStage, float m00, float m01, float m10, float m11)
{
	return;
}


// Sets the vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderIndex(int vshIndex/* = -1*/)
{
	return;
}

void CShaderAPIDX11::SetPixelShaderIndex(int pshIndex)
{
	return;
}


// Get the dimensions of the back buffer.
void CShaderAPIDX11::GetBackBufferDimensions(int& width, int& height) const
{
	g_pShaderDeviceDX11->GetBackBufferDimensions(width, height);
}


// FIXME: The following 6 methods used to live in IShaderAPI
// and were moved for stdshader_dx8. Let's try to move them back!

// Get the lights
int CShaderAPIDX11::GetMaxLights(void) const
{
	return 0;
}

// Get light at lightNum
const LightDesc_t& CShaderAPIDX11::GetLight(int lightNum) const
{
	static LightDesc_t tmp;
	return tmp;
}

// Set fogparams erm... awkwerd...
void CShaderAPIDX11::SetPixelShaderFogParams(int reg)
{
	return;
}


// Render state for the ambient light cube
void CShaderAPIDX11::SetVertexShaderStateAmbientLightCube()
{
	return;
}

// Set register pshReg to ambient lightcube, or black if bForceToBlack
void CShaderAPIDX11::SetPixelShaderStateAmbientLightCube(int pshReg, bool bForceToBlack/* = false*/)
{
	return;
}

// Set pshReg to pshReg + 5 to light data. 4 lights max in default functionality.
void CShaderAPIDX11::CommitPixelShaderLighting(int pshReg)
{
	return;
}

// Use this to get the mesh builder that allows us to modify vertex data
CMeshBuilder* CShaderAPIDX11::GetVertexModifyBuilder()
{
	return NULL;
}

// Currently using flashlight? (Probably)
bool CShaderAPIDX11::InFlashlightMode() const
{
	return ShaderUtil()->InFlashlightMode();
}

// Get flashlight state and set worldToTexture to the flashlight's worldToTexture matrix
const FlashlightState_t &CShaderAPIDX11::GetFlashlightState(VMatrix &worldToTexture) const
{
	static FlashlightState_t tmp;
	return tmp;
}


bool CShaderAPIDX11::InEditorMode() const
{
	return ShaderUtil()->InEditorMode();
}


// Gets the bound morph's vertex format; returns 0 if no morph is bound
MorphFormat_t CShaderAPIDX11::GetBoundMorphFormat()
{
	return ShaderUtil()->GetBoundMorphFormat();
}


// Binds a standard texture at id to sampler
void CShaderAPIDX11::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	ShaderUtil()->BindStandardTexture(sampler, id);
}

// Gets rendertarget at nRenderTargetID
ITexture *CShaderAPIDX11::GetRenderTargetEx(int nRenderTargetID)
{
	return ShaderUtil()->GetRenderTargetEx(nRenderTargetID);
}

// Set tonemapping scale to scale
void CShaderAPIDX11::SetToneMappingScaleLinear(const Vector &scale)
{
	return;
}

// Return tonemapping scale
const Vector &CShaderAPIDX11::GetToneMappingScaleLinear(void) const
{
	static Vector tmp;
	return tmp;
}

// Get lightmap scale
float CShaderAPIDX11::GetLightMapScaleFactor(void) const
{
	return 1.f;
}

// Load matrix m into bone at boneIndex
void CShaderAPIDX11::LoadBoneMatrix(int boneIndex, const float *m)
{
	return;
}

// Create perspective matrix given offset, fov and aspect
void CShaderAPIDX11::PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right)
{
	// Get dimensions of near 'face'
	float widthOver2 = tan((fovx / 2) * (M_PI / 180.0)) * zNear;
	float heightOver2 = widthOver2 / aspect;

	float planeLeft = left * widthOver2 - widthOver2 * (1.f - left);
	float planeRight = right * widthOver2 - widthOver2 * (1.f - right);
	float planeBottom = bottom * heightOver2 - heightOver2 * (1.f - bottom);
	float planeTop = top * heightOver2 - heightOver2 * (1.f - top);

	DirectX::XMMATRIX persp = DirectX::XMMatrixPerspectiveOffCenterRH(planeLeft, planeRight, planeBottom, planeTop, zNear, zFar);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(persp, *m_pCurMatrix);
}

// Gets max dxlevel and actual dxlevel, puts em in you know where
void CShaderAPIDX11::GetDXLevelDefaults(uint &max_dxlevel, uint &recommended_dxlevel)
{
	max_dxlevel = g_pHardwareConfigDX11->GetInfo().m_nMaxDXSupportLevel;
	recommended_dxlevel = g_pHardwareConfigDX11->GetInfo().m_nDXSupportLevel;
}

// Get flashlight state, world2texture mat and flashlightdepthtexture
const FlashlightState_t &CShaderAPIDX11::GetFlashlightStateEx(VMatrix &worldToTexture, ITexture **pFlashlightDepthTexture) const
{
	static FlashlightState_t tmp;
	return tmp;
}

// Calculate luminance of lightcube
float CShaderAPIDX11::GetAmbientLightCubeLuminance()
{
	return 0.f;
}

// Get lightstate and put it in state
void CShaderAPIDX11::GetDX9LightState(LightState_t *state) const
{
	state->m_nNumLights = 0;
	state->m_bAmbientLight = false;
	state->m_bStaticLightTexel = false;
	state->m_bStaticLightVertex = false;
}

int CShaderAPIDX11::GetPixelFogCombo() //0 is either range fog, or no fog simulated with rigged range fog values. 1 is height fog
{
	return 0;
}

// Bind vertex texture to texture at slot id
void CShaderAPIDX11::BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id)
{
	ShaderUtil()->BindStandardVertexTexture(sampler, id);
}


// Is hardware morphing enabled?
bool CShaderAPIDX11::IsHWMorphingEnabled() const
{
	return false;
}


// Get dimensions of standard texture 'id'
void CShaderAPIDX11::GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id)
{
	ShaderUtil()->GetStandardTextureDimensions(pWidth, pHeight, id);
}

// Set vertex shader constant var to pVec (BOOLEAN)
void CShaderAPIDX11::SetBooleanVertexShaderConstant(int var, BOOL const* pVec, int numBools/* = 1*/, bool bForce/* = false*/)
{

}

// Set vertex shader constant var to pVec (INT)
void CShaderAPIDX11::SetIntegerVertexShaderConstant(int var, int const* pVec, int numIntVecs/* = 1*/, bool bForce/* = false*/)
{
	return;
}

// Set pixel shader constant var to pVec (BOOLEAN)
void CShaderAPIDX11::SetBooleanPixelShaderConstant(int var, BOOL const* pVec, int numBools/* = 1*/, bool bForce/* = false*/)
{
	return;
}

// Set pixel shader constant var to pVec (INT)
void CShaderAPIDX11::SetIntegerPixelShaderConstant(int var, int const* pVec, int numIntVecs/* = 1*/, bool bForce/* = false*/)
{
	return;
}

//Are we in a configuration that needs access to depth data through the alpha channel later?
bool CShaderAPIDX11::ShouldWriteDepthToDestAlpha(void) const
{
	return false;
}



// deformations
void CShaderAPIDX11::PushDeformation(DeformationBase_t const *Deformation)
{
	return;
}

void CShaderAPIDX11::PopDeformation()
{
	return;
}

int CShaderAPIDX11::GetNumActiveDeformations() const
{
	return 0;
}



// for shaders to set vertex shader constants. returns a packed state which can be used to set
// the dynamic combo. returns # of active deformations
int CShaderAPIDX11::GetPackedDeformationInformation(int nMaskOfUnderstoodDeformations,
	float *pConstantValuesOut,
	int nBufferSize,
	int nMaximumDeformations,
	int *pNumDefsOut) const
{
	*pNumDefsOut = 0;
	return 0;
}


// This lets the lower level system that certain vertex fields requested 
// in the shadow state aren't actually being read given particular state
// known only at dynamic state time. It's here only to silence warnings.
void CShaderAPIDX11::MarkUnusedVertexFields(unsigned int nFlags, int nTexCoordCount, bool *pUnusedTexCoords)
{
	return;
}


// run all the commands in buffer pCmdBuffer
void CShaderAPIDX11::ExecuteCommandBuffer(uint8 *pCmdBuffer)
{
	return;
}


// interface for mat system to tell shaderapi about standard texture handles
void CShaderAPIDX11::SetStandardTextureHandle(StandardTextureId_t nId, ShaderAPITextureHandle_t nHandle)
{
	return;
}


// Interface for mat system to tell shaderapi about color correction
void CShaderAPIDX11::GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
{
	pInfo->m_bIsEnabled = false;
	pInfo->m_nLookupCount = 0;
	pInfo->m_flDefaultWeight = 0.0f;
}

// Set pshReg to nearz and farz vals
void CShaderAPIDX11::SetPSNearAndFarZ(int pshReg)
{
	return;
}


// Buffer clearing
void CShaderAPIDX11::ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil, int renderTargetWidth, int renderTargetHeight)
{
	return;
}

// Set clearcolor to rgb val
void CShaderAPIDX11::ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

// Set clearcolor to rgba val
void CShaderAPIDX11::ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return;
}


// Methods related to binding shaders
void CShaderAPIDX11::BindVertexShader(VertexShaderHandle_t hVertexShader)
{
	ID3D11VertexShader *pVertexShader = g_pShaderDeviceDX11->GetVertexShader(hVertexShader);
	m_DynamicState.m_pCurVertexShader = pVertexShader;

	g_pD3DDeviceContext->VSSetShader(m_DynamicState.m_pCurVertexShader, NULL, 0);
}

void CShaderAPIDX11::BindGeometryShader(GeometryShaderHandle_t hGeometryShader)
{
	ID3D11GeometryShader *pGeometryShader = g_pShaderDeviceDX11->GetGeometryShader(hGeometryShader);
	m_DynamicState.m_pCurGeometryShader = pGeometryShader;

	g_pD3DDeviceContext->GSSetShader(m_DynamicState.m_pCurGeometryShader, NULL, 0);
}

void CShaderAPIDX11::BindPixelShader(PixelShaderHandle_t hPixelShader)
{
	ID3D11PixelShader *pPixelShader = g_pShaderDeviceDX11->GetPixelShader(hPixelShader);
	m_DynamicState.m_pCurPixelShader = pPixelShader;

	g_pD3DDeviceContext->PSSetShader(m_DynamicState.m_pCurPixelShader, NULL, 0);
}


// Methods related to state objects
void CShaderAPIDX11::SetRasterState(const ShaderRasterState_t& state)
{
	return;
}


//
// NOTE: These methods have not yet been ported to DX10
//

// Sets the mode...
bool CShaderAPIDX11::SetMode(void* hwnd, int nAdapter, const ShaderDeviceInfo_t &info)
{
	return g_pShaderDeviceMgrDX11->SetMode(hwnd, nAdapter, info) != NULL;
}

// Change video mode to that described in info
void CShaderAPIDX11::ChangeVideoMode(const ShaderDeviceInfo_t &info)
{
	return;
}


// Returns the snapshot id for the shader state
StateSnapshot_t	CShaderAPIDX11::TakeSnapshot()
{
	return StateSnapshot_t();
}

// Filter for downscaling??
void CShaderAPIDX11::TexMinFilter(ShaderTexFilterMode_t texFilterMode)
{
	return;
}

// Filter for upscaling??
void CShaderAPIDX11::TexMagFilter(ShaderTexFilterMode_t texFilterMode)
{
	return;
}

// How to deal with > range coordinates
void CShaderAPIDX11::TexWrap(ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode)
{
	return;
}

// Copy rendertarget 0 to textureHandle
void CShaderAPIDX11::CopyRenderTargetToTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Binds a particular material to render with
void CShaderAPIDX11::Bind(IMaterial* pMaterial)
{
	return;
}


// Flushes any primitives that are buffered
void CShaderAPIDX11::FlushBufferedPrimitives()
{
	return;
}


// Gets the dynamic mesh { } note that you've got to render the mesh
// before calling this function a second time. Clients should *not*
// call DestroyStaticMesh on the mesh returned by this call.
IMesh* CShaderAPIDX11::GetDynamicMesh(IMaterial* pMaterial, int nHWSkinBoneCount, bool bBuffered/* = true*/,
	IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	return GetDynamicMeshEx(pMaterial, 0, nHWSkinBoneCount, bBuffered, pVertexOverride, pIndexOverride);
}

IMesh* CShaderAPIDX11::GetDynamicMeshEx(IMaterial* pMaterial, VertexFormat_t vertexFormat, int nHWSkinBoneCount,
	bool bBuffered/* = true*/, IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	CBaseMeshDX11 *pMesh;

	pMesh = &m_DynamicMesh;

	if (pVertexOverride)
	{
		CBaseMeshDX11 *pVertexOverrideMesh = static_cast<CBaseMeshDX11 *>(pVertexOverride);
		pMesh->SetVertexFormat(pVertexOverrideMesh->GetVertexFormat());
	}
	else
	{
		VertexFormat_t fmt;

		if (vertexFormat != 0)
		{
			fmt = vertexFormat;
			int nVertexFormatBoneWeights = NumBoneWeights(vertexFormat);
			if (nHWSkinBoneCount < nVertexFormatBoneWeights)
			{
				nHWSkinBoneCount = nVertexFormatBoneWeights;
			}
		}
		else
			fmt = pMaterial->GetVertexFormat() & ~VERTEX_FORMAT_COMPRESSED;

		fmt &= ~VERTEX_BONE_WEIGHT_MASK;
		if (nHWSkinBoneCount > 0)
		{
			fmt |= VERTEX_BONEWEIGHT(2);
			fmt |= VERTEX_BONE_INDEX;
		}

		fmt |= VERTEX_POSITION | VERTEX_NORMAL | VERTEX_COLOR; // Remove later

		pMesh->SetVertexFormat(fmt);
	}

	if (pMesh == &m_DynamicMesh)
	{
		CBaseMeshDX11 *pVertexOverrideMesh = static_cast<CBaseMeshDX11 *>(pVertexOverride);
		if (pVertexOverrideMesh)
		{
			pMesh->SetVertexBuffer(pVertexOverrideMesh->GetVertexBuffer());
		}

		CBaseMeshDX11 *pIndexOverrideMesh = static_cast<CBaseMeshDX11 *>(pIndexOverride);
		if (pIndexOverrideMesh)
		{
			pMesh->SetIndexBuffer(pIndexOverrideMesh->GetIndexBuffer());
		}
	}

	return pMesh;
}

enum
{
	TRANSLUCENT = 0x1,
	ALPHATESTED = 0x2,
	VERTEX_AND_PIXEL_SHADERS = 0x4,
	DEPTHWRITE = 0x8,
};

// Methods to ask about particular state snapshots
// Does the state snapshot use translucency
bool CShaderAPIDX11::IsTranslucent(StateSnapshot_t id) const
{
	return (id & TRANSLUCENT) != 0;
}

// Does the state snapshot use alpha testing
bool CShaderAPIDX11::IsAlphaTested(StateSnapshot_t id) const
{
	return (id & ALPHATESTED) != 0;
}

// Does the state snapshot use vertex and pixel shaders
bool CShaderAPIDX11::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	return (id & VERTEX_AND_PIXEL_SHADERS) != 0;
}

// Does the state snapshot need to write to depth
bool CShaderAPIDX11::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	return (id & DEPTHWRITE) != 0;
}


// Gets the vertex format for a set of snapshot ids
VertexFormat_t CShaderAPIDX11::ComputeVertexFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	return NULL;
}


// What fields in the vertex do we actually use?
VertexFormat_t CShaderAPIDX11::ComputeVertexUsage(int numSnapshots, StateSnapshot_t* pIds) const
{
	return NULL;
}


// Begins a rendering pass
void CShaderAPIDX11::BeginPass(StateSnapshot_t snapshot)
{
	return;
}


// Renders a single pass of a material
void CShaderAPIDX11::RenderPass(int nPass, int nPassCount)
{
	return;
}


// Set the number of bone weights
void CShaderAPIDX11::SetNumBoneWeights(int numBones)
{
	return;
}


// Sets the lights
void CShaderAPIDX11::SetLight(int lightNum, const LightDesc_t& desc)
{
	return;
}


// Lighting origin for the current model
void CShaderAPIDX11::SetLightingOrigin(Vector vLightingOrigin)
{
	return;
}

// Set ambient light to r,g,b
void CShaderAPIDX11::SetAmbientLight(float r, float g, float b)
{
	return;
}

// Set ambient light cube to that described in cube
void CShaderAPIDX11::SetAmbientLightCube(Vector4D cube[6])
{
	return;
}


// The shade mode
void CShaderAPIDX11::ShadeMode(ShaderShadeMode_t mode)
{
	return;
}


// The cull mode
void CShaderAPIDX11::CullMode(MaterialCullMode_t cullMode)
{
	return;
}


// Force writes only when z matches. . . useful for stenciling things out
// by rendering the desired Z values ahead of time.
void CShaderAPIDX11::ForceDepthFuncEquals(bool bEnable)
{
	return;
}


// Forces Z buffering to be on or off
void CShaderAPIDX11::OverrideDepthEnable(bool bEnable, bool bDepthEnable)
{
	return;
}

// Does as it says on the tin
void CShaderAPIDX11::SetHeightClipZ(float z)
{
	return;
}

// How do things render in relation to heightclip z
void CShaderAPIDX11::SetHeightClipMode(enum MaterialHeightClipMode_t heightClipMode)
{
	return;
}

// Set clipping plane at index to plane specified in pPlane
void CShaderAPIDX11::SetClipPlane(int index, const float *pPlane)
{
	return;
}

// Enable clipping plane at index
void CShaderAPIDX11::EnableClipPlane(int index, bool bEnable)
{
	return;
}


// Put all the model matrices into vertex shader constants.
void CShaderAPIDX11::SetSkinningMatrices()
{
	return;
}


// Returns the nearest supported format
ImageFormat CShaderAPIDX11::GetNearestSupportedFormat(ImageFormat fmt, bool bFilteringRequired/* = true*/) const
{
	return fmt;
}

// Get nearest format usable for render targets to fmt
ImageFormat CShaderAPIDX11::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	return fmt;
}


// When AA is enabled, render targets are not AA and require a separate
// depth buffer.
bool CShaderAPIDX11::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	return false;
}


// Texture management methods
// For CreateTexture also see CreateTextures below
ShaderAPITextureHandle_t CShaderAPIDX11::CreateTexture(
	int width,
	int height,
	int depth,
	ImageFormat dstImageFormat,
	int numMipLevels,
	int numCopies,
	int flags,
	const char *pDebugName,
	const char *pTextureGroupName)
{
	return NULL;
}

// Delete texture at textureHandle
void CShaderAPIDX11::DeleteTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}

// Create a depth texture
ShaderAPITextureHandle_t CShaderAPIDX11::CreateDepthTexture(
	ImageFormat renderTargetFormat,
	int width,
	int height,
	const char *pDebugName,
	bool bTexture)
{
	return NULL;
}

// Is the texture at textureHandle a texture? Redundant????
bool CShaderAPIDX11::IsTexture(ShaderAPITextureHandle_t textureHandle)
{
	return true;
}

// Is this a texture from praised video game franchise Resident Evil?
bool CShaderAPIDX11::IsTextureResident(ShaderAPITextureHandle_t textureHandle)
{
	// I actually have no idea what this means but let's settle at no, it is not.
	
	return false;
}


// Indicates we're going to be modifying this texture
// TexImage2D, TexSubImage2D, TexWrap, TexMinFilter, and TexMagFilter
// all use the texture specified by this function.
void CShaderAPIDX11::ModifyTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}

// Erm, X360, who cares
void CShaderAPIDX11::TexImage2D(
	int level,
	int cubeFaceID,
	ImageFormat dstFormat,
	int zOffset,
	int width,
	int height,
	ImageFormat srcFormat,
	bool bSrcIsTiled,		// NOTE: for X360 only
	void *imageData)
{
	return;
}

// Erm, X360, who cares
void CShaderAPIDX11::TexSubImage2D(
	int level,
	int cubeFaceID,
	int xOffset,
	int yOffset,
	int zOffset,
	int width,
	int height,
	ImageFormat srcFormat,
	int srcStride,
	bool bSrcIsTiled,		// NOTE: for X360 only
	void *imageData)
{
	return;
}

// Get teximage from vtf texture at iVTFFrame
void CShaderAPIDX11::TexImageFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	return;
}


// An alternate (and faster) way of writing image data
// (locks the current Modify Texture). Use the pixel writer to write the data
// after Lock is called
// Doesn't work for compressed textures 
bool CShaderAPIDX11::TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
	int width, int height, CPixelWriter& writer)
{
	return false;
}

// Unlock the poor texture already
void CShaderAPIDX11::TexUnlock()
{
	return;
}


// These are bound to the texture
void CShaderAPIDX11::TexSetPriority(int priority)
{
	return;
}


// Sets the texture state
void CShaderAPIDX11::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTarget(ShaderAPITextureHandle_t colorTextureHandle/* = SHADER_RENDERTARGET_BACKBUFFER*/,
	ShaderAPITextureHandle_t depthTextureHandle/* = SHADER_RENDERTARGET_DEPTHBUFFER*/)
{
	return;
}


// stuff that isn't to be used from within a shader
void CShaderAPIDX11::ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth)
{
	return;
}

// Read pixels to data
void CShaderAPIDX11::ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat)
{
	return;
}

// Read pixels to data but rect moment
void CShaderAPIDX11::ReadPixels(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *data, ImageFormat dstFormat, int nDstStride)
{
	return;
}

// Flush hardware
void CShaderAPIDX11::FlushHardware()
{
	return;
}


// Use this to begin the frame
void CShaderAPIDX11::BeginFrame()
{
	return;
}

//  Use this to end the frame
void CShaderAPIDX11::EndFrame()
{
	return;
}


// Selection mode methods
int CShaderAPIDX11::SelectionMode(bool selectionMode)
{
	return 0;
}

void CShaderAPIDX11::SelectionBuffer(unsigned int* pBuffer, int size)
{
	return;
}

void CShaderAPIDX11::ClearSelectionNames()
{
	return;
}

void CShaderAPIDX11::LoadSelectionName(int name)
{
	return;
}

void CShaderAPIDX11::PushSelectionName(int name)
{
	return;
}

void CShaderAPIDX11::PopSelectionName()
{
	return;
}


// Force the hardware to finish whatever it's doing
void CShaderAPIDX11::ForceHardwareSync()
{
	return;
}


// Used to clear the transition table when we know it's become invalid.
void CShaderAPIDX11::ClearSnapshots()
{
	return;
}

// Where fog starts
void CShaderAPIDX11::FogStart(float fStart)
{
	return;
}

// Where fog ends
void CShaderAPIDX11::FogEnd(float fEnd)
{
	return;
}

// Z of fog
void CShaderAPIDX11::SetFogZ(float fogZ)
{
	return;
}

// Scene fog state.
void CShaderAPIDX11::SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

// Fog mode of scene
void CShaderAPIDX11::SceneFogMode(MaterialFogMode_t fogMode)
{
	return;
}


// Can we download textures?
bool CShaderAPIDX11::CanDownloadTextures() const
{
	return false;// g_pShaderDeviceDX11->IsActivated();
}


void CShaderAPIDX11::ResetRenderState(bool bFullReset/* = true*/)
{
	return;
}


// We use smaller dynamic VBs during level transitions, to free up memory
int CShaderAPIDX11::GetCurrentDynamicVBSize(void)
{
	return 0;
}

// Delete the darn things
void CShaderAPIDX11::DestroyVertexBuffers(bool bExitingLevel/* = false*/)
{
	return;
}


void CShaderAPIDX11::EvictManagedResources()
{
	return;
}


// Level of anisotropic filtering
void CShaderAPIDX11::SetAnisotropicLevel(int nAnisotropyLevel)
{
	return;
}


// For debugging and building recording files. This will stuff a token into the recording file,
// then someone doing a playback can watch for the token.
void CShaderAPIDX11::SyncToken(const char *pToken)
{
	return;
}


// Setup standard vertex shader constants (that don't change)
// This needs to be called anytime that overbright changes.
void CShaderAPIDX11::SetStandardVertexShaderConstants(float fOverbright)
{
	return;
}


//
// Occlusion query support
//

// Allocate and delete query objects.
ShaderAPIOcclusionQuery_t CShaderAPIDX11::CreateOcclusionQueryObject(void)
{
	return INVALID_SHADERAPI_OCCLUSION_QUERY_HANDLE;
}

void CShaderAPIDX11::DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t)
{
	return;
}


// Bracket drawing with begin and end so that we can get counts next frame.
void CShaderAPIDX11::BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	return;
}

void CShaderAPIDX11::EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	return;
}


// OcclusionQuery_GetNumPixelsRendered
//	Get the number of pixels rendered between begin and end on an earlier frame.
//	Calling this in the same frame is a huge perf hit!
// Returns iQueryResult:
//	iQueryResult >= 0					-	iQueryResult is the number of pixels rendered
//	OCCLUSION_QUERY_RESULT_PENDING		-	query results are not available yet
//	OCCLUSION_QUERY_RESULT_ERROR		-	query failed
// Use OCCLUSION_QUERY_FINISHED( iQueryResult ) to test if query finished.
int CShaderAPIDX11::OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t hQuery, bool bFlush/* = false*/)
{
	return 0;
}

// Set flashlight state given state and w2t matrix
void CShaderAPIDX11::SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture)
{
	return;
}

void CShaderAPIDX11::ClearVertexAndPixelShaderRefCounts()
{
	return;
}

void CShaderAPIDX11::PurgeUnusedVertexAndPixelShaders()
{
	return;
}


// Called when the dx support level has changed
void CShaderAPIDX11::DXSupportLevelChanged()
{
	return;
}


// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
// planes (which are specified in world space) to generate projection-space user clip planes
// Occasionally (for the particle system in hl2, for example), we want to override that
// behavior and explictly specify a View transform for user clip planes. The PROJECTION
// will be mutliplied against this instead of the normal VIEW matrix.
void CShaderAPIDX11::EnableUserClipTransformOverride(bool bEnable)
{
	return;
}

void CShaderAPIDX11::UserClipTransform(const VMatrix &worldToView)
{
	return;
}


// ----------------------------------------------------------------------------------
// Everything after this point added after HL2 shipped.
// ----------------------------------------------------------------------------------

// What fields in the morph do we actually use?
MorphFormat_t CShaderAPIDX11::ComputeMorphFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	return MorphFormat_t();
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTargetEx(int nRenderTargetID,
	ShaderAPITextureHandle_t colorTextureHandle/* = SHADER_RENDERTARGET_BACKBUFFER*/,
	ShaderAPITextureHandle_t depthTextureHandle/* = SHADER_RENDERTARGET_DEPTHBUFFER*/)
{
	return;
}

// Copy render target to pDstRect of textureHandle given id, and pSrcRect
void CShaderAPIDX11::CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t textureHandle, int nRenderTargetID, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}

// Copy textureHandle to pDstRect of rendertarget given id, and pSrcRect
void CShaderAPIDX11::CopyTextureToRenderTargetEx(int nRenderTargetID, ShaderAPITextureHandle_t textureHandle, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}


// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
void CShaderAPIDX11::HandleDeviceLost()
{
	return;
}

// Make framebuffer linear color space
void CShaderAPIDX11::EnableLinearColorSpaceFrameBuffer(bool bEnable)
{
	return;
}


// Lets the shader know about the full-screen texture so it can 
void CShaderAPIDX11::SetFullScreenTextureHandle(ShaderAPITextureHandle_t h)
{
	return;
}


// Rendering parameters control special drawing modes withing the material system, shader
// system, shaders, and engine. renderparm.h has their definitions.

// Set rendering parameter parm_number to value
void CShaderAPIDX11::SetFloatRenderingParameter(int parm_number, float value)
{
	return;
}

// Set rendering parameter parm_number to value
void CShaderAPIDX11::SetIntRenderingParameter(int parm_number, int value)
{
	return;
}

// Set rendering parameter parm_number to value
void CShaderAPIDX11::SetVectorRenderingParameter(int parm_number, Vector const &value)
{
	return;
}

// Set rendering parameter parm_number to value
float CShaderAPIDX11::GetFloatRenderingParameter(int parm_number) const
{
	return 0.f;
}

// Get rendering parameter parm_number
int CShaderAPIDX11::GetIntRenderingParameter(int parm_number) const
{
	return 0;
}

// Get rendering parameter parm_number
Vector CShaderAPIDX11::GetVectorRenderingParameter(int parm_number) const
{
	return Vector(0, 0, 0);
}

// Set clip plane for fast clipping
void CShaderAPIDX11::SetFastClipPlane(const float *pPlane)
{
	return;
}

// Should fast clip be used?
void CShaderAPIDX11::EnableFastClip(bool bEnable)
{
	return;
}


// Returns the number of vertices + indices we can render using the dynamic mesh
// Passing true in the second parameter will return the max # of vertices + indices
// we can use before a flush is provoked and may return different values 
// if called multiple times in succession. 
// Passing false into the second parameter will return
// the maximum possible vertices + indices that can be rendered in a single batch
void CShaderAPIDX11::GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices)
{
	*pMaxVerts = 32768;
	*pMaxIndices = 32768;
	return;
}


// Returns the max number of vertices we can render for a given material
int CShaderAPIDX11::GetMaxVerticesToRender(IMaterial *pMaterial)
{
	return 32768;
}

// Get maximum indices to render for a specific mesh
int CShaderAPIDX11::GetMaxIndicesToRender()
{
	return 32768;
}


// stencil methods

// Should stencil be used?
void CShaderAPIDX11::SetStencilEnable(bool onoff)
{
	return;
}

// Set operation to do when stencil fails?
// Probably means when a pixel is not in the stencil
void CShaderAPIDX11::SetStencilFailOperation(StencilOperation_t op)
{
	return;
}

// Set operation to do when stencil fails on z stuff?
// Probably means when a pixel is fails compare function but is still in stencil
void CShaderAPIDX11::SetStencilZFailOperation(StencilOperation_t op)
{
	return;
}

// Set operation to do when stencil passes?
// Probably means when a pixel passes compare function
void CShaderAPIDX11::SetStencilPassOperation(StencilOperation_t op)
{
	return;
}

// Set function to compare stencil, will dictate fail, zfail and pass
void CShaderAPIDX11::SetStencilCompareFunction(StencilComparisonFunction_t cmpfn)
{
	return;
}

// Set stencil reference value? I need to read up on this
// May be z value
void CShaderAPIDX11::SetStencilReferenceValue(int ref)
{
	return;
}

// Set mask to be used before operations
void CShaderAPIDX11::SetStencilTestMask(uint32 msk)
{
	return;
}

// Set mask to be used when writing
void CShaderAPIDX11::SetStencilWriteMask(uint32 msk)
{
	return;
}

// Set stencil buffer in rect to value
void CShaderAPIDX11::ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value)
{
	return;
}


// disables all local lights
void CShaderAPIDX11::DisableAllLocalLights()
{
	return;
}

// Compare snapshot0 to snapshot1 and return some int that shows difference
int CShaderAPIDX11::CompareSnapshots(StateSnapshot_t snapshot0, StateSnapshot_t snapshot1)
{
	return 0;
}

// Get mesh for flexing
IMesh *CShaderAPIDX11::GetFlexMesh()
{
	return &m_FlexMesh;
}

// Set flashlight state to one given by state, w2t, and texture pointer
void CShaderAPIDX11::SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture)
{
	return;
}

// Check if nMSAAMode is supported
bool CShaderAPIDX11::SupportsMSAAMode(int nMSAAMode)
{
	return false;
}

// X360 specific functionality, just false outside of this
bool CShaderAPIDX11::OwnGPUResources(bool bEnable)
{
	return false;
}


//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
void CShaderAPIDX11::GetFogDistances(float *fStart, float *fEnd, float *fFogZ)
{
	return;
}


// Hooks for firing PIX events from outside the Material System...
void CShaderAPIDX11::BeginPIXEvent(unsigned long color, const char *szName)
{
	return;
}

void CShaderAPIDX11::EndPIXEvent()
{
	return;
}

void CShaderAPIDX11::SetPIXMarker(unsigned long color, const char *szName)
{
	return;
}


// Enables and disables for Alpha To Coverage

// Alpha to coverage is an MSAA technique that allows for (usually) cleaner results than alphatests with antialiasing
// Enable AlphaToCoverage
void CShaderAPIDX11::EnableAlphaToCoverage()
{
	return;
}

// Alpha to coverage is an MSAA technique that allows for (usually) cleaner results than alphatests with antialiasing
// Disable AlphaToCoverage
void CShaderAPIDX11::DisableAlphaToCoverage()
{
	return;
}


// Computes the vertex buffer pointers 
void CShaderAPIDX11::ComputeVertexDescription(unsigned char* pBuffer, VertexFormat_t vertexFormat, MeshDesc_t& desc) const
{
	return;
}

// Does the card support shadowmaps through depth textures?
bool CShaderAPIDX11::SupportsShadowDepthTextures(void)
{
	//return g_pHardwareConfig->GetInfo().something;
	return false;
}

// Not used right now, for mutex stuff
void CShaderAPIDX11::SetDisallowAccess(bool)
{
	return;
}

// Not used right now, for mutex stuff
void CShaderAPIDX11::EnableShaderShaderMutex(bool)
{
	return;
}

// Not used right now, for mutex stuff
void CShaderAPIDX11::ShaderLock()
{
	return;
}

// Not used right now, for mutex stuff
void CShaderAPIDX11::ShaderUnlock()
{
	return;
}

// Get shadowmap format
ImageFormat CShaderAPIDX11::GetShadowDepthTextureFormat(void)
{
	return IMAGE_FORMAT_UNKNOWN;
}

// Fetch4 is an ATI technology (or AMD)
bool CShaderAPIDX11::SupportsFetch4(void)
{
	return false;
}

// Set shadow biases to those specified
void CShaderAPIDX11::SetShadowDepthBiasFactors(float fShadowSlopeScaleDepthBias, float fShadowDepthBias)
{
	return;
}


// Bind pVertexBuffer to buffer at nStreamID nOffsetInBytes bytes down the stream or 
// nFirstVertex vertices down the stream if nOffsetInBytes < 0 of format fmt
void CShaderAPIDX11::BindVertexBuffer(int nStreamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions/* = 1*/)
{
	return;
}

// Bind pIndexBuffer to buffer nOffsetInBytes bytes down the stream
void CShaderAPIDX11::BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes)
{
	return;
}

void CShaderAPIDX11::UnbindVertexBuffer(ID3D11Buffer *pBuffer)
{
	// Check for buffer later

	for (int i = 0; i < MAX_DX11_STREAMS; ++i)
	{
		BindVertexBuffer(i, NULL, 0, 0, 0, VERTEX_POSITION, 0);
	}
}

void CShaderAPIDX11::UnbindIndexBuffer(ID3D11Buffer *pBuffer)
{
	// Check for buffer later

	BindIndexBuffer(NULL, 0);
}

// The classic moment:
// Draws a primitive of type primitiveType from nFirstIndex to nFirstIndex + nIndexCount - 1
void CShaderAPIDX11::Draw(MaterialPrimitiveType_t primitiveType, int nFirstIndex, int nIndexCount)
{
	return;
}

void CShaderAPIDX11::DrawMesh(IMesh *pMesh)
{
	m_pMesh = static_cast<CMeshDX11 *>(pMesh);
	if (!m_pMesh || !m_pMaterial)
	{
		return;
	}

	//TODO TODO
}

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
void CShaderAPIDX11::PerformFullScreenStencilOperation(void)
{
	return;
}

// Sets render rect (scissor test) and if it should be used
void CShaderAPIDX11::SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor)
{
	return;
}


// nVidia CSAA modes, different from SupportsMSAAMode()
bool CShaderAPIDX11::SupportsCSAAMode(int nNumSamples, int nQualityLevel)
{
	return false;
}


//Notifies the shaderapi to invalidate the current set of delayed constants because we just finished a draw pass. Either actual or not.
void CShaderAPIDX11::InvalidateDelayedShaderConstants(void)
{
	return;
}


// Gamma<->Linear conversions according to the video hardware we're running on
float CShaderAPIDX11::GammaToLinear_HardwareSpecific(float fGamma) const
{
// 	if (IsPC() || IsX360())
// 		return SrgbGammaToLinear(fGamma);

	//return pow(fGamma, 2.2f);
	return 0.0f;
}

float CShaderAPIDX11::LinearToGamma_HardwareSpecific(float fLinear) const
{
// 	if (IsPC() || IsX360())
// 		return SrgbLinearToGamma(fLinear);

	//return pow(fLinear, (1.0f / 2.2f));
	return 0.0f;
}


//Set's the linear->gamma conversion textures to use for this hardware for both srgb writes enabled and disabled(identity)
void CShaderAPIDX11::SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t hSRGBWriteEnabledTexture, ShaderAPITextureHandle_t hIdentityTexture)
{
	return;
}

// Format for null textures
ImageFormat CShaderAPIDX11::GetNullTextureFormat(void)
{
	return IMAGE_FORMAT_RGBA8888;
}

// Bind textureHandle to nSampler
void CShaderAPIDX11::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Enables hardware morphing
void CShaderAPIDX11::EnableHWMorphing(bool bEnable)
{
	return;
}


// Sets flexweights for rendering
void CShaderAPIDX11::SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights)
{
	return;
}

// Set max density of fog
void CShaderAPIDX11::FogMaxDensity(float flMaxDensity)
{
	return;
}


// Create a multi-frame texture (equivalent to calling "CreateTexture" multiple times, but more efficient)
void CShaderAPIDX11::CreateTextures(
	ShaderAPITextureHandle_t *pHandles,
	int count,
	int width,
	int height,
	int depth,
	ImageFormat dstImageFormat,
	int numMipLevels,
	int numCopies,
	int flags,
	const char *pDebugName,
	const char *pTextureGroupName)
{
	for (int k = 0; k < count; ++k)
		pHandles[k] = 0;
}

// Does nothing Lole
void CShaderAPIDX11::AcquireThreadOwnership()
{
	return;
}

// Equally does nothing lole
void CShaderAPIDX11::ReleaseThreadOwnership()
{
	return;
}


bool CShaderAPIDX11::SupportsNormalMapCompression() const
{
	return g_pHardwareConfigDX11->GetInfo().m_bSupportsNormalMapCompression;
}


// Only does anything on XBox360. This is useful to eliminate stalls
void CShaderAPIDX11::EnableBuffer2FramesAhead(bool bEnable)
{
	return;
}


void CShaderAPIDX11::SetDepthFeatheringPixelShaderConstant(int iConstant, float fDepthBlendScale)
{
	return;
}


// debug logging
// only implemented in some subclasses
void CShaderAPIDX11::PrintfVA(char *fmt, va_list vargs)
{
	return;
}

void CShaderAPIDX11::Printf(PRINTF_FORMAT_STRING const char *fmt, ...)
{
	return;
}

float CShaderAPIDX11::Knob(char *knobname, float *setvalue/* = NULL*/)
{
	return 0.f;
}

// Allows us to override the alpha write setting of a material
void CShaderAPIDX11::OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable)
{
	return;
}

void CShaderAPIDX11::OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable)
{
	return;
}


//extended clear buffers function with alpha independent from color
void CShaderAPIDX11::ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth)
{
	return;
}


// Allows copying a render target to another texture by specifying them both.
void CShaderAPIDX11::CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}


// Allows locking and unlocking of very specific surface types.
void CShaderAPIDX11::LockRect(void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead)
{
	return;
}

void CShaderAPIDX11::UnlockRect(ShaderAPITextureHandle_t texHandle, int mipmap)
{
	return;
}

// IDebugTextureInfo
// Use this to turn on the mode where it builds the debug texture list.
	// At the end of the next frame, GetDebugTextureList() will return a valid list of the textures.
void CShaderAPIDX11::EnableDebugTextureList(bool bEnable)
{
	return;
}

// If this is on, then it will return all textures that exist, not just the ones that were bound in the last frame.
// It is required to enable debug texture list to get this.
void CShaderAPIDX11::EnableGetAllTextures(bool bEnable)
{
	return;
}


// Use this to get the results of the texture list.
// Do NOT release the KeyValues after using them.
// There will be a bunch of subkeys, each with these values:
//    Name   - the texture's filename
//    Binds  - how many times the texture was bound
//    Format - ImageFormat of the texture
//    Width  - Width of the texture
//    Height - Height of the texture
// It is required to enable debug texture list to get this.
KeyValues* CShaderAPIDX11::GetDebugTextureList()
{
	return NULL;
}


// This returns how much memory was used.
int CShaderAPIDX11::GetTextureMemoryUsed(CShaderAPIDX11::TextureMemoryType eTextureMemory)
{
	return 0;
}


// Use this to determine if texture debug info was computed within last numFramesAllowed frames.
bool CShaderAPIDX11::IsDebugTextureListFresh(int numFramesAllowed/* = 1*/)
{
	return false;
}


// Enable debug texture rendering when texture binds should not count towards textures
// used during a frame. Returns the old state of debug texture rendering flag to use
// it for restoring the mode.
bool CShaderAPIDX11::SetDebugTextureRendering(bool bEnable)
{
	return false;
}

