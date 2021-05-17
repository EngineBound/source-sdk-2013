
#include "dx11global.h"

#include "shadershadowdx11.h"
#include "ishaderdevicedx11.h"
#include "shaderapidx11.h"

#include "materialsystem/shader_vcs_version.h"
#include "filesystem.h"

#include "vcsreader.h"

#include "memdbgon.h"

static CShaderShadowDX11 s_ShaderShadowDX11;
CShaderShadowDX11 *g_pShaderShadowDX11 = &s_ShaderShadowDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderShadowDX11, IShaderShadowDX11,
	SHADERSHADOW_INTERFACE_VERSION, s_ShaderShadowDX11)

IShaderShadowDX11* g_pShaderShadow = g_pShaderShadowDX11;

CShaderShadowDX11::CShaderShadowDX11() : m_ShadowState()
{

}

CShaderShadowDX11::~CShaderShadowDX11()
{
	for (int i = 0; i < m_ShaderFiles.Count(); i++)
	{
		delete m_ShaderFiles[i].m_pVCSReader;
	}

	m_ShaderFiles.RemoveAll();
}

// Sets the default *shadow* state
void CShaderShadowDX11::SetDefaultState()
{
	ALERT_NOT_IMPLEMENTED();
}


// Methods related to depth buffering
void CShaderShadowDX11::DepthFunc(ShaderDepthFunc_t depthFunc)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableDepthWrites(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableDepthTest(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnablePolyOffset(PolygonOffsetMode_t nOffsetMode)
{
	ALERT_NOT_IMPLEMENTED();
}


// These methods for controlling stencil are obsolete and stubbed to do nothing.  Stencil
// control is via the shaderapi/material system now, not part of the shadow state.
// Methods related to stencil
void CShaderShadowDX11::EnableStencil(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilFunc(ShaderStencilFunc_t stencilFunc)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilPassOp(ShaderStencilOp_t stencilOp)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilFailOp(ShaderStencilOp_t stencilOp)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilDepthFailOp(ShaderStencilOp_t stencilOp)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilReference(int nReference)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilMask(int nMask)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::StencilWriteMask(int nMask)
{
	ALERT_NOT_IMPLEMENTED();
}


// Suppresses/activates color writing 
void CShaderShadowDX11::EnableColorWrites(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableAlphaWrites(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Methods related to alpha blending
void CShaderShadowDX11::EnableBlending(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	ALERT_NOT_IMPLEMENTED();
}

// More below...

// Alpha testing
void CShaderShadowDX11::EnableAlphaTest(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef /* [0-1] */)
{
	ALERT_NOT_IMPLEMENTED();
}


// Wireframe/filled polygons
void CShaderShadowDX11::PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t polyMode)
{
	ALERT_NOT_IMPLEMENTED();
}


// Back face culling
void CShaderShadowDX11::EnableCulling(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// constant color + transparency
void CShaderShadowDX11::EnableConstantColor(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Indicates the vertex format for use with a vertex shader
// The flags to pass in here come from the VertexFormatFlags_t enum
// If pTexCoordDimensions is *not* specified, we assume all coordinates
// are 2-dimensional
void CShaderShadowDX11::VertexShaderVertexFormat(unsigned int nFlags,
	int nTexCoordCount, int* pTexCoordDimensions, int nUserDataSize) 
{
	g_pShaderAPIDX11->VertexShaderVertexFormat(nFlags, nTexCoordCount, pTexCoordDimensions, nUserDataSize);
}

// Pixel and vertex shader methods
void CShaderShadowDX11::SetVertexShader(const char* pFileName, int nStaticVshIndex)
{
	ALERT_INCOMPLETE();

	m_ShadowState.m_hVertexShader = NULL;

	if (!pFileName)
		return;

	VCSRep_t lookupFile;
	lookupFile.m_Name = pFileName;
	
	VCSIndex_t VCSInd = m_ShaderFiles.Find(lookupFile);
	if (!m_ShaderFiles.IsValidIndex(VCSInd))
	{
		VCSInd = m_ShaderFiles.AddToTail(lookupFile);

		m_ShaderFiles[VCSInd].m_pVCSReader = new CVCSReader();
		m_ShaderFiles[VCSInd].m_pVCSReader->InitReader(pFileName, true);
	}

	VCSRep_t &shaderFile = m_ShaderFiles[VCSInd];

	shaderFile.m_pVCSReader->CreateShadersForStaticComboIfNeeded(nStaticVshIndex);

	m_ShadowState.m_hStaticVertexShader = shaderFile.m_pVCSReader->GetStaticShader(nStaticVshIndex);
	m_ShadowState.m_hVertexShader = shaderFile.m_pVCSReader->GetShader(m_ShadowState.m_hStaticVertexShader, 0);
}

void CShaderShadowDX11::SetPixelShader(const char* pFileName, int nStaticPshIndex)
{
	ALERT_INCOMPLETE();

	m_ShadowState.m_hPixelShader = NULL;

	if (!pFileName)
		return;

	VCSRep_t lookupFile;
	lookupFile.m_Name = pFileName;

	VCSIndex_t VCSInd = m_ShaderFiles.Find(lookupFile);
	if (!m_ShaderFiles.IsValidIndex(VCSInd))
	{
		VCSInd = m_ShaderFiles.AddToTail(lookupFile);

		m_ShaderFiles[VCSInd].m_pVCSReader = new CVCSReader();
		m_ShaderFiles[VCSInd].m_pVCSReader->InitReader(pFileName, false);
	}

	VCSRep_t &shaderFile = m_ShaderFiles[VCSInd];

	shaderFile.m_pVCSReader->CreateShadersForStaticComboIfNeeded(nStaticPshIndex);

	m_ShadowState.m_hStaticPixelShader = shaderFile.m_pVCSReader->GetStaticShader(nStaticPshIndex);
	m_ShadowState.m_hPixelShader = shaderFile.m_pVCSReader->GetShader(m_ShadowState.m_hStaticPixelShader, 0);

}


// Indicates we're going to light the model
void CShaderShadowDX11::EnableLighting(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Enables specular lighting (lighting has also got to be enabled)
void CShaderShadowDX11::EnableSpecular(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Convert from linear to gamma color space on writes to frame buffer.
void CShaderShadowDX11::EnableSRGBWrite(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Convert from gamma to linear on texture fetch.
void CShaderShadowDX11::EnableSRGBRead(Sampler_t sampler, bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Activate/deactivate skinning. Indexed blending is automatically
// enabled if it's available for this hardware. When blending is enabled,
// we allocate enough room for 3 weights (max allowed)
void CShaderShadowDX11::EnableVertexBlend(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// per texture unit stuff
void CShaderShadowDX11::OverbrightValue(TextureStage_t stage, float value)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableTexture(Sampler_t sampler, bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableTexGen(TextureStage_t stage, bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::TexGen(TextureStage_t stage, ShaderTexGenParam_t param)
{
	ALERT_NOT_IMPLEMENTED();
}


// alternate method of specifying per-texture unit stuff, more flexible and more complicated
// Can be used to specify different operation per channel (alpha/color)...
void CShaderShadowDX11::EnableCustomPixelPipe(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::CustomTextureStages(int stageCount)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
	ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2) 
{
	ALERT_NOT_IMPLEMENTED();
}

// indicates what per-vertex data we're providing
void CShaderShadowDX11::DrawFlags(unsigned int drawFlags)
{
	ALERT_NOT_IMPLEMENTED();
}


// A simpler method of dealing with alpha modulation
void CShaderShadowDX11::EnableAlphaPipe(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableConstantAlpha(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableVertexAlpha(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::EnableTextureAlpha(TextureStage_t stage, bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// GR - Separate alpha blending
void CShaderShadowDX11::EnableBlendingSeparateAlpha(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::FogMode(ShaderFogMode_t fogMode)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderShadowDX11::SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource)
{
	ALERT_NOT_IMPLEMENTED();
}


// Indicates the morph format for use with a vertex shader
// The flags to pass in here come from the MorphFormatFlags_t enum
void CShaderShadowDX11::SetMorphFormat(MorphFormat_t flags)
{
	ALERT_NOT_IMPLEMENTED();
}


void CShaderShadowDX11::DisableFogGammaCorrection(bool bDisable)
{
	ALERT_NOT_IMPLEMENTED();
}
 //some blending modes won't work properly with corrected fog

// Alpha to coverage
void CShaderShadowDX11::EnableAlphaToCoverage(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}


// Shadow map filtering
void CShaderShadowDX11::SetShadowDepthFiltering(Sampler_t stage)
{
	ALERT_NOT_IMPLEMENTED();
}


// More alpha blending state
void CShaderShadowDX11::BlendOp(ShaderBlendOp_t blendOp)
{
	ALERT_NOT_IMPLEMENTED();
}

void CShaderShadowDX11::BlendOpSeparateAlpha(ShaderBlendOp_t blendOp)
{
	ALERT_NOT_IMPLEMENTED();
}
