
#include "shadershadowdx11.h"

static CShaderShadowDX11 s_ShaderShadowDX11;
CShaderShadowDX11 *g_pShaderShadowDX11 = &s_ShaderShadowDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderShadowDX11, IShaderShadow,
	SHADERSHADOW_INTERFACE_VERSION, s_ShaderShadowDX11)

CShaderShadowDX11::CShaderShadowDX11() 
{
	SetDefaultState();
}

CShaderShadowDX11::~CShaderShadowDX11()
{
}

// Sets the default *shadow* state
void CShaderShadowDX11::SetDefaultState()
{
	m_bIsTranslucent = false;
	m_bIsAlphaTested = false;
	m_bIsDepthWriteEnabled = true;
	m_bUsesVertexAndPixelShaders = false;
}

// Methods related to depth buffering
void CShaderShadowDX11::DepthFunc(ShaderDepthFunc_t depthFunc)
{
	return;
}

void CShaderShadowDX11::EnableDepthWrites(bool bEnable)
{
	m_bIsDepthWriteEnabled = bEnable;
}

void CShaderShadowDX11::EnableDepthTest(bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnablePolyOffset(PolygonOffsetMode_t nOffsetMode)
{
	return;
}


// These methods for controlling stencil are obsolete and stubbed to do nothing.  Stencil
// control is via the shaderapi/material system now, not part of the shadow state.
// Methods related to stencil
void CShaderShadowDX11::EnableStencil(bool bEnable)
{
	return;
}

void CShaderShadowDX11::StencilFunc(ShaderStencilFunc_t stencilFunc)
{
	return;
}

void CShaderShadowDX11::StencilPassOp(ShaderStencilOp_t stencilOp)
{
	return;
}

void CShaderShadowDX11::StencilFailOp(ShaderStencilOp_t stencilOp)
{
	return;
}

void CShaderShadowDX11::StencilDepthFailOp(ShaderStencilOp_t stencilOp)
{
	return;
}

void CShaderShadowDX11::StencilReference(int nReference)
{
	return;
}

void CShaderShadowDX11::StencilMask(int nMask)
{
	return;
}

void CShaderShadowDX11::StencilWriteMask(int nMask)
{
	return;
}


// Suppresses/activates color writing 
void CShaderShadowDX11::EnableColorWrites(bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnableAlphaWrites(bool bEnable)
{
	return;
}


// Methods related to alpha blending
void CShaderShadowDX11::EnableBlending(bool bEnable)
{
	m_bIsTranslucent = bEnable;
}

void CShaderShadowDX11::BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	return;
}

// More below...

// Alpha testing
void CShaderShadowDX11::EnableAlphaTest(bool bEnable)
{
	m_bIsAlphaTested = bEnable;
}

void CShaderShadowDX11::AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef /* [0-1] */)
{
	return;
}


// Wireframe/filled polygons
void CShaderShadowDX11::PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t polyMode)
{
	return;
}


// Back face culling
void CShaderShadowDX11::EnableCulling(bool bEnable)
{
	return;
}


// constant color + transparency
void CShaderShadowDX11::EnableConstantColor(bool bEnable)
{
	return;
}


// Indicates the vertex format for use with a vertex shader
// The flags to pass in here come from the VertexFormatFlags_t enum
// If pTexCoordDimensions is *not* specified, we assume all coordinates
// are 2-dimensional
void CShaderShadowDX11::VertexShaderVertexFormat(unsigned int nFlags,
	int nTexCoordCount, int* pTexCoordDimensions, int nUserDataSize)
{
	return;
}


// Pixel and vertex shader methods
void CShaderShadowDX11::SetVertexShader(const char* pFileName, int nStaticVshIndex)
{
	m_bUsesVertexAndPixelShaders = (pFileName != NULL);
}

void CShaderShadowDX11::SetPixelShader(const char* pFileName, int nStaticPshIndex)
{
	m_bUsesVertexAndPixelShaders = (pFileName != NULL);
}


// Indicates we're going to light the model
void CShaderShadowDX11::EnableLighting(bool bEnable)
{
	return;
}


// Enables specular lighting (lighting has also got to be enabled)
void CShaderShadowDX11::EnableSpecular(bool bEnable)
{
	return;
}


// Convert from linear to gamma color space on writes to frame buffer.
void CShaderShadowDX11::EnableSRGBWrite(bool bEnable)
{
	return;
}


// Convert from gamma to linear on texture fetch.
void CShaderShadowDX11::EnableSRGBRead(Sampler_t sampler, bool bEnable)
{
	return;
}


// Activate/deactivate skinning. Indexed blending is automatically
// enabled if it's available for this hardware. When blending is enabled,
// we allocate enough room for 3 weights (max allowed)
void CShaderShadowDX11::EnableVertexBlend(bool bEnable)
{
	return;
}


// per texture unit stuff
void CShaderShadowDX11::OverbrightValue(TextureStage_t stage, float value)
{
	return;
}

void CShaderShadowDX11::EnableTexture(Sampler_t sampler, bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnableTexGen(TextureStage_t stage, bool bEnable)
{
	return;
}

void CShaderShadowDX11::TexGen(TextureStage_t stage, ShaderTexGenParam_t param)
{
	return;
}


// alternate method of specifying per-texture unit stuff, more flexible and more complicated
// Can be used to specify different operation per channel (alpha/color)...
void CShaderShadowDX11::EnableCustomPixelPipe(bool bEnable)
{
	return;
}

void CShaderShadowDX11::CustomTextureStages(int stageCount)
{
	return;
}

void CShaderShadowDX11::CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
	ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2)
{
	return;
}


// indicates what per-vertex data we're providing
void CShaderShadowDX11::DrawFlags(unsigned int drawFlags)
{

}


// A simpler method of dealing with alpha modulation
void CShaderShadowDX11::EnableAlphaPipe(bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnableConstantAlpha(bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnableVertexAlpha(bool bEnable)
{
	return;
}

void CShaderShadowDX11::EnableTextureAlpha(TextureStage_t stage, bool bEnable)
{
	return;
}


// GR - Separate alpha blending
void CShaderShadowDX11::EnableBlendingSeparateAlpha(bool bEnable)
{
	return;
}

void CShaderShadowDX11::BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	return;
}

void CShaderShadowDX11::FogMode(ShaderFogMode_t fogMode)
{
	return;
}


void CShaderShadowDX11::SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource)
{
	return;
}


// Indicates the morph format for use with a vertex shader
// The flags to pass in here come from the MorphFormatFlags_t enum
void CShaderShadowDX11::SetMorphFormat(MorphFormat_t flags)
{
	return;
}


void CShaderShadowDX11::DisableFogGammaCorrection(bool bDisable) //some blending modes won't work properly with corrected fog
{
	return;
}


// Alpha to coverage
void CShaderShadowDX11::EnableAlphaToCoverage(bool bEnable)
{
	return;
}


// Shadow map filtering
void CShaderShadowDX11::SetShadowDepthFiltering(Sampler_t stage)
{
	return;
}


// More alpha blending state
void CShaderShadowDX11::BlendOp(ShaderBlendOp_t blendOp)
{
	return;
}

void CShaderShadowDX11::BlendOpSeparateAlpha(ShaderBlendOp_t blendOp)
{
	return;
}
