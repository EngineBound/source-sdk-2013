
#include "shadershadowdx11.h"

static CShaderShadowDX11 s_ShaderShadowDX11;
CShaderShadowDX11 *g_pShaderShadowDX11 = &s_ShaderShadowDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderShadowDX11, IShaderShadowDX11,
	SHADERSHADOW_INTERFACE_VERSION, s_ShaderShadowDX11)

// Sets the default *shadow* state
void CShaderShadowDX11::SetDefaultState()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Methods related to depth buffering
void CShaderShadowDX11::DepthFunc(ShaderDepthFunc_t depthFunc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableDepthWrites(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableDepthTest(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnablePolyOffset(PolygonOffsetMode_t nOffsetMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// These methods for controlling stencil are obsolete and stubbed to do nothing.  Stencil
// control is via the shaderapi/material system now, not part of the shadow state.
// Methods related to stencil
void CShaderShadowDX11::EnableStencil(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilFunc(ShaderStencilFunc_t stencilFunc)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilPassOp(ShaderStencilOp_t stencilOp)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilFailOp(ShaderStencilOp_t stencilOp)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilDepthFailOp(ShaderStencilOp_t stencilOp)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilReference(int nReference)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilMask(int nMask)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::StencilWriteMask(int nMask)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Suppresses/activates color writing 
void CShaderShadowDX11::EnableColorWrites(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableAlphaWrites(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Methods related to alpha blending
void CShaderShadowDX11::EnableBlending(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// More below...

// Alpha testing
void CShaderShadowDX11::EnableAlphaTest(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef /* [0-1] */)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Wireframe/filled polygons
void CShaderShadowDX11::PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t polyMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Back face culling
void CShaderShadowDX11::EnableCulling(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// constant color + transparency
void CShaderShadowDX11::EnableConstantColor(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Indicates the vertex format for use with a vertex shader
// The flags to pass in here come from the VertexFormatFlags_t enum
// If pTexCoordDimensions is *not* specified, we assume all coordinates
// are 2-dimensional
void CShaderShadowDX11::VertexShaderVertexFormat(unsigned int nFlags,
	int nTexCoordCount, int* pTexCoordDimensions, int nUserDataSize) 
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Pixel and vertex shader methods
void CShaderShadowDX11::SetVertexShader(const char* pFileName, int nStaticVshIndex)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::SetPixelShader(const char* pFileName, int nStaticPshIndex)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Indicates we're going to light the model
void CShaderShadowDX11::EnableLighting(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Enables specular lighting (lighting has also got to be enabled)
void CShaderShadowDX11::EnableSpecular(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Convert from linear to gamma color space on writes to frame buffer.
void CShaderShadowDX11::EnableSRGBWrite(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Convert from gamma to linear on texture fetch.
void CShaderShadowDX11::EnableSRGBRead(Sampler_t sampler, bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Activate/deactivate skinning. Indexed blending is automatically
// enabled if it's available for this hardware. When blending is enabled,
// we allocate enough room for 3 weights (max allowed)
void CShaderShadowDX11::EnableVertexBlend(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// per texture unit stuff
void CShaderShadowDX11::OverbrightValue(TextureStage_t stage, float value)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableTexture(Sampler_t sampler, bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableTexGen(TextureStage_t stage, bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::TexGen(TextureStage_t stage, ShaderTexGenParam_t param)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// alternate method of specifying per-texture unit stuff, more flexible and more complicated
// Can be used to specify different operation per channel (alpha/color)...
void CShaderShadowDX11::EnableCustomPixelPipe(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::CustomTextureStages(int stageCount)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
	ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2) 
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// indicates what per-vertex data we're providing
void CShaderShadowDX11::DrawFlags(unsigned int drawFlags)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// A simpler method of dealing with alpha modulation
void CShaderShadowDX11::EnableAlphaPipe(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableConstantAlpha(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableVertexAlpha(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::EnableTextureAlpha(TextureStage_t stage, bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// GR - Separate alpha blending
void CShaderShadowDX11::EnableBlendingSeparateAlpha(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::FogMode(ShaderFogMode_t fogMode)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderShadowDX11::SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Indicates the morph format for use with a vertex shader
// The flags to pass in here come from the MorphFormatFlags_t enum
void CShaderShadowDX11::SetMorphFormat(MorphFormat_t flags)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


void CShaderShadowDX11::DisableFogGammaCorrection(bool bDisable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}
 //some blending modes won't work properly with corrected fog

// Alpha to coverage
void CShaderShadowDX11::EnableAlphaToCoverage(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// Shadow map filtering
void CShaderShadowDX11::SetShadowDepthFiltering(Sampler_t stage)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}


// More alpha blending state
void CShaderShadowDX11::BlendOp(ShaderBlendOp_t blendOp)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CShaderShadowDX11::BlendOpSeparateAlpha(ShaderBlendOp_t blendOp)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}
