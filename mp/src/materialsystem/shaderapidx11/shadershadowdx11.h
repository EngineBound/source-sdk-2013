#ifndef SHADERSHADOWDX11_H
#define SHADERSHADOWDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishadershadow.h"

class CShaderShadowDX11 : public IShaderShadow
{
public:
	CShaderShadowDX11();
	~CShaderShadowDX11();

	// Sets the default *shadow* state
	virtual void SetDefaultState();

	// Methods related to depth buffering
	virtual void DepthFunc(ShaderDepthFunc_t depthFunc);
	virtual void EnableDepthWrites(bool bEnable);
	virtual void EnableDepthTest(bool bEnable);
	virtual void EnablePolyOffset(PolygonOffsetMode_t nOffsetMode);

	// These methods for controlling stencil are obsolete and stubbed to do nothing.  Stencil
	// control is via the shaderapi/material system now, not part of the shadow state.
	// Methods related to stencil
	virtual void EnableStencil(bool bEnable);
	virtual void StencilFunc(ShaderStencilFunc_t stencilFunc);
	virtual void StencilPassOp(ShaderStencilOp_t stencilOp);
	virtual void StencilFailOp(ShaderStencilOp_t stencilOp);
	virtual void StencilDepthFailOp(ShaderStencilOp_t stencilOp);
	virtual void StencilReference(int nReference);
	virtual void StencilMask(int nMask);
	virtual void StencilWriteMask(int nMask);

	// Suppresses/activates color writing 
	virtual void EnableColorWrites(bool bEnable);
	virtual void EnableAlphaWrites(bool bEnable);

	// Methods related to alpha blending
	virtual void EnableBlending(bool bEnable);
	virtual void BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor);
	// More below...

	// Alpha testing
	virtual void EnableAlphaTest(bool bEnable);
	virtual void AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef /* [0-1] */);

	// Wireframe/filled polygons
	virtual void PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t polyMode);

	// Back face culling
	virtual void EnableCulling(bool bEnable);

	// constant color + transparency
	virtual void EnableConstantColor(bool bEnable);

	// Indicates the vertex format for use with a vertex shader
	// The flags to pass in here come from the VertexFormatFlags_t enum
	// If pTexCoordDimensions is *not* specified, we assume all coordinates
	// are 2-dimensional
	virtual void VertexShaderVertexFormat(unsigned int nFlags,
		int nTexCoordCount, int* pTexCoordDimensions, int nUserDataSize);

	// Pixel and vertex shader methods
	virtual void SetVertexShader(const char* pFileName, int nStaticVshIndex);
	virtual	void SetPixelShader(const char* pFileName, int nStaticPshIndex);

	// Indicates we're going to light the model
	virtual void EnableLighting(bool bEnable);

	// Enables specular lighting (lighting has also got to be enabled)
	virtual void EnableSpecular(bool bEnable);

	// Convert from linear to gamma color space on writes to frame buffer.
	virtual void EnableSRGBWrite(bool bEnable);

	// Convert from gamma to linear on texture fetch.
	virtual void EnableSRGBRead(Sampler_t sampler, bool bEnable);

	// Activate/deactivate skinning. Indexed blending is automatically
	// enabled if it's available for this hardware. When blending is enabled,
	// we allocate enough room for 3 weights (max allowed)
	virtual void EnableVertexBlend(bool bEnable);

	// per texture unit stuff
	virtual void OverbrightValue(TextureStage_t stage, float value);
	virtual void EnableTexture(Sampler_t sampler, bool bEnable);
	virtual void EnableTexGen(TextureStage_t stage, bool bEnable);
	virtual void TexGen(TextureStage_t stage, ShaderTexGenParam_t param);

	// alternate method of specifying per-texture unit stuff, more flexible and more complicated
	// Can be used to specify different operation per channel (alpha/color)...
	virtual void EnableCustomPixelPipe(bool bEnable);
	virtual void CustomTextureStages(int stageCount);
	virtual void CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
		ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2);

	// indicates what per-vertex data we're providing
	virtual void DrawFlags(unsigned int drawFlags);

	// A simpler method of dealing with alpha modulation
	virtual void EnableAlphaPipe(bool bEnable);
	virtual void EnableConstantAlpha(bool bEnable);
	virtual void EnableVertexAlpha(bool bEnable);
	virtual void EnableTextureAlpha(TextureStage_t stage, bool bEnable);

	// GR - Separate alpha blending
	virtual void EnableBlendingSeparateAlpha(bool bEnable);
	virtual void BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor);
	virtual void FogMode(ShaderFogMode_t fogMode);

	virtual void SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource);

	// Indicates the morph format for use with a vertex shader
	// The flags to pass in here come from the MorphFormatFlags_t enum
	virtual void SetMorphFormat(MorphFormat_t flags);

	virtual void DisableFogGammaCorrection(bool bDisable); //some blending modes won't work properly with corrected fog

	// Alpha to coverage
	virtual void EnableAlphaToCoverage(bool bEnable);

	// Shadow map filtering
	virtual void SetShadowDepthFiltering(Sampler_t stage);

	// More alpha blending state
	virtual void BlendOp(ShaderBlendOp_t blendOp);
	virtual void BlendOpSeparateAlpha(ShaderBlendOp_t blendOp);
	
	bool m_bIsTranslucent;
	bool m_bIsAlphaTested;
	bool m_bIsDepthWriteEnabled;
	bool m_bUsesVertexAndPixelShaders;
};

#endif //SHADERSHADOWDX11_H