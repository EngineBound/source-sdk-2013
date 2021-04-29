
#include "shaderapidx11.h"
#include "dx11global.h"

#include "shaderdevicedx11.h"
#include "ishaderdevicemgrdx11.h"
#include "ishaderutil.h"

static CShaderAPIDX11 s_ShaderAPIDX11;
CShaderAPIDX11 *g_pShaderAPIDX11 = &s_ShaderAPIDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IShaderAPIDX11,
	SHADERAPI_INTERFACE_VERSION, s_ShaderAPIDX11)

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IDebugTextureInfoDX11,
	DEBUG_TEXTURE_INFO_VERSION, s_ShaderAPIDX11)

IShaderAPIDX11 *g_pShaderAPI = g_pShaderAPIDX11;

extern CShaderDeviceDX11* g_pShaderDeviceDX11;

CShaderAPIDX11::CShaderAPIDX11()
{
	m_MatrixMode = MATERIAL_VIEW;
	m_DynamicState = DynamicStateDX11();
	m_ShaderState = ShaderStateDX11();

	m_pCurMatrix = &m_ShaderState.m_MatrixStacks[m_MatrixMode].Top();
}

// ------------------------------------------------------- //
//                       IShaderAPI                        //
// ------------------------------------------------------- //

// Viewport methods
void CShaderAPIDX11::SetViewports(int nCount, const ShaderViewport_t* pViewports)
{
	nCount = min(nCount, MAX_DX11_VIEWPORTS);

	for (int i = 0; i < nCount; ++i)
	{
		Assert(pViewports[i].m_nVersion == SHADER_VIEWPORT_VERSION);

		m_DynamicState.m_pViewports[i].Width = pViewports->m_nWidth;
		m_DynamicState.m_pViewports[i].Height = pViewports->m_nHeight;
		
		m_DynamicState.m_pViewports[i].MinDepth = pViewports->m_flMinZ;
		m_DynamicState.m_pViewports[i].MaxDepth = pViewports->m_flMaxZ;

		m_DynamicState.m_pViewports[i].TopLeftX = pViewports->m_nTopLeftX;
		m_DynamicState.m_pViewports[i].TopLeftY = pViewports->m_nTopLeftY;
	}

	m_DynamicState.m_nViewportCount = nCount;

	// May be better to do this only on each draw if needed
	g_pShaderDeviceDX11->GetDeviceContext()->RSSetViewports(nCount, m_DynamicState.m_pViewports);
}

int CShaderAPIDX11::GetViewports(ShaderViewport_t* pViewports, int nMax) const
{
	if (!pViewports)
		return 0;

	int nCount = min(m_DynamicState.m_nViewportCount, nMax);
	for (int i = 0; i < nCount; ++i)
	{
		pViewports[i].m_nVersion = SHADER_VIEWPORT_VERSION;

		pViewports[i].m_nWidth = m_DynamicState.m_pViewports[i].Width;
		pViewports[i].m_nHeight = m_DynamicState.m_pViewports[i].Height;

		pViewports->m_flMinZ = m_DynamicState.m_pViewports[i].MinDepth;
		pViewports->m_flMaxZ = m_DynamicState.m_pViewports[i].MaxDepth;

		pViewports->m_nTopLeftX = m_DynamicState.m_pViewports[i].TopLeftX;
		pViewports->m_nTopLeftY = m_DynamicState.m_pViewports[i].TopLeftY;
	}

	return nCount;
}


// returns the current time in seconds....
double CShaderAPIDX11::CurrentTime() const
{
	// Why is this a shaderapi function???
	return Sys_FloatTime();
}


// Gets the lightmap dimensions
void CShaderAPIDX11::GetLightmapDimensions(int *w, int *h)
{
	g_pShaderUtil->GetLightmapDimensions(w, h);
}


// Scene fog state.
// This is used by the shaders for picking the proper vertex shader for fogging based on dynamic state.
MaterialFogMode_t CShaderAPIDX11::GetSceneFogMode()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MATERIAL_FOG_NONE;
}

void CShaderAPIDX11::GetSceneFogColor(unsigned char *rgb)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// stuff related to matrix stacks
void CShaderAPIDX11::MatrixMode(MaterialMatrixMode_t matrixMode)
{
	m_MatrixMode = matrixMode;
	m_pCurMatrix = &m_ShaderState.m_MatrixStacks[m_MatrixMode].Top();
}

void CShaderAPIDX11::PushMatrix()
{
	CUtlStack<DirectX::XMMATRIX> &matStack = m_ShaderState.m_MatrixStacks[m_MatrixMode];
	int nNewInd = matStack.Push();
	matStack[nNewInd] = matStack[nNewInd - 1];

	m_pCurMatrix = &m_ShaderState.m_MatrixStacks[m_MatrixMode].Top();
}

void CShaderAPIDX11::PopMatrix()
{
	m_ShaderState.m_MatrixStacks[m_MatrixMode].Pop();

	m_pCurMatrix = &m_ShaderState.m_MatrixStacks[m_MatrixMode].Top();
}

void CShaderAPIDX11::LoadMatrix(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMLoadFloat4x4(&inMat);
}

void CShaderAPIDX11::MultMatrix(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(*m_pCurMatrix, DirectX::XMLoadFloat4x4(&inMat));
}

void CShaderAPIDX11::MultMatrixLocal(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&inMat), *m_pCurMatrix);
}

void CShaderAPIDX11::GetMatrix(MaterialMatrixMode_t matrixMode, float *dst)
{
	DirectX::XMFLOAT4X4 tmpMat;
	DirectX::XMStoreFloat4x4(&tmpMat, *m_pCurMatrix);
	V_memcpy(dst, &tmpMat, sizeof(DirectX::XMFLOAT4X4));
}

void CShaderAPIDX11::LoadIdentity(void)
{
	*m_pCurMatrix = DirectX::XMMatrixIdentity();
}

void CShaderAPIDX11::LoadCameraToWorld(void)
{
	// C2W = inv view - translation

	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(NULL, m_ShaderState.m_MatrixStacks[MATERIAL_VIEW].Top());

	DirectX::XMFLOAT4X4 tmpMat;
	DirectX::XMStoreFloat4x4(&tmpMat, invView);

	tmpMat.m[3][0] = tmpMat.m[3][1] = tmpMat.m[3][2] = 0.f;
	invView = DirectX::XMLoadFloat4x4(&tmpMat);

	*m_pCurMatrix = invView;

}

void CShaderAPIDX11::Ortho(double left, double right, double bottom, double top, double zNear, double zFar)
{
	DirectX::XMMATRIX orthoMat = DirectX::XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(orthoMat, *m_pCurMatrix);
}

void CShaderAPIDX11::PerspectiveX(double fovx, double aspect, double zNear, double zFar)
{
	float width = 2 * zNear * tan(fovx * M_PI / 360.0);
	float height = width / aspect;

	DirectX::XMMATRIX perspMat = DirectX::XMMatrixPerspectiveRH(width, height, zNear, zFar);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(perspMat, *m_pCurMatrix);
}

void CShaderAPIDX11::PickMatrix(int x, int y, int width, int height)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Rotate(float angle, float x, float y, float z)
{
	DirectX::XMVECTOR rotAxis;
	rotAxis = DirectX::XMVectorSet(x, y, z, 0);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(rotAxis, angle * M_PI / 180.0), *m_pCurMatrix);
}

void CShaderAPIDX11::Translate(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(x, y, z), *m_pCurMatrix);
}

void CShaderAPIDX11::Scale(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, z), *m_pCurMatrix);
}

void CShaderAPIDX11::ScaleXY(float x, float y)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, 1.f), *m_pCurMatrix);
}


// Sets the color to modulate by
void CShaderAPIDX11::Color3f(float r, float g, float b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color3fv(float const* pColor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color4f(float r, float g, float b, float a)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color4fv(float const* pColor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::Color3ub(unsigned char r, unsigned char g, unsigned char b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color3ubv(unsigned char const* pColor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Color4ubv(unsigned char const* pColor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets the constant register for vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderConstant(int var, float const* pVec, int numConst, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetPixelShaderConstant(int var, float const* pVec, int numConst, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets the default *dynamic* state
void CShaderAPIDX11::SetDefaultState()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Get the current camera position in world space.
void CShaderAPIDX11::GetWorldSpaceCameraPosition(float* pPos) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


int CShaderAPIDX11::GetCurrentNumBones(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

int CShaderAPIDX11::GetCurrentLightCombo(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


MaterialFogMode_t CShaderAPIDX11::GetCurrentFogType(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MATERIAL_FOG_NONE;
}


// fixme: move this to shadow state
void CShaderAPIDX11::SetTextureTransformDimension(TextureStage_t textureStage, int dimension, bool projected)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::DisableTextureTransform(TextureStage_t textureStage)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetBumpEnvMatrix(TextureStage_t textureStage, float m00, float m01, float m10, float m11)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets the vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderIndex(int vshIndex)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetPixelShaderIndex(int pshIndex)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Get the dimensions of the back buffer.
void CShaderAPIDX11::GetBackBufferDimensions(int& width, int& height) const
{
	g_pShaderDevice->GetBackBufferDimensions(width, height);
}


// FIXME: The following 6 methods used to live in IShaderAPI
// and were moved for stdshader_dx8. Let's try to move them back!

// Get the lights
int CShaderAPIDX11::GetMaxLights(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

static LightDesc_t s_tmpLightDesc;

const LightDesc_t& CShaderAPIDX11::GetLight(int lightNum) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return s_tmpLightDesc;
}


void CShaderAPIDX11::SetPixelShaderFogParams(int reg)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Render state for the ambient light cube
void CShaderAPIDX11::SetVertexShaderStateAmbientLightCube()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetPixelShaderStateAmbientLightCube(int pshReg, bool bForceToBlack)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::CommitPixelShaderLighting(int pshReg)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Use this to get the mesh builder that allows us to modify vertex data
CMeshBuilder* CShaderAPIDX11::GetVertexModifyBuilder()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

bool CShaderAPIDX11::InFlashlightMode() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

static FlashlightState_t s_tmpFlashlightState;

const FlashlightState_t &CShaderAPIDX11::GetFlashlightState(VMatrix &worldToTexture) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return s_tmpFlashlightState;
}

bool CShaderAPIDX11::InEditorMode() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Gets the bound morph's vertex format; returns 0 if no morph is bound
MorphFormat_t CShaderAPIDX11::GetBoundMorphFormat()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MORPH_NORMAL;
}


// Binds a standard texture
void CShaderAPIDX11::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


ITexture *CShaderAPIDX11::GetRenderTargetEx(int nRenderTargetID)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


void CShaderAPIDX11::SetToneMappingScaleLinear(const Vector &scale)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

static Vector s_tmpVec;
const Vector &CShaderAPIDX11::GetToneMappingScaleLinear(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return s_tmpVec;
}

float CShaderAPIDX11::GetLightMapScaleFactor(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.0f;
}


void CShaderAPIDX11::LoadBoneMatrix(int boneIndex, const float *m)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::GetDXLevelDefaults(uint &max_dxlevel, uint &recommended_dxlevel)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


const FlashlightState_t &CShaderAPIDX11::GetFlashlightStateEx(VMatrix &worldToTexture, ITexture **pFlashlightDepthTexture) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return s_tmpFlashlightState;
}


float CShaderAPIDX11::GetAmbientLightCubeLuminance()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.0f;
}


void CShaderAPIDX11::GetDX9LightState(LightState_t *state) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

int CShaderAPIDX11::GetPixelFogCombo()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}
 //0 is either range fog, or no fog simulated with rigged range fog values. 1 is height fog

void CShaderAPIDX11::BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Is hardware morphing enabled?
bool CShaderAPIDX11::IsHWMorphingEnabled() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


void CShaderAPIDX11::GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetBooleanVertexShaderConstant(int var, BOOL const* pVec, int numBools, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetIntegerVertexShaderConstant(int var, int const* pVec, int numIntVecs, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetBooleanPixelShaderConstant(int var, BOOL const* pVec, int numBools, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetIntegerPixelShaderConstant(int var, int const* pVec, int numIntVecs, bool bForce)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//Are we in a configuration that needs access to depth data through the alpha channel later?
bool CShaderAPIDX11::ShouldWriteDepthToDestAlpha(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}



// deformations
void CShaderAPIDX11::PushDeformation(DeformationBase_t const *Deformation)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::PopDeformation()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

int CShaderAPIDX11::GetNumActiveDeformations() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}



// for shaders to set vertex shader constants. returns a packed state which can be used to set
// the dynamic combo. returns # of active deformations
int CShaderAPIDX11::GetPackedDeformationInformation(int nMaskOfUnderstoodDeformations,
float *pConstantValuesOut,
int nBufferSize,
int nMaximumDeformations,
int *pNumDefsOut) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// This lets the lower level system that certain vertex fields requested 
// in the shadow state aren't actually being read given particular state
// known only at dynamic state time. It's here only to silence warnings.
void CShaderAPIDX11::MarkUnusedVertexFields(unsigned int nFlags, int nTexCoordCount, bool *pUnusedTexCoords)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}



void CShaderAPIDX11::ExecuteCommandBuffer(uint8 *pCmdBuffer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// interface for mat system to tell shaderapi about standard texture handles
void CShaderAPIDX11::SetStandardTextureHandle(StandardTextureId_t nId, ShaderAPITextureHandle_t nHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Interface for mat system to tell shaderapi about color correction
void CShaderAPIDX11::GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetPSNearAndFarZ(int pshReg)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetDepthFeatheringPixelShaderConstant(int iConstant, float fDepthBlendScale)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Buffer clearing
void CShaderAPIDX11::ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil, int renderTargetWidth, int renderTargetHeight)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Methods related to binding shaders
void CShaderAPIDX11::BindVertexShader(VertexShaderHandle_t hVertexShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::BindGeometryShader(GeometryShaderHandle_t hGeometryShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::BindPixelShader(PixelShaderHandle_t hPixelShader)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Methods related to state objects
void CShaderAPIDX11::SetRasterState(const ShaderRasterState_t& state)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//
// NOTE: These methods have not yet been ported to DX10
//

// Sets the mode...
bool CShaderAPIDX11::SetMode(void* hwnd, int nAdapter, const ShaderDeviceInfo_t &info)
{
	return g_pShaderDeviceMgr->SetMode(hwnd, nAdapter, info) != NULL;
}


void CShaderAPIDX11::ChangeVideoMode(const ShaderDeviceInfo_t &info)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the snapshot id for the shader state
StateSnapshot_t	 CShaderAPIDX11::TakeSnapshot()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


void CShaderAPIDX11::TexMinFilter(ShaderTexFilterMode_t texFilterMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::TexMagFilter(ShaderTexFilterMode_t texFilterMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::TexWrap(ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::CopyRenderTargetToTexture(ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Binds a particular material to render with
void CShaderAPIDX11::Bind(IMaterial* pMaterial)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Flushes any primitives that are buffered
void CShaderAPIDX11::FlushBufferedPrimitives()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Gets the dynamic mesh; note that you've got to render the mesh
// before calling this function a second time. Clients should *not*
// call DestroyStaticMesh on the mesh returned by this call.
IMesh* CShaderAPIDX11::GetDynamicMesh(IMaterial* pMaterial, int nHWSkinBoneCount, bool bBuffered,
	IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

IMesh* CShaderAPIDX11::GetDynamicMeshEx(IMaterial* pMaterial, VertexFormat_t vertexFormat, int nHWSkinBoneCount,
	bool bBuffered, IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


// Methods to ask about particular state snapshots
bool CShaderAPIDX11::IsTranslucent(StateSnapshot_t id) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CShaderAPIDX11::IsAlphaTested(StateSnapshot_t id) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CShaderAPIDX11::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CShaderAPIDX11::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Gets the vertex format for a set of snapshot ids
VertexFormat_t CShaderAPIDX11::ComputeVertexFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return VERTEX_FORMAT_UNKNOWN;
}


// What fields in the vertex do we actually use?
VertexFormat_t CShaderAPIDX11::ComputeVertexUsage(int numSnapshots, StateSnapshot_t* pIds) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return VERTEX_FORMAT_UNKNOWN;
}


// Begins a rendering pass
void CShaderAPIDX11::BeginPass(StateSnapshot_t snapshot)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Renders a single pass of a material
void CShaderAPIDX11::RenderPass(int nPass, int nPassCount)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Set the number of bone weights
void CShaderAPIDX11::SetNumBoneWeights(int numBones)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets the lights
void CShaderAPIDX11::SetLight(int lightNum, const LightDesc_t& desc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Lighting origin for the current model
void CShaderAPIDX11::SetLightingOrigin(Vector vLightingOrigin)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetAmbientLight(float r, float g, float b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetAmbientLightCube(Vector4D cube[6])
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// The shade mode
void CShaderAPIDX11::ShadeMode(ShaderShadeMode_t mode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// The cull mode
void CShaderAPIDX11::CullMode(MaterialCullMode_t cullMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Force writes only when z matches. . . useful for stenciling things out
// by rendering the desired Z values ahead of time.
void CShaderAPIDX11::ForceDepthFuncEquals(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Forces Z buffering to be on or off
void CShaderAPIDX11::OverrideDepthEnable(bool bEnable, bool bDepthEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetHeightClipZ(float z)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetHeightClipMode(enum MaterialHeightClipMode_t heightClipMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetClipPlane(int index, const float *pPlane)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EnableClipPlane(int index, bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Put all the model matrices into vertex shader constants.
void CShaderAPIDX11::SetSkinningMatrices()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the nearest supported format
ImageFormat CShaderAPIDX11::GetNearestSupportedFormat(ImageFormat fmt, bool bFilteringRequired) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return IMAGE_FORMAT_UNKNOWN;
}

ImageFormat CShaderAPIDX11::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return IMAGE_FORMAT_UNKNOWN;
}


// When AA is enabled, render targets are not AA and require a separate
// depth buffer.
bool CShaderAPIDX11::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderAPIDX11::DeleteTexture(ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


ShaderAPITextureHandle_t CShaderAPIDX11::CreateDepthTexture(
ImageFormat renderTargetFormat,
int width,
int height,
const char *pDebugName,
bool bTexture)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

bool CShaderAPIDX11::IsTexture(ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CShaderAPIDX11::IsTextureResident(ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Indicates we're going to be modifying this texture
// TexImage2D, TexSubImage2D, TexWrap, TexMinFilter, and TexMagFilter
// all use the texture specified by this function.
void CShaderAPIDX11::ModifyTexture(ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::TexImageFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// An alternate (and faster) way of writing image data
// (locks the current Modify Texture). Use the pixel writer to write the data
// after Lock is called
// Doesn't work for compressed textures 
bool CShaderAPIDX11::TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
	int width, int height, CPixelWriter& writer)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CShaderAPIDX11::TexUnlock()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// These are bound to the texture
void CShaderAPIDX11::TexSetPriority(int priority)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets the texture state
void CShaderAPIDX11::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTarget(ShaderAPITextureHandle_t colorTextureHandle,
	ShaderAPITextureHandle_t depthTextureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// stuff that isn't to be used from within a shader
void CShaderAPIDX11::ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ReadPixels(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *data, ImageFormat dstFormat, int nDstStride)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::FlushHardware()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Use this to begin and end the frame
void CShaderAPIDX11::BeginFrame()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EndFrame()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Selection mode methods
int  CShaderAPIDX11::SelectionMode(bool selectionMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

void CShaderAPIDX11::SelectionBuffer(unsigned int* pBuffer, int size)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ClearSelectionNames()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::LoadSelectionName(int name)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::PushSelectionName(int name)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::PopSelectionName()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Force the hardware to finish whatever it's doing
void CShaderAPIDX11::ForceHardwareSync()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Used to clear the transition table when we know it's become invalid.
void CShaderAPIDX11::ClearSnapshots()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::FogStart(float fStart)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::FogEnd(float fEnd)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetFogZ(float fogZ)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Scene fog state.
void CShaderAPIDX11::SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SceneFogMode(MaterialFogMode_t fogMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Can we download textures?
bool CShaderAPIDX11::CanDownloadTextures() const
{
	return g_pShaderDeviceDX11->IsActivated();
}


void CShaderAPIDX11::ResetRenderState(bool bFullReset)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// We use smaller dynamic VBs during level transitions, to free up memory
int  CShaderAPIDX11::GetCurrentDynamicVBSize(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

void CShaderAPIDX11::DestroyVertexBuffers(bool bExitingLevel)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::EvictManagedResources()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Level of anisotropic filtering
void CShaderAPIDX11::SetAnisotropicLevel(int nAnisotropyLevel)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// For debugging and building recording files. This will stuff a token into the recording file,
// then someone doing a playback can watch for the token.
void CShaderAPIDX11::SyncToken(const char *pToken)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Setup standard vertex shader constants (that don't change)
// This needs to be called anytime that overbright changes.
void CShaderAPIDX11::SetStandardVertexShaderConstants(float fOverbright)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//
// Occlusion query support
//

// Allocate and delete query objects.
ShaderAPIOcclusionQuery_t CShaderAPIDX11::CreateOcclusionQueryObject(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}

void CShaderAPIDX11::DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Bracket drawing with begin and end so that we can get counts next frame.
void CShaderAPIDX11::BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// OcclusionQuery_GetNumPixelsRendered
//	Get the number of pixels rendered between begin and end on an earlier frame.
//	Calling this in the same frame is a huge perf hit!
// Returns iQueryResult:
//	iQueryResult >= 0					-	iQueryResult is the number of pixels rendered
//	OCCLUSION_QUERY_RESULT_PENDING		-	query results are not available yet
//	OCCLUSION_QUERY_RESULT_ERROR		-	query failed
// Use OCCLUSION_QUERY_FINISHED( iQueryResult ) to test if query finished.
int CShaderAPIDX11::OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t hQuery, bool bFlush)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


void CShaderAPIDX11::SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::ClearVertexAndPixelShaderRefCounts()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::PurgeUnusedVertexAndPixelShaders()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Called when the dx support level has changed
void CShaderAPIDX11::DXSupportLevelChanged()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
// planes (which are specified in world space) to generate projection-space user clip planes
// Occasionally (for the particle system in hl2, for example), we want to override that
// behavior and explictly specify a View transform for user clip planes. The PROJECTION
// will be mutliplied against this instead of the normal VIEW matrix.
void CShaderAPIDX11::EnableUserClipTransformOverride(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::UserClipTransform(const VMatrix &worldToView)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// ----------------------------------------------------------------------------------
// Everything after this point added after HL2 shipped.
// ----------------------------------------------------------------------------------

// What fields in the morph do we actually use?
MorphFormat_t CShaderAPIDX11::ComputeMorphFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MORPH_NORMAL;
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTargetEx(int nRenderTargetID,
ShaderAPITextureHandle_t colorTextureHandle,
ShaderAPITextureHandle_t depthTextureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t textureHandle, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::CopyTextureToRenderTargetEx(int nRenderTargetID, ShaderAPITextureHandle_t textureHandle, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
void CShaderAPIDX11::HandleDeviceLost()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::EnableLinearColorSpaceFrameBuffer(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Lets the shader know about the full-screen texture so it can 
void CShaderAPIDX11::SetFullScreenTextureHandle(ShaderAPITextureHandle_t h)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Rendering parameters control special drawing modes withing the material system, shader
// system, shaders, and engine. renderparm.h has their definitions.
void CShaderAPIDX11::SetFloatRenderingParameter(int parm_number, float value)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetIntRenderingParameter(int parm_number, int value)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetVectorRenderingParameter(int parm_number, Vector const &value)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


float CShaderAPIDX11::GetFloatRenderingParameter(int parm_number) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.f;
}

int CShaderAPIDX11::GetIntRenderingParameter(int parm_number) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

Vector CShaderAPIDX11::GetVectorRenderingParameter(int parm_number) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return s_tmpVec;
}


void CShaderAPIDX11::SetFastClipPlane(const float *pPlane)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EnableFastClip(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the number of vertices + indices we can render using the dynamic mesh
// Passing true in the second parameter will return the max # of vertices + indices
// we can use before a flush is provoked and may return different values 
// if called multiple times in succession. 
// Passing false into the second parameter will return
// the maximum possible vertices + indices that can be rendered in a single batch
void CShaderAPIDX11::GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Returns the max number of vertices we can render for a given material
int CShaderAPIDX11::GetMaxVerticesToRender(IMaterial *pMaterial)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

int CShaderAPIDX11::GetMaxIndicesToRender()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// stencil methods
void CShaderAPIDX11::SetStencilEnable(bool onoff)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilFailOperation(StencilOperation_t op)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilZFailOperation(StencilOperation_t op)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilPassOperation(StencilOperation_t op)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilCompareFunction(StencilComparisonFunction_t cmpfn)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilReferenceValue(int ref)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilTestMask(uint32 msk)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetStencilWriteMask(uint32 msk)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// disables all local lights
void CShaderAPIDX11::DisableAllLocalLights()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

int CShaderAPIDX11::CompareSnapshots(StateSnapshot_t snapshot0, StateSnapshot_t snapshot1)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


IMesh *CShaderAPIDX11::GetFlexMesh()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


void CShaderAPIDX11::SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


bool CShaderAPIDX11::SupportsMSAAMode(int nMSAAMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


bool CShaderAPIDX11::OwnGPUResources(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
void CShaderAPIDX11::GetFogDistances(float *fStart, float *fEnd, float *fFogZ)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Hooks for firing PIX events from outside the Material System...
void CShaderAPIDX11::BeginPIXEvent(unsigned long color, const char *szName)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EndPIXEvent()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::SetPIXMarker(unsigned long color, const char *szName)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Enables and disables for Alpha To Coverage
void CShaderAPIDX11::EnableAlphaToCoverage()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::DisableAlphaToCoverage()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Computes the vertex buffer pointers 
void CShaderAPIDX11::ComputeVertexDescription(unsigned char* pBuffer, VertexFormat_t vertexFormat, MeshDesc_t& desc) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


bool CShaderAPIDX11::SupportsShadowDepthTextures(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


void CShaderAPIDX11::SetDisallowAccess(bool)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::EnableShaderShaderMutex(bool)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ShaderLock()
{
	// Nothing for now
}

void CShaderAPIDX11::ShaderUnlock()
{
	// Nothing for now
}


ImageFormat CShaderAPIDX11::GetShadowDepthTextureFormat(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return IMAGE_FORMAT_UNKNOWN;
}


bool CShaderAPIDX11::SupportsFetch4(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CShaderAPIDX11::SetShadowDepthBiasFactors(float fShadowSlopeScaleDepthBias, float fShadowDepthBias)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// ------------ New Vertex/Index Buffer interface ----------------------------
void CShaderAPIDX11::BindVertexBuffer(int nStreamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Draw(MaterialPrimitiveType_t primitiveType, int nFirstIndex, int nIndexCount)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::DrawMesh(IMesh *pMesh)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// ------------ End ----------------------------


// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
void CShaderAPIDX11::PerformFullScreenStencilOperation(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// nVidia CSAA modes, different from SupportsMSAAMode()
bool CShaderAPIDX11::SupportsCSAAMode(int nNumSamples, int nQualityLevel)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


//Notifies the shaderapi to invalidate the current set of delayed constants because we just finished a draw pass. Either actual or not.
void CShaderAPIDX11::InvalidateDelayedShaderConstants(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Gamma<->Linear conversions according to the video hardware we're running on
float CShaderAPIDX11::GammaToLinear_HardwareSpecific(float fGamma) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.f;
}

float CShaderAPIDX11::LinearToGamma_HardwareSpecific(float fLinear) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.f;
}


//Set's the linear->gamma conversion textures to use for this hardware for both srgb writes enabled and disabled(identity)
void CShaderAPIDX11::SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t hSRGBWriteEnabledTexture, ShaderAPITextureHandle_t hIdentityTexture)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


ImageFormat CShaderAPIDX11::GetNullTextureFormat(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return IMAGE_FORMAT_UNKNOWN;
}


void CShaderAPIDX11::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Enables hardware morphing
void CShaderAPIDX11::EnableHWMorphing(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Sets flexweights for rendering
void CShaderAPIDX11::SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderAPIDX11::FogMaxDensity(float flMaxDensity)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::AcquireThreadOwnership()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::ReleaseThreadOwnership()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


bool CShaderAPIDX11::SupportsNormalMapCompression() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Only does anything on XBox360. This is useful to eliminate stalls
void CShaderAPIDX11::EnableBuffer2FramesAhead(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// debug logging
// only implemented in some subclasses
void CShaderAPIDX11::PrintfVA(char *fmt, va_list vargs)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::Printf(PRINTF_FORMAT_STRING const char *fmt, ...)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

float CShaderAPIDX11::Knob(char *knobname, float *setvalue)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.f;
}

// Allows us to override the alpha write setting of a material
void CShaderAPIDX11::OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


//extended clear buffers function with alpha independent from color
void CShaderAPIDX11::ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Allows copying a render target to another texture by specifying them both.
void CShaderAPIDX11::CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Allows locking and unlocking of very specific surface types.
void CShaderAPIDX11::LockRect(void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderAPIDX11::UnlockRect(ShaderAPITextureHandle_t texHandle, int mipmap)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// ------------------------------------------------------- //
//                      IDebugTextureInfo                  //
// ------------------------------------------------------- //

// Use this to turn on the mode where it builds the debug texture list.
// At the end of the next frame, GetDebugTextureList() will return a valid list of the textures.
void CShaderAPIDX11::EnableDebugTextureList(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// If this is on, then it will return all textures that exist, not just the ones that were bound in the last frame.
// It is required to enable debug texture list to get this.
void CShaderAPIDX11::EnableGetAllTextures(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
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
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return NULL;
}


// This returns how much memory was used.
int CShaderAPIDX11::GetTextureMemoryUsed(TextureMemoryType eTextureMemory)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Use this to determine if texture debug info was computed within last numFramesAllowed frames.
bool CShaderAPIDX11::IsDebugTextureListFresh(int numFramesAllowed)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// Enable debug texture rendering when texture binds should not count towards textures
// used during a frame. Returns the old state of debug texture rendering flag to use
// it for restoring the mode.
bool CShaderAPIDX11::SetDebugTextureRendering(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}
