
#include "shaderapidx11.h"
#include "dx11global.h"

#include "vcsreader.h"

#include "shaderdevicedx11.h"
#include "hardwareconfigdx11.h"
#include "ishaderdevicemgrdx11.h"
#include "shadershadowdx11.h"
#include "materialdx11.h"
#include "meshdx11.h"
#include "ishaderutil.h"
#include "constantbufferdx11.h"

#include "shaderapi/commandbuffer.h"

#include "memdbgon.h"

static CShaderAPIDX11 s_ShaderAPIDX11;
CShaderAPIDX11 *g_pShaderAPIDX11 = &s_ShaderAPIDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IShaderAPIDX11,
	SHADERAPI_INTERFACE_VERSION, s_ShaderAPIDX11)

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IShaderDynamicAPI,
	SHADERDYNAMIC_INTERFACE_VERSION, s_ShaderAPIDX11)

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IDebugTextureInfoDX11,
	DEBUG_TEXTURE_INFO_VERSION, s_ShaderAPIDX11)

IShaderAPIDX11 *g_pShaderAPI = g_pShaderAPIDX11;

CShaderAPIDX11::CShaderAPIDX11() : m_DynamicState(), m_ShaderState()
{
	m_MatrixMode = MATERIAL_VIEW;

	m_pCurMatrix = &m_ShaderState.m_MatrixStacks[m_MatrixMode].Top();

	m_pDynamicMesh = NULL;

	m_vTextures.RemoveAll();
	m_ModifyTextureHandle = 0;
	m_BackBufferHandle = 0;

	m_StateChanges = STATE_CHANGED_PRIMITIVE_TOPOLOGY;

	m_pMaterial = NULL;
	m_pRenderMesh = NULL;
}

CShaderAPIDX11::~CShaderAPIDX11()
{
	if (m_pDynamicMesh)
		delete m_pDynamicMesh;

	OnDeviceShutdown();
}

void CShaderAPIDX11::HandleStateChanges()
{
	ID3D11DeviceContext *pDeviceContext = g_pShaderDeviceDX11->GetDeviceContext();

	if (m_StateChanges & STATE_CHANGED_VIEWPORTS)
	{
		pDeviceContext->RSSetViewports(m_DynamicState.m_nViewportCount, m_DynamicState.m_pViewports);
	}

	if (m_StateChanges & STATE_CHANGED_VERTEX_BUFFER)
	{
		pDeviceContext->IASetVertexBuffers(0, 1, &m_DynamicState.m_pVertexBuffer, &m_DynamicState.m_VBStride, &m_DynamicState.m_VBOffset);
	}

	if (m_StateChanges & STATE_CHANGED_INDEX_BUFFER)
	{
		pDeviceContext->IASetIndexBuffer(m_DynamicState.m_pIndexBuffer, m_DynamicState.m_IBFmt, m_DynamicState.m_IBOffset);
	}

	if (m_StateChanges & STATE_CHANGED_MATRICES)
	{
		LoadMatConstantBuffer();
	}

	if (m_StateChanges & STATE_CHANGED_CONSTANT_BUFFER)
	{	
		pDeviceContext->PSSetConstantBuffers(0, 1, m_DynamicState.m_ppPSConstantBuffers);
		pDeviceContext->VSSetConstantBuffers(0, 1, m_DynamicState.m_ppVSConstantBuffers);
	}

	if (m_StateChanges & STATE_CHANGED_PRIMITIVE_TOPOLOGY)
	{
		pDeviceContext->IASetPrimitiveTopology(m_DynamicState.m_PrimitiveTopology);
	}

	if (m_StateChanges & STATE_CHANGED_VERTEX_SHADER)
	{
		pDeviceContext->VSSetShader(m_DynamicState.m_pVertexShader, NULL, 0);
	}

	if (m_StateChanges & STATE_CHANGED_GEOMETRY_SHADER)
	{
		Assert(0); // TODO
	}

	if (m_StateChanges & STATE_CHANGED_PIXEL_SHADER)
	{
		pDeviceContext->PSSetShader(m_DynamicState.m_pPixelShader, NULL, 0);
	}

	if (m_StateChanges & STATE_CHANGED_INPUT_LAYOUT)
	{
		m_DynamicState.m_pInputLayout = g_pShaderDeviceDX11->GetInputLayout(m_DynamicState.m_hVertexShader, m_DynamicState.m_VertexFormat);

		pDeviceContext->IASetInputLayout(m_DynamicState.m_pInputLayout);
	}

	if (m_StateChanges & STATE_CHANGED_RENDER_TARGETS)
	{
		// TODO

		ID3D11RenderTargetView *pRTView = m_vTextures[m_BackBufferHandle].GetRenderTargetView();
		pDeviceContext->OMSetRenderTargets(1, &pRTView, NULL);
	}

	if (m_StateChanges & STATE_CHANGED_TEXTURES)
	{
		// TODO: Vertex textures
		pDeviceContext->PSSetShaderResources(0, m_DynamicState.m_nNumSamplers, m_DynamicState.m_ppTextures);
	}

	if (m_StateChanges & STATE_CHANGED_SAMPLERS)
	{
		// TODO: Vertex samplers
		pDeviceContext->PSSetSamplers(0, m_DynamicState.m_nNumSamplers, m_DynamicState.m_ppSamplers);
	}

	m_StateChanges = STATE_CHANGED_NONE;

	// Always update these



}

struct MatrixCBuffers_t // TODO: Only view matrix is required to be reset here
{
	DirectX::XMFLOAT4X4 mModelViewProj;
};

void CShaderAPIDX11::LoadMatConstantBuffer()
{
	MatrixCBuffers_t matBuffer;

	DirectX::XMMATRIX modelViewProj = DirectX::XMMatrixTranspose(m_ShaderState.m_MatrixStacks[MATERIAL_PROJECTION].Top()
		* m_ShaderState.m_MatrixStacks[MATERIAL_VIEW].Top()
		* m_ShaderState.m_MatrixStacks[MATERIAL_MODEL].Top());

	XMStoreFloat4x4(&matBuffer.mModelViewProj, modelViewProj);

	ConstantDesc_t constDesc;
	m_pMatrixConstBuffer->Lock(sizeof(MatrixCBuffers_t), false, constDesc);
	V_memcpy(constDesc.m_pData, &matBuffer, sizeof(MatrixCBuffers_t));
	m_pMatrixConstBuffer->Unlock(sizeof(MatrixCBuffers_t));

	BindConstantBuffer(CBUFFER_VERTEX_SHADER, m_pMatrixConstBuffer, 0);

	m_StateChanges |= STATE_CHANGED_CONSTANT_BUFFER;
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

	m_StateChanges |= STATE_CHANGED_VIEWPORTS;
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
	ALERT_NOT_IMPLEMENTED();
	return MATERIAL_FOG_NONE;
}

void CShaderAPIDX11::GetSceneFogColor(unsigned char *rgb)
{
	ALERT_NOT_IMPLEMENTED();
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

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::LoadMatrix(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMLoadFloat4x4(&inMat);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::MultMatrix(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(*m_pCurMatrix, DirectX::XMLoadFloat4x4(&inMat));

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::MultMatrixLocal(float *m)
{
	DirectX::XMFLOAT4X4 inMat(m);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&inMat), *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
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

	m_StateChanges |= STATE_CHANGED_MATRICES;
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

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::Ortho(double left, double right, double bottom, double top, double zNear, double zFar)
{
	DirectX::XMMATRIX orthoMat = DirectX::XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(orthoMat, *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::PerspectiveX(double fovx, double aspect, double zNear, double zFar)
{
	float width = 2 * zNear * tan(fovx * M_PI / 360.0);
	float height = width / aspect;

	DirectX::XMMATRIX perspMat = DirectX::XMMatrixPerspectiveRH(width, height, zNear, zFar);
	*m_pCurMatrix = DirectX::XMMatrixMultiply(perspMat, *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::PickMatrix(int x, int y, int width, int height)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Rotate(float angle, float x, float y, float z)
{
	DirectX::XMVECTOR rotAxis;
	rotAxis = DirectX::XMVectorSet(x, y, z, 0);

	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(rotAxis, angle * M_PI / 180.0), *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::Translate(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(x, y, z), *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::Scale(float x, float y, float z)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, z), *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}

void CShaderAPIDX11::ScaleXY(float x, float y)
{
	*m_pCurMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(x, y, 1.f), *m_pCurMatrix);

	m_StateChanges |= STATE_CHANGED_MATRICES;
}


// Sets the color to modulate by
void CShaderAPIDX11::Color3f(float r, float g, float b)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color3fv(float const* pColor)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color4f(float r, float g, float b, float a)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color4fv(float const* pColor)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::Color3ub(unsigned char r, unsigned char g, unsigned char b)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color3ubv(unsigned char const* pColor)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Color4ubv(unsigned char const* pColor)
{
	ALERT_NOT_IMPLEMENTED();
}


// Sets the constant register for vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderConstant(int var, float const* pVec, int numConst, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetPixelShaderConstant(int var, float const* pVec, int numConst, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}


// Sets the default *dynamic* state
void CShaderAPIDX11::SetDefaultState()
{
	ALERT_NOT_IMPLEMENTED();
}


// Get the current camera position in world space.
void CShaderAPIDX11::GetWorldSpaceCameraPosition(float* pPos) const
{
	ALERT_NOT_IMPLEMENTED();
}


int CShaderAPIDX11::GetCurrentNumBones(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

int CShaderAPIDX11::GetCurrentLightCombo(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


MaterialFogMode_t CShaderAPIDX11::GetCurrentFogType(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return MATERIAL_FOG_NONE;
}


// fixme: move this to shadow state
void CShaderAPIDX11::SetTextureTransformDimension(TextureStage_t textureStage, int dimension, bool projected)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::DisableTextureTransform(TextureStage_t textureStage)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetBumpEnvMatrix(TextureStage_t textureStage, float m00, float m01, float m10, float m11)
{
	ALERT_NOT_IMPLEMENTED();
}


// Sets the vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderIndex(int vshIndex) // TODO: Invalid checking
{
	ShadowStateDX11 &shadowState = g_pShaderShadowDX11->GetShadowState();
	BindVertexShader(VertexShaderHandle_t(CVCSReader::GetShader(shadowState.m_hStaticVertexShader, vshIndex)));
}

void CShaderAPIDX11::SetPixelShaderIndex(int pshIndex)
{
	ShadowStateDX11 &shadowState = g_pShaderShadowDX11->GetShadowState();
	BindPixelShader(PixelShaderHandle_t(CVCSReader::GetShader(shadowState.m_hStaticPixelShader, pshIndex)));
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
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

static LightDesc_t s_tmpLightDesc;

const LightDesc_t& CShaderAPIDX11::GetLight(int lightNum) const
{
	ALERT_NOT_IMPLEMENTED();
	return s_tmpLightDesc;
}


void CShaderAPIDX11::SetPixelShaderFogParams(int reg)
{
	ALERT_NOT_IMPLEMENTED();
}


// Render state for the ambient light cube
void CShaderAPIDX11::SetVertexShaderStateAmbientLightCube()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetPixelShaderStateAmbientLightCube(int pshReg, bool bForceToBlack)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::CommitPixelShaderLighting(int pshReg)
{
	ALERT_NOT_IMPLEMENTED();
}


// Use this to get the mesh builder that allows us to modify vertex data
CMeshBuilder* CShaderAPIDX11::GetVertexModifyBuilder()
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

bool CShaderAPIDX11::InFlashlightMode() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

static FlashlightState_t s_tmpFlashlightState;

const FlashlightState_t &CShaderAPIDX11::GetFlashlightState(VMatrix &worldToTexture) const
{
	ALERT_NOT_IMPLEMENTED();
	return s_tmpFlashlightState;
}

bool CShaderAPIDX11::InEditorMode() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Gets the bound morph's vertex format; returns 0 if no morph is bound
MorphFormat_t CShaderAPIDX11::GetBoundMorphFormat()
{
	ALERT_NOT_IMPLEMENTED();
	return MORPH_NORMAL;
}


// Binds a standard texture
void CShaderAPIDX11::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	g_pShaderUtil->BindStandardTexture(sampler, id);
}


ITexture *CShaderAPIDX11::GetRenderTargetEx(int nRenderTargetID)
{
	return g_pShaderUtil->GetRenderTargetEx(nRenderTargetID);
}


void CShaderAPIDX11::SetToneMappingScaleLinear(const Vector &scale)
{
	ALERT_NOT_IMPLEMENTED();
}

static Vector s_tmpVec;
const Vector &CShaderAPIDX11::GetToneMappingScaleLinear(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return s_tmpVec;
}

float CShaderAPIDX11::GetLightMapScaleFactor(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1.0f;
}


void CShaderAPIDX11::LoadBoneMatrix(int boneIndex, const float *m)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::GetDXLevelDefaults(uint &max_dxlevel, uint &recommended_dxlevel)
{
	ALERT_NOT_IMPLEMENTED();
}


const FlashlightState_t &CShaderAPIDX11::GetFlashlightStateEx(VMatrix &worldToTexture, ITexture **pFlashlightDepthTexture) const
{
	ALERT_NOT_IMPLEMENTED();
	return s_tmpFlashlightState;
}


float CShaderAPIDX11::GetAmbientLightCubeLuminance()
{
	ALERT_NOT_IMPLEMENTED();
	return -1.0f;
}


void CShaderAPIDX11::GetDX9LightState(LightState_t *state) const
{
	ALERT_NOT_IMPLEMENTED();
}

int CShaderAPIDX11::GetPixelFogCombo()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}
 //0 is either range fog, or no fog simulated with rigged range fog values. 1 is height fog

void CShaderAPIDX11::BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id)
{
	g_pShaderUtil->BindStandardVertexTexture(sampler, id);
}


// Is hardware morphing enabled?
bool CShaderAPIDX11::IsHWMorphingEnabled() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


void CShaderAPIDX11::GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetBooleanVertexShaderConstant(int var, BOOL const* pVec, int numBools, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetIntegerVertexShaderConstant(int var, int const* pVec, int numIntVecs, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetBooleanPixelShaderConstant(int var, BOOL const* pVec, int numBools, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetIntegerPixelShaderConstant(int var, int const* pVec, int numIntVecs, bool bForce)
{
	ALERT_NOT_IMPLEMENTED();
}


//Are we in a configuration that needs access to depth data through the alpha channel later?
bool CShaderAPIDX11::ShouldWriteDepthToDestAlpha(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}



// deformations
void CShaderAPIDX11::PushDeformation(DeformationBase_t const *Deformation)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::PopDeformation()
{
	ALERT_NOT_IMPLEMENTED();
}

int CShaderAPIDX11::GetNumActiveDeformations() const
{
	ALERT_NOT_IMPLEMENTED();
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
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


// This lets the lower level system that certain vertex fields requested 
// in the shadow state aren't actually being read given particular state
// known only at dynamic state time. It's here only to silence warnings.
void CShaderAPIDX11::MarkUnusedVertexFields(unsigned int nFlags, int nTexCoordCount, bool *pUnusedTexCoords)
{
	ALERT_NOT_IMPLEMENTED();
}

#define INTERPRET_DATA(_type, _ptr) (*(reinterpret_cast<_type const *>(_ptr)))

void CShaderAPIDX11::ExecuteCommandBuffer(uint8 *pCmdBuffer)
{
	for (;;)
	{
		uint8 curCmd = *pCmdBuffer;
		pCmdBuffer += sizeof(int);

		switch (curCmd)
		{
		case CBCMD_END:
			return;
		case CBCMD_JUMP:
		{
			pCmdBuffer = INTERPRET_DATA(uint8 *, pCmdBuffer);
		}
		break;

		case CBCMD_JSR:
		{
			ExecuteCommandBuffer(INTERPRET_DATA(uint8 *, pCmdBuffer));
			pCmdBuffer += sizeof(uint8 *);
		}
		break;

		case CBCMD_SET_PIXEL_SHADER_FLOAT_CONST:
		{
			int first_reg = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			int nregs = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			float *values = INTERPRET_DATA(float *, pCmdBuffer);
			pCmdBuffer += sizeof(float) * nregs * 4;

			SetPixelShaderConstant(first_reg, values, nregs, false); // maybe true
		}
		break;

		case CBCMD_SET_VERTEX_SHADER_FLOAT_CONST:
		{
			int first_reg = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			int nregs = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			float *values = INTERPRET_DATA(float *, pCmdBuffer);
			pCmdBuffer += sizeof(float) * nregs * 4;

			SetVertexShaderConstant(first_reg, values, nregs, false); // maybe true
		}
		break;

		case CBCMD_SETPIXELSHADERFOGPARAMS:
		{
			int regdest = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			SetPixelShaderFogParams(regdest);
		}
		break;

		case CBCMD_COMMITPIXELSHADERLIGHTING:
		{
			int regdest = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			CommitPixelShaderLighting(regdest);
		}
		break;

		case CBCMD_SETPIXELSHADERSTATEAMBIENTLIGHTCUBE:
		{
			int regdest = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			SetPixelShaderStateAmbientLightCube(regdest);
		}
		break;

		case CBCMD_SETAMBIENTCUBEDYNAMICSTATEVERTEXSHADER:
		{
			SetVertexShaderStateAmbientLightCube();
		}
		break;

		case CBCMD_SET_DEPTH_FEATHERING_CONST:
		{
			int constReg = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			float blendScale = INTERPRET_DATA(float, pCmdBuffer);
			pCmdBuffer += sizeof(float);

			SetDepthFeatheringPixelShaderConstant(constReg, blendScale);
		}
		break;

		case CBCMD_BIND_STANDARD_TEXTURE:
		{
			Sampler_t sampler = INTERPRET_DATA(Sampler_t, pCmdBuffer);
			pCmdBuffer += sizeof(Sampler_t);
			
			StandardTextureId_t texID = INTERPRET_DATA(StandardTextureId_t, pCmdBuffer);
			pCmdBuffer += sizeof(StandardTextureId_t);

			BindStandardTexture(sampler, texID);
		}
		break;

		case CBCMD_BIND_SHADERAPI_TEXTURE_HANDLE:
		{
			Sampler_t sampler = INTERPRET_DATA(Sampler_t, pCmdBuffer);
			pCmdBuffer += sizeof(Sampler_t);

			ShaderAPITextureHandle_t hTex = INTERPRET_DATA(ShaderAPITextureHandle_t, pCmdBuffer);
			pCmdBuffer += sizeof(ShaderAPITextureHandle_t);

			BindTexture(sampler, hTex);
		}
		break;

		case CBCMD_SET_PSHINDEX:
		{
			int shaderInd = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			SetPixelShaderIndex(shaderInd);
		}
		break;

		case CBCMD_SET_VSHINDEX:
		{
			int shaderInd = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);

			SetVertexShaderIndex(shaderInd);
		}
		break;

		case CBCMD_SET_VERTEX_SHADER_FLASHLIGHT_STATE:
		case CBCMD_STORE_EYE_POS_IN_PSCONST:
		{
			pCmdBuffer += sizeof(int);
		}
		break;
		case CBCMD_SET_VERTEX_SHADER_FLOAT_CONST_REF:
		{
			//int first_reg = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			int nregs = INTERPRET_DATA(int, pCmdBuffer);
			pCmdBuffer += sizeof(int);
			//float *values = INTERPRET_DATA(float *, pCmdBuffer);
			pCmdBuffer += sizeof(float) * nregs * 4;
		}
		break;

		case CBCMD_SET_PIXEL_SHADER_FLASHLIGHT_STATE:
		{
			pCmdBuffer += sizeof(int) * 3 + sizeof(Sampler_t);
		}
		break;

		case CBCMD_SET_PIXEL_SHADER_UBERLIGHT_STATE:
		case CBCMD_SET_VERTEX_SHADER_NEARZFARZ_STATE:
		default:
			break; // Lole
		}
	}
}


// interface for mat system to tell shaderapi about standard texture handles
void CShaderAPIDX11::SetStandardTextureHandle(StandardTextureId_t nId, ShaderAPITextureHandle_t nHandle)
{
	ALERT_NOT_IMPLEMENTED();
}


// Interface for mat system to tell shaderapi about color correction
void CShaderAPIDX11::GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetPSNearAndFarZ(int pshReg)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetDepthFeatheringPixelShaderConstant(int iConstant, float fDepthBlendScale)
{
	ALERT_NOT_IMPLEMENTED();
}


// Buffer clearing
void CShaderAPIDX11::ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil, int renderTargetWidth, int renderTargetHeight)
{
	if (!g_pShaderDeviceDX11->IsActivated()) return;

	if (bClearColor)
	{
		ID3D11RenderTargetView* pRTView;

		pRTView = m_vTextures[m_BackBufferHandle].GetRenderTargetView();
		g_pShaderDeviceDX11->GetDeviceContext()->ClearRenderTargetView(pRTView, m_DynamicState.m_ClearColor);
	}
}

void CShaderAPIDX11::ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	m_DynamicState.m_ClearColor[0] = r / 255.f;
	m_DynamicState.m_ClearColor[1] = g / 255.f;
	m_DynamicState.m_ClearColor[2] = b / 255.f;
	m_DynamicState.m_ClearColor[3] = 1.f;
}

void CShaderAPIDX11::ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_DynamicState.m_ClearColor[0] = r / 255.f;
	m_DynamicState.m_ClearColor[1] = g / 255.f;
	m_DynamicState.m_ClearColor[2] = b / 255.f;
	m_DynamicState.m_ClearColor[3] = a / 255.f;
}


// Methods related to binding shaders
void CShaderAPIDX11::BindVertexShader(VertexShaderHandle_t hVertexShader)
{
	if (hVertexShader == VERTEX_SHADER_HANDLE_INVALID)
		return;

	ID3D11VertexShader* pVertexShader = g_pShaderDeviceDX11->GetVertexShader(hVertexShader);
	if (m_DynamicState.m_pVertexShader != pVertexShader || m_DynamicState.m_hVertexShader != hVertexShader)
	{
		m_DynamicState.m_pVertexShader = pVertexShader;
		m_DynamicState.m_hVertexShader = hVertexShader;

		m_StateChanges |= STATE_CHANGED_VERTEX_SHADER | STATE_CHANGED_INPUT_LAYOUT;
	}
}

void CShaderAPIDX11::UnbindVertexShader(VertexShaderHandle_t hVertexShader)
{
	if (hVertexShader == VERTEX_SHADER_HANDLE_INVALID)
		return;

	ID3D11VertexShader* pVertexShader = g_pShaderDeviceDX11->GetVertexShader(hVertexShader);
	if (pVertexShader == m_DynamicState.m_pVertexShader)
	{
		BindVertexShader(VERTEX_SHADER_HANDLE_INVALID);
	}
}

void CShaderAPIDX11::BindGeometryShader(GeometryShaderHandle_t hGeometryShader)
{
	if (hGeometryShader == GEOMETRY_SHADER_HANDLE_INVALID)
		return;

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::BindPixelShader(PixelShaderHandle_t hPixelShader)
{
	if (hPixelShader == PIXEL_SHADER_HANDLE_INVALID)
		return;

	ID3D11PixelShader* pPixelShader = g_pShaderDeviceDX11->GetPixelShader(hPixelShader);
	if (pPixelShader != m_DynamicState.m_pPixelShader)
	{
		m_DynamicState.m_pPixelShader = pPixelShader;
		m_StateChanges |= STATE_CHANGED_PIXEL_SHADER;
	}
}

void CShaderAPIDX11::UnbindPixelShader(PixelShaderHandle_t hPixelShader)
{
	if (hPixelShader == PIXEL_SHADER_HANDLE_INVALID)
		return;

	ID3D11PixelShader* pPixelShader = g_pShaderDeviceDX11->GetPixelShader(hPixelShader);
	if (pPixelShader == m_DynamicState.m_pPixelShader)
	{
		BindPixelShader(PIXEL_SHADER_HANDLE_INVALID);
	}
}


// Methods related to state objects
void CShaderAPIDX11::SetRasterState(const ShaderRasterState_t& state)
{
	ALERT_NOT_IMPLEMENTED();
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
	ALERT_NOT_IMPLEMENTED();
}


// Returns the snapshot id for the shader state
StateSnapshot_t	 CShaderAPIDX11::TakeSnapshot()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


void CShaderAPIDX11::TexMinFilter(ShaderTexFilterMode_t texFilterMode)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::TexMagFilter(ShaderTexFilterMode_t texFilterMode)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::TexWrap(ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::CopyRenderTargetToTexture(ShaderAPITextureHandle_t textureHandle)
{
	ALERT_NOT_IMPLEMENTED();
}


// Binds a particular material to render with
void CShaderAPIDX11::Bind(IMaterial* pMaterial)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	 // TODO: Check if materials are the same
	m_pMaterial = static_cast<CMaterialDX11 *>(pMaterial);
}


// Flushes any primitives that are buffered
void CShaderAPIDX11::FlushBufferedPrimitives()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Gets the dynamic mesh; note that you've got to render the mesh
// before calling this function a second time. Clients should *not*
// call DestroyStaticMesh on the mesh returned by this call.
IMesh* CShaderAPIDX11::GetDynamicMesh(IMaterial* pMaterial, int nHWSkinBoneCount, bool bBuffered,
	IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	return GetDynamicMeshEx(pMaterial, pMaterial->GetVertexFormat(), nHWSkinBoneCount, bBuffered, pVertexOverride, pIndexOverride);
}

IMesh* CShaderAPIDX11::GetDynamicMeshEx(IMaterial* pMaterial, VertexFormat_t vertexFormat, int nHWSkinBoneCount,
	bool bBuffered, IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	ALERT_INCOMPLETE();

	if (!m_pDynamicMesh)
		m_pDynamicMesh = new CMeshDX11(true, vertexFormat);

	if (vertexFormat != VERTEX_FORMAT_UNKNOWN)
	{
		CMeshDX11 *pDynamicMeshDX11 = (CMeshDX11 *)m_pDynamicMesh;

		pDynamicMeshDX11->SetVertexFormat(vertexFormat);
	}

	return m_pDynamicMesh;
}


// Methods to ask about particular state snapshots
bool CShaderAPIDX11::IsTranslucent(StateSnapshot_t id) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CShaderAPIDX11::IsAlphaTested(StateSnapshot_t id) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CShaderAPIDX11::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	ALERT_NOT_IMPLEMENTED();
	return true;
}

bool CShaderAPIDX11::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Gets the vertex format for a set of snapshot ids
VertexFormat_t CShaderAPIDX11::ComputeVertexFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return m_DynamicState.m_VertexFormat; // FIXME FIXME
}


// What fields in the vertex do we actually use?
VertexFormat_t CShaderAPIDX11::ComputeVertexUsage(int numSnapshots, StateSnapshot_t* pIds) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return m_DynamicState.m_VertexFormat; // FIXME FIXME
}


// Begins a rendering pass
void CShaderAPIDX11::BeginPass(StateSnapshot_t snapshot)
{
	ALERT_NOT_IMPLEMENTED();
}


// Renders a single pass of a material
void CShaderAPIDX11::RenderPass(int nPass, int nPassCount)
{
	ALERT_INCOMPLETE();

	HandleStateChanges();

	g_pShaderDeviceDX11->GetDeviceContext()->DrawIndexed(m_pRenderMesh->IndexCount(), 0, 0);
}


// Set the number of bone weights
void CShaderAPIDX11::SetNumBoneWeights(int numBones)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Sets the lights
void CShaderAPIDX11::SetLight(int lightNum, const LightDesc_t& desc)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Lighting origin for the current model
void CShaderAPIDX11::SetLightingOrigin(Vector vLightingOrigin)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetAmbientLight(float r, float g, float b)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetAmbientLightCube(Vector4D cube[6])
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// The shade mode
void CShaderAPIDX11::ShadeMode(ShaderShadeMode_t mode)
{
	ALERT_NOT_IMPLEMENTED();
}


// The cull mode
void CShaderAPIDX11::CullMode(MaterialCullMode_t cullMode)
{
	ALERT_NOT_IMPLEMENTED();
}


// Force writes only when z matches. . . useful for stenciling things out
// by rendering the desired Z values ahead of time.
void CShaderAPIDX11::ForceDepthFuncEquals(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Forces Z buffering to be on or off
void CShaderAPIDX11::OverrideDepthEnable(bool bEnable, bool bDepthEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetHeightClipZ(float z)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetHeightClipMode(enum MaterialHeightClipMode_t heightClipMode)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetClipPlane(int index, const float *pPlane)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EnableClipPlane(int index, bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Put all the model matrices into vertex shader constants.
void CShaderAPIDX11::SetSkinningMatrices()
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns the nearest supported format
ImageFormat CShaderAPIDX11::GetNearestSupportedFormat(ImageFormat fmt, bool bFilteringRequired) const
{
	//return IMAGE_FORMAT_UNKNOWN;

	return CAPITextureDX11::ResolveToSupportedFormat(fmt);
}

ImageFormat CShaderAPIDX11::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	//return IMAGE_FORMAT_UNKNOWN;

	return CAPITextureDX11::ResolveToSupportedFormat(fmt);
}


// When AA is enabled, render targets are not AA and require a separate
// depth buffer.
bool CShaderAPIDX11::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	ALERT_NOT_IMPLEMENTED();
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
	ShaderAPITextureHandle_t texHandle;
	CreateTextures(&texHandle, 1, width, height, depth, dstImageFormat, numMipLevels, numCopies, flags, pDebugName, pTextureGroupName);
	return texHandle;
}

void CShaderAPIDX11::DeleteTexture(ShaderAPITextureHandle_t textureHandle)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	m_vTextures[textureHandle].Shutdown();
}


ShaderAPITextureHandle_t CShaderAPIDX11::CreateDepthTexture(
ImageFormat renderTargetFormat,
int width,
int height,
const char *pDebugName,
bool bTexture)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

bool CShaderAPIDX11::IsTexture(ShaderAPITextureHandle_t textureHandle)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CShaderAPIDX11::IsTextureResident(ShaderAPITextureHandle_t textureHandle)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Indicates we're going to be modifying this texture
// TexImage2D, TexSubImage2D, TexWrap, TexMinFilter, and TexMagFilter
// all use the texture specified by this function.
void CShaderAPIDX11::ModifyTexture(ShaderAPITextureHandle_t textureHandle)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	m_ModifyTextureHandle = textureHandle;
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
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	TexSubImage2D(level, cubeFaceID, 0, 0, zOffset, width, height, srcFormat, 0, bSrcIsTiled, imageData);
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
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	CAPITextureDX11 &tex = m_vTextures[m_ModifyTextureHandle];

	tex.LoadImage2D(level, cubeFaceID, xOffset, yOffset, zOffset, width, height, srcFormat, srcStride, bSrcIsTiled, imageData);
}

void CShaderAPIDX11::TexImageFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_INCOMPLETE();
	// Needs miplevel and cubemap/volume support

	unsigned char *imageData = pVTF->ImageData( iVTFFrame, 0, 0);

	if (!imageData) return;

	ImageFormat format = pVTF->Format();

	TexSubImage2D(0, 0, 0, 0, 0, pVTF->Width(), pVTF->Height(), format, 0, false, imageData);
}


// An alternate (and faster) way of writing image data
// (locks the current Modify Texture). Use the pixel writer to write the data
// after Lock is called
// Doesn't work for compressed textures 
bool CShaderAPIDX11::TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
	int width, int height, CPixelWriter& writer)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CShaderAPIDX11::TexUnlock()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// These are bound to the texture
void CShaderAPIDX11::TexSetPriority(int priority)
{
	ALERT_NOT_IMPLEMENTED();
}


// Sets the texture state
void CShaderAPIDX11::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle)
{
	if (sampler >= MAX_DX11_SAMPLERS)
		return;

	CAPITextureDX11 *pTexture = &m_vTextures[textureHandle];
	if (!pTexture)
		return;

	ID3D11ShaderResourceView *pResourceView = pTexture->GetResourceView();
	ID3D11SamplerState *pSamplerState = pTexture->GetSamplerState();
	if (pResourceView != m_DynamicState.m_ppTextures[sampler])
	{
		m_DynamicState.m_ppTextures[sampler] = pResourceView;

		m_StateChanges |= STATE_CHANGED_TEXTURES;
	}

	if (pSamplerState != m_DynamicState.m_ppSamplers[sampler])
	{
		m_DynamicState.m_ppSamplers[sampler] = pSamplerState;

		m_StateChanges |= STATE_CHANGED_SAMPLERS;
	}

	if (sampler >= m_DynamicState.m_nNumSamplers)
	{
		m_DynamicState.m_nNumSamplers = sampler + 1;
		m_StateChanges |= STATE_CHANGED_TEXTURES | STATE_CHANGED_SAMPLERS;
	}
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTarget(ShaderAPITextureHandle_t colorTextureHandle,
	ShaderAPITextureHandle_t depthTextureHandle)
{
	ALERT_NOT_IMPLEMENTED();
}

// stuff that isn't to be used from within a shader
void CShaderAPIDX11::ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::ReadPixels(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *data, ImageFormat dstFormat, int nDstStride)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::FlushHardware()
{
	ALERT_NOT_IMPLEMENTED();
}


// Use this to begin and end the frame
void CShaderAPIDX11::BeginFrame()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EndFrame()
{
	ALERT_NOT_IMPLEMENTED();
}


// Selection mode methods
int  CShaderAPIDX11::SelectionMode(bool selectionMode)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return -1;
}

void CShaderAPIDX11::SelectionBuffer(unsigned int* pBuffer, int size)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::ClearSelectionNames()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::LoadSelectionName(int name)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::PushSelectionName(int name)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::PopSelectionName()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Force the hardware to finish whatever it's doing
void CShaderAPIDX11::ForceHardwareSync()
{
	ALERT_NOT_IMPLEMENTED();
}


// Used to clear the transition table when we know it's become invalid.
void CShaderAPIDX11::ClearSnapshots()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::FogStart(float fStart)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::FogEnd(float fEnd)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetFogZ(float fogZ)
{
	ALERT_NOT_IMPLEMENTED();
}

// Scene fog state.
void CShaderAPIDX11::SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SceneFogMode(MaterialFogMode_t fogMode)
{
	ALERT_NOT_IMPLEMENTED();
}


// Can we download textures?
bool CShaderAPIDX11::CanDownloadTextures() const
{
	return g_pShaderDeviceDX11->IsActivated();
}


void CShaderAPIDX11::ResetRenderState(bool bFullReset)
{
	ALERT_NOT_IMPLEMENTED();
}


// We use smaller dynamic VBs during level transitions, to free up memory
int  CShaderAPIDX11::GetCurrentDynamicVBSize(void)
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

void CShaderAPIDX11::DestroyVertexBuffers(bool bExitingLevel)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::EvictManagedResources()
{
	ALERT_NOT_IMPLEMENTED();
}


// Level of anisotropic filtering
void CShaderAPIDX11::SetAnisotropicLevel(int nAnisotropyLevel)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// For debugging and building recording files. This will stuff a token into the recording file,
// then someone doing a playback can watch for the token.
void CShaderAPIDX11::SyncToken(const char *pToken)
{
	ALERT_NOT_IMPLEMENTED();
}


// Setup standard vertex shader constants (that don't change)
// This needs to be called anytime that overbright changes.
void CShaderAPIDX11::SetStandardVertexShaderConstants(float fOverbright)
{
	ALERT_NOT_IMPLEMENTED();
}


//
// Occlusion query support
//

// Allocate and delete query objects.
ShaderAPIOcclusionQuery_t CShaderAPIDX11::CreateOcclusionQueryObject(void)
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}

void CShaderAPIDX11::DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t)
{
	ALERT_NOT_IMPLEMENTED();
}


// Bracket drawing with begin and end so that we can get counts next frame.
void CShaderAPIDX11::BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	ALERT_NOT_IMPLEMENTED();
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
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


void CShaderAPIDX11::SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::ClearVertexAndPixelShaderRefCounts()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::PurgeUnusedVertexAndPixelShaders()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Called when the dx support level has changed
void CShaderAPIDX11::DXSupportLevelChanged()
{
	ALERT_NOT_IMPLEMENTED();
}


// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
// planes (which are specified in world space) to generate projection-space user clip planes
// Occasionally (for the particle system in hl2, for example), we want to override that
// behavior and explictly specify a View transform for user clip planes. The PROJECTION
// will be mutliplied against this instead of the normal VIEW matrix.
void CShaderAPIDX11::EnableUserClipTransformOverride(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::UserClipTransform(const VMatrix &worldToView)
{
	ALERT_NOT_IMPLEMENTED();
}


// ----------------------------------------------------------------------------------
// Everything after this point added after HL2 shipped.
// ----------------------------------------------------------------------------------

// What fields in the morph do we actually use?
MorphFormat_t CShaderAPIDX11::ComputeMorphFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return MORPH_NORMAL;
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTargetEx(int nRenderTargetID,
ShaderAPITextureHandle_t colorTextureHandle,
ShaderAPITextureHandle_t depthTextureHandle)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t textureHandle, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::CopyTextureToRenderTargetEx(int nRenderTargetID, ShaderAPITextureHandle_t textureHandle, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
void CShaderAPIDX11::HandleDeviceLost()
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::EnableLinearColorSpaceFrameBuffer(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Lets the shader know about the full-screen texture so it can 
void CShaderAPIDX11::SetFullScreenTextureHandle(ShaderAPITextureHandle_t h)
{
	ALERT_NOT_IMPLEMENTED();
}


// Rendering parameters control special drawing modes withing the material system, shader
// system, shaders, and engine. renderparm.h has their definitions.
void CShaderAPIDX11::SetFloatRenderingParameter(int parm_number, float value)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetIntRenderingParameter(int parm_number, int value)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetVectorRenderingParameter(int parm_number, Vector const &value)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


float CShaderAPIDX11::GetFloatRenderingParameter(int parm_number) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return -1.f;
}

int CShaderAPIDX11::GetIntRenderingParameter(int parm_number) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return -1;
}

Vector CShaderAPIDX11::GetVectorRenderingParameter(int parm_number) const
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return s_tmpVec;
}


void CShaderAPIDX11::SetFastClipPlane(const float *pPlane)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EnableFastClip(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns the number of vertices + indices we can render using the dynamic mesh
// Passing true in the second parameter will return the max # of vertices + indices
// we can use before a flush is provoked and may return different values 
// if called multiple times in succession. 
// Passing false into the second parameter will return
// the maximum possible vertices + indices that can be rendered in a single batch
void CShaderAPIDX11::GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices)
{
	ALERT_NOT_IMPLEMENTED();
}


// Returns the max number of vertices we can render for a given material
int CShaderAPIDX11::GetMaxVerticesToRender(IMaterial *pMaterial)
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

int CShaderAPIDX11::GetMaxIndicesToRender()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


// stencil methods
void CShaderAPIDX11::SetStencilEnable(bool onoff)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilFailOperation(StencilOperation_t op)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilZFailOperation(StencilOperation_t op)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilPassOperation(StencilOperation_t op)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilCompareFunction(StencilComparisonFunction_t cmpfn)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilReferenceValue(int ref)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilTestMask(uint32 msk)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetStencilWriteMask(uint32 msk)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value)
{
	ALERT_NOT_IMPLEMENTED();
}


// disables all local lights
void CShaderAPIDX11::DisableAllLocalLights()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}

int CShaderAPIDX11::CompareSnapshots(StateSnapshot_t snapshot0, StateSnapshot_t snapshot1)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
	return -1;
}


IMesh *CShaderAPIDX11::GetFlexMesh()
{
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}


void CShaderAPIDX11::SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


bool CShaderAPIDX11::SupportsMSAAMode(int nMSAAMode)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


bool CShaderAPIDX11::OwnGPUResources(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
void CShaderAPIDX11::GetFogDistances(float *fStart, float *fEnd, float *fFogZ)
{
	ALERT_NOT_IMPLEMENTED();
}


// Hooks for firing PIX events from outside the Material System...
void CShaderAPIDX11::BeginPIXEvent(unsigned long color, const char *szName)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EndPIXEvent()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::SetPIXMarker(unsigned long color, const char *szName)
{
	ALERT_NOT_IMPLEMENTED();
}


// Enables and disables for Alpha To Coverage
void CShaderAPIDX11::EnableAlphaToCoverage()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::DisableAlphaToCoverage()
{
	ALERT_NOT_IMPLEMENTED();
}


// Computes the vertex buffer pointers 
void CShaderAPIDX11::ComputeVertexDescription(unsigned char* pBuffer, VertexFormat_t vertexFormat, MeshDesc_t& desc) const
{
	ComputeVertexDesc(pBuffer, vertexFormat, desc);
}


bool CShaderAPIDX11::SupportsShadowDepthTextures(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


void CShaderAPIDX11::SetDisallowAccess(bool)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::EnableShaderShaderMutex(bool)
{
	ALERT_NOT_IMPLEMENTED();
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
	ALERT_NOT_IMPLEMENTED();
	return IMAGE_FORMAT_UNKNOWN;
}


bool CShaderAPIDX11::SupportsFetch4(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CShaderAPIDX11::SetShadowDepthBiasFactors(float fShadowSlopeScaleDepthBias, float fShadowDepthBias)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::VertexShaderVertexFormat(unsigned int nFlags, int nTexCoordCount, int* pTexCoordDimensions, int nUserDataSize)
{
	VertexFormat_t fmt = nFlags & ~VERTEX_FORMAT_USE_EXACT_FORMAT;

	if (g_pHardwareConfig->SupportsCompressedVertices() == VERTEX_COMPRESSION_NONE)
	{
		fmt &= ~VERTEX_FORMAT_COMPRESSED;
	}

	fmt |= VERTEX_USERDATA_SIZE(nUserDataSize);

	nTexCoordCount = min(nTexCoordCount, VERTEX_MAX_TEXTURE_COORDINATES);
	for (int i = 0; i < nTexCoordCount; ++i)
	{
		// A nullptr means 2 dimensions
		int nDimensions = 2;

		if (pTexCoordDimensions)
		{
			nDimensions = pTexCoordDimensions[i];
		}

		fmt |= VERTEX_TEXCOORD_SIZE(i, nDimensions);
	}

	if (m_DynamicState.m_VertexFormat != fmt)
	{
		m_DynamicState.m_VertexFormat = fmt;

		m_StateChanges |= STATE_CHANGED_INPUT_LAYOUT;
	}

	VertexDesc_t fmtDesc;
	ComputeVertexDesc(0, fmt, fmtDesc);

	UINT bufStride = fmtDesc.m_ActualVertexSize;

	if (m_DynamicState.m_VBStride != bufStride)
	{
		m_DynamicState.m_VBStride = bufStride;

		m_StateChanges |= STATE_CHANGED_VERTEX_BUFFER;
	}
}


// ------------ New Vertex/Index Buffer interface ----------------------------
void CShaderAPIDX11::BindVertexBuffer(int nStreamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions)
{
	CVertexBufferDX11 *pVertexBufferDX11 = static_cast<CVertexBufferDX11*>(pVertexBuffer);
	ID3D11Buffer *pBuffer = NULL;
	UINT bufStride = 0;
	UINT nOffset = nOffsetInBytes;
	if (pVertexBufferDX11)
	{
		pBuffer = pVertexBufferDX11->GetBuffer();
		bufStride = fmt == VERTEX_FORMAT_UNKNOWN ? m_DynamicState.m_VBStride : pVertexBufferDX11->VertexSize();
	}

	if (nOffsetInBytes < 0)
	{
		nOffset = nFirstVertex * bufStride;
	}

	if (m_DynamicState.m_pVertexBuffer != pBuffer || m_DynamicState.m_VBStride != bufStride || m_DynamicState.m_VBOffset != nOffset)
	{
		m_DynamicState.m_pVertexBuffer = pBuffer;
		m_DynamicState.m_VBStride = bufStride;
		m_DynamicState.m_VBOffset = nOffset;

		m_StateChanges |= STATE_CHANGED_VERTEX_BUFFER;
	}

	if (m_DynamicState.m_VertexFormat != fmt)
	{
		m_DynamicState.m_VertexFormat = fmt;
	
		m_StateChanges |= STATE_CHANGED_INPUT_LAYOUT;
	}
}

void CShaderAPIDX11::BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes)
{
	CIndexBufferDX11 *pVertexBufferDX11 = static_cast<CIndexBufferDX11*>(pIndexBuffer);
	ID3D11Buffer *pBuffer = NULL;
	UINT indexSize = 0;
	UINT nOffset = nOffsetInBytes;
	if (pVertexBufferDX11)
	{
		pBuffer = pVertexBufferDX11->GetBuffer();
		indexSize = pVertexBufferDX11->IndexSize();
	}

	DXGI_FORMAT indexFormat = (indexSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	if (m_DynamicState.m_pIndexBuffer != pBuffer || m_DynamicState.m_IBFmt != indexFormat || m_DynamicState.m_IBOffset != nOffset)
	{
		m_DynamicState.m_pIndexBuffer = pBuffer;
		m_DynamicState.m_IBFmt = indexFormat;
		m_DynamicState.m_IBOffset = nOffset;

		m_StateChanges |= STATE_CHANGED_INDEX_BUFFER;
	}
}

void CShaderAPIDX11::BindConstantBuffer(ConstantBufferType_t type, IConstantBufferDX11 *pConstantBuffer, int nOffsetInBytes, int nChannel) // TODO: CLEAN THIS UP A MILLION TIMES
{
	CConstantBufferDX11 *pConstantBufferDX11 = static_cast<CConstantBufferDX11*>(pConstantBuffer);
	ID3D11Buffer *pBuffer = NULL;
	UINT nOffset = nOffsetInBytes;
	if (pConstantBufferDX11)
	{
		pBuffer = pConstantBufferDX11->GetBuffer();
	}

	if (IsVertexShaderType(type))
	{
		if (m_DynamicState.m_ppVSConstantBuffers[nChannel] != pBuffer || m_DynamicState.m_pVSCBOffsets[nChannel] != nOffset)
		{
			m_DynamicState.m_ppVSConstantBuffers[nChannel] = pBuffer;
			m_DynamicState.m_pVSCBOffsets[nChannel] = nOffset;

			m_StateChanges |= STATE_CHANGED_CONSTANT_BUFFER;
		}
	}
	else
	{
		if (m_DynamicState.m_ppPSConstantBuffers[nChannel] != pBuffer || m_DynamicState.m_pPSCBOffsets[nChannel] != nOffset)
		{
			m_DynamicState.m_ppPSConstantBuffers[nChannel] = pBuffer;
			m_DynamicState.m_pPSCBOffsets[nChannel] = nOffset;

			m_StateChanges |= STATE_CHANGED_CONSTANT_BUFFER;
		}
	}
}

void CShaderAPIDX11::Draw(MaterialPrimitiveType_t primitiveType, int nFirstIndex, int nIndexCount)
{
	ALERT_INCOMPLETE();
	SetPrimitiveTopology(primitiveType);

	HandleStateChanges();

	g_pShaderDeviceDX11->GetDeviceContext()->DrawIndexed(nIndexCount, nFirstIndex, 0);
}

void CShaderAPIDX11::SetPrimitiveTopology(MaterialPrimitiveType_t primitiveType)
{
	D3D11_PRIMITIVE_TOPOLOGY d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	switch (primitiveType)
	{

	case MATERIAL_POINTS:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;

	case MATERIAL_LINES:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;

	case MATERIAL_TRIANGLES:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;

	case MATERIAL_TRIANGLE_STRIP:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;

	case MATERIAL_LINE_STRIP:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;

	default:
		d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		break;

	}

	if (m_DynamicState.m_PrimitiveTopology == d3dTopology)
		return;

	m_DynamicState.m_PrimitiveTopology = d3dTopology;
	m_StateChanges |= STATE_CHANGED_PRIMITIVE_TOPOLOGY;
}

void CShaderAPIDX11::DrawMesh(IMesh *pMesh)
{
	m_pRenderMesh = static_cast<CMeshDX11 *>(pMesh);

	SetPrimitiveTopology(m_pRenderMesh->GetTopology());

	BindVertexBuffer(0, m_pRenderMesh->GetVertexBuffer(), 0, 0, m_pRenderMesh->VertexCount(), m_pMaterial->GetVertexFormat());
	BindIndexBuffer(m_pRenderMesh->GetIndexBuffer(), 0);

	m_pMaterial->DrawElements(CompressionType(m_pRenderMesh->GetVertexFormat()));

	m_pRenderMesh = NULL;
}

// ------------ End ----------------------------


// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
void CShaderAPIDX11::PerformFullScreenStencilOperation(void)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor)
{
	ALERT_NOT_IMPLEMENTED();
}


// nVidia CSAA modes, different from SupportsMSAAMode()
bool CShaderAPIDX11::SupportsCSAAMode(int nNumSamples, int nQualityLevel)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


//Notifies the shaderapi to invalidate the current set of delayed constants because we just finished a draw pass. Either actual or not.
void CShaderAPIDX11::InvalidateDelayedShaderConstants(void)
{
	ALERT_NOT_IMPLEMENTED();
}


// Gamma<->Linear conversions according to the video hardware we're running on
float CShaderAPIDX11::GammaToLinear_HardwareSpecific(float fGamma) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1.f;
}

float CShaderAPIDX11::LinearToGamma_HardwareSpecific(float fLinear) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1.f;
}


//Set's the linear->gamma conversion textures to use for this hardware for both srgb writes enabled and disabled(identity)
void CShaderAPIDX11::SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t hSRGBWriteEnabledTexture, ShaderAPITextureHandle_t hIdentityTexture)
{
	ALERT_NOT_IMPLEMENTED();
}


ImageFormat CShaderAPIDX11::GetNullTextureFormat(void)
{
	ALERT_NOT_IMPLEMENTED();
	return IMAGE_FORMAT_UNKNOWN;
}


void CShaderAPIDX11::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle)
{
	ALERT_NOT_IMPLEMENTED();
}


// Enables hardware morphing
void CShaderAPIDX11::EnableHWMorphing(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Sets flexweights for rendering
void CShaderAPIDX11::SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


void CShaderAPIDX11::FogMaxDensity(float flMaxDensity)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::CreateTextureHandles(ShaderAPITextureHandle_t* pHandles, int count)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	int i = 0;
	for (ShaderAPITextureHandle_t texHandle = 0; texHandle < m_vTextures.Count(); ++texHandle)
	{
		if (!m_vTextures[texHandle].IsActivated())
			pHandles[i++] = texHandle;
	}

	while (i < count)
		pHandles[i++] = m_vTextures.AddToTail();

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
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	if (dstImageFormat == IMAGE_FORMAT_UNKNOWN) return;

	CreateTextureHandles(pHandles, count);

	for (int i = 0; i < count; ++i)
	{
		m_vTextures[pHandles[i]].InitTexture(width, height, depth, dstImageFormat, numMipLevels, numCopies, flags);
	}
}

void CShaderAPIDX11::AcquireThreadOwnership()
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::ReleaseThreadOwnership()
{
	ALERT_NOT_IMPLEMENTED();
}


bool CShaderAPIDX11::SupportsNormalMapCompression() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Only does anything on XBox360. This is useful to eliminate stalls
void CShaderAPIDX11::EnableBuffer2FramesAhead(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// debug logging
// only implemented in some subclasses
void CShaderAPIDX11::PrintfVA(char *fmt, va_list vargs)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::Printf(PRINTF_FORMAT_STRING const char *fmt, ...)
{
	ALERT_NOT_IMPLEMENTED();
}

float CShaderAPIDX11::Knob(char *knobname, float *setvalue)
{
	ALERT_NOT_IMPLEMENTED();
	return -1.f;
}

// Allows us to override the alpha write setting of a material
void CShaderAPIDX11::OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


//extended clear buffers function with alpha independent from color
void CShaderAPIDX11::ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth)
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	ALERT_NOT_IMPLEMENTED();
}


// Allows copying a render target to another texture by specifying them both.
void CShaderAPIDX11::CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect, Rect_t *pDstRect)
{
	ALERT_NOT_IMPLEMENTED();
}


// Allows locking and unlocking of very specific surface types.
void CShaderAPIDX11::LockRect(void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderAPIDX11::UnlockRect(ShaderAPITextureHandle_t texHandle, int mipmap)
{
	ALERT_NOT_IMPLEMENTED();
}


// ------------------------------------------------------- //
//                      IDebugTextureInfo                  //
// ------------------------------------------------------- //

// Use this to turn on the mode where it builds the debug texture list.
// At the end of the next frame, GetDebugTextureList() will return a valid list of the textures.
void CShaderAPIDX11::EnableDebugTextureList(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// If this is on, then it will return all textures that exist, not just the ones that were bound in the last frame.
// It is required to enable debug texture list to get this.
void CShaderAPIDX11::EnableGetAllTextures(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
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
	ALERT_NOT_IMPLEMENTED();
	return NULL;
}


// This returns how much memory was used.
int CShaderAPIDX11::GetTextureMemoryUsed(TextureMemoryType eTextureMemory)
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}


// Use this to determine if texture debug info was computed within last numFramesAllowed frames.
bool CShaderAPIDX11::IsDebugTextureListFresh(int numFramesAllowed)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}


// Enable debug texture rendering when texture binds should not count towards textures
// used during a frame. Returns the old state of debug texture rendering flag to use
// it for restoring the mode.
bool CShaderAPIDX11::SetDebugTextureRendering(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CShaderAPIDX11::OnDeviceInitialised()
{
	AUTO_LOCK_FM(g_ShaderAPIMutex);

	int w, h;
	GetBackBufferDimensions(w, h);

	// Create the backbuffer
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	HRESULT hr = g_pShaderDeviceDX11->GetSwapChain()->GetDesc(&swapChainDesc);
	Assert(!FAILED(hr));

	ID3D11Texture2D *pBackBuffer;
	hr = g_pShaderDeviceDX11->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void **)&pBackBuffer);
	Assert(!FAILED(hr));

	m_BackBufferHandle = 0;
	CreateTextureHandles(&m_BackBufferHandle, 1);
	CAPITextureDX11* pBackBufferTex = &m_vTextures[m_BackBufferHandle];
	pBackBufferTex->InitRenderTarget(w, h, pBackBuffer, g_pShaderDevice->GetBackBufferFormat());

	m_StateChanges |= STATE_CHANGED_RENDER_TARGETS;

	m_pMatrixConstBuffer = g_pShaderDevice->CreateConstantBuffer(SHADER_BUFFER_TYPE_DYNAMIC, sizeof(MatrixCBuffers_t), "");

	// TODO: Move this to state change
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState *blendState = NULL; // Check if this is a mem leak
	hr = g_pShaderDeviceDX11->GetDevice()->CreateBlendState(&blendDesc, &blendState);
	Assert(!FAILED(hr));

	g_pShaderDeviceDX11->GetDeviceContext()->OMSetBlendState(blendState, NULL, 0xFFFFFF);
}

void CShaderAPIDX11::OnDeviceShutdown()
{
	for (int i = 0; i < m_vTextures.Count(); ++i)
	{
		m_vTextures[i].Shutdown();
	}
	m_vTextures.RemoveAll();
}