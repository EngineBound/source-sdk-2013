#ifndef HARDWARECONFIGDX11_H
#define HARDWARECONFIGDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/imaterialsystem.h"

struct HWInfo_t : public MaterialAdapterInfo_t
{
	int m_nSamplerCount;
	bool m_bHasSetDeviceGammaRamp;
	bool m_bSupportsCompressedTextures;
	VertexCompressionType_t m_VertexCompressionType;
	bool m_bSupportsNormalMapCompression;

	bool m_bSupportsVertexAndPixelShaders;
	bool m_bSupportsPixelShaders_1_4;
	bool m_bSupportsPixelShaders_2_0;
	bool m_bSupportsPixelShaders_2_b;
	bool m_bSupportsShaderModel_3_0;
	bool m_bSupportsVertexShaders_2_0;

	bool m_bSupportsStaticControlFlow;
	int m_nMaximumAnisotropicLevel;
	int m_nMaxTextureWidth;
	int m_nMaxTextureHeight;
	int m_nTextureMemorySize;
	bool m_bSupportsOverbright;
	bool m_bSupportsCubemaps;
	bool m_bSupportsMipmappedCubemaps;
	bool m_bSupportsNonPow2Textures;

	int m_nTextureStageCount;
	int m_nNumVertexShaderConstants;
	int m_nNumPixelShaderConstants;
	int m_nMaxNumLights;
	bool m_bSupportsHardwareLighting;
	int m_nMaxBlendMatrices;
	int m_nMaxBlendMatrixIndices;
	int m_nMaxTextureAspectRatio;
	int m_nMaxVertexShaderBlendMatrices;
	int m_nMaxUserClipPlanes;
	bool m_bUseFastClipping;

	//int m_nDXSupportLevel;
	char m_pShaderDLLName[32];

	bool m_bPreferDynamicTextures;

	bool m_bSupportsHDR;

	bool m_bHasProjectedBumpEnv;
	bool m_bSupportsSpheremapping;
	bool m_bNeedsAAClamp;
	bool m_bNeedsATICentroidHack;

	bool m_bSupportsColorOnSecondStream;
	bool m_bSupportsStaticPlusDynamicLighting;

	bool m_bPreferReducedFillrate;

	//int	 m_nMaxDXSupportLevel;

	bool m_bSpecifiesFogColorInLinearSpace;

	bool m_bSupportsSRGB;
	bool m_bFakeSRGBWrite;
	bool m_bCanDoSRGBReadFromRTs;

	bool m_bSupportsGLMixedSizeTargets;

	int m_nVertexTextureCount;
	int m_nMaxVertexTextureDimension;

	int m_nMaxTextureDepth;

	HDRType_t m_HDRType;

	bool m_bSupportsStreamOffset;
	bool m_bSupportsStreamOffsetActual;

	int m_nMaxViewports;

	int m_nShadowFilterMode;

	bool m_bNeedsShaderSRGBConversion;

	bool m_bUsesSRGBCorrectBlending;

	bool m_bHasFastVertexTextures;

	bool m_bActuallySupportsPixelShaders_2_b;

	HDRType_t m_MaxHDRMode;

	bool m_bSupportsBorderColor;
	bool m_bSupportsFetch4;

	float m_flMinGammaControlPoint;
	float m_flMaxGammaControlPoint;
	float m_nGammaControlPointCount;
};

class IMaterialSystemHardwareConfigDX11 : public IMaterialSystemHardwareConfig
{
public:
	// Required for the system to run
	virtual const char *GetHWSpecificShaderDLLName() const = 0;
};

class CHardwareConfigDX11 : public IMaterialSystemHardwareConfigDX11
{
public:
	CHardwareConfigDX11();

	virtual bool HasDestAlphaBuffer() const;
	virtual bool HasStencilBuffer() const;
	virtual int	 GetFrameBufferColorDepth() const;
	virtual int  GetSamplerCount() const;
	virtual bool HasSetDeviceGammaRamp() const;
	virtual bool SupportsCompressedTextures() const;
	virtual VertexCompressionType_t SupportsCompressedVertices() const;
	virtual bool SupportsNormalMapCompression() const;

	const char *GetHWSpecificShaderDLLName() const {
		return "stdshader_dx9";
	}

	virtual bool SupportsVertexAndPixelShaders() const;
	virtual bool SupportsPixelShaders_1_4() const;
	virtual bool SupportsPixelShaders_2_0() const;
	virtual bool SupportsPixelShaders_2_b() const;
	virtual bool SupportsShaderModel_3_0() const;
	virtual bool SupportsVertexShaders_2_0() const;

	virtual bool SupportsStaticControlFlow() const;
	virtual int  MaximumAnisotropicLevel() const;	// 0 means no anisotropic filtering
	virtual int  MaxTextureWidth() const;
	virtual int  MaxTextureHeight() const;
	virtual int	 TextureMemorySize() const;
	virtual bool SupportsOverbright() const;
	virtual bool SupportsCubeMaps() const;
	virtual bool SupportsMipmappedCubemaps() const;
	virtual bool SupportsNonPow2Textures() const;

	// The number of texture stages represents the number of computations
	// we can do in the fixed-function pipeline, it is *not* related to the
	// simultaneous number of textures we can use
	virtual int  GetTextureStageCount() const;
	virtual int	 NumVertexShaderConstants() const;
	virtual int	 NumPixelShaderConstants() const;
	virtual int	 MaxNumLights() const;
	virtual bool SupportsHardwareLighting() const;
	virtual int	 MaxBlendMatrices() const;
	virtual int	 MaxBlendMatrixIndices() const;
	virtual int	 MaxTextureAspectRatio() const;
	virtual int	 MaxVertexShaderBlendMatrices() const;
	virtual int	 MaxUserClipPlanes() const;
	virtual bool UseFastClipping() const;

	// This here should be the major item looked at when checking for compat
	// from anywhere other than the material system	shaders
	virtual int GetDXSupportLevel() const;
	virtual const char *GetShaderDLLName() const;

	virtual bool ReadPixelsFromFrontBuffer() const;

	// Are dx dynamic textures preferred?
	virtual bool PreferDynamicTextures() const;

	virtual bool SupportsHDR() const;

	virtual bool HasProjectedBumpEnv() const;
	virtual bool SupportsSpheremapping() const;
	virtual bool NeedsAAClamp() const;
	virtual bool NeedsATICentroidHack() const;

	virtual bool SupportsColorOnSecondStream() const;
	virtual bool SupportsStaticPlusDynamicLighting() const;

	// Does our card have a hard time with fillrate 
	// relative to other cards w/ the same dx level?
	virtual bool PreferReducedFillrate() const;

	// This is the max dx support level supported by the card
	virtual int	 GetMaxDXSupportLevel() const;

	// Does the card specify fog color in linear space when sRGBWrites are enabled?
	virtual bool SpecifiesFogColorInLinearSpace() const;

	// Does the card support sRGB reads/writes?
	virtual bool SupportsSRGB() const;
	virtual bool FakeSRGBWrite() const;
	virtual bool CanDoSRGBReadFromRTs() const;

	virtual bool SupportsGLMixedSizeTargets() const;

	virtual bool IsAAEnabled() const;	// Is antialiasing being used?

	// NOTE: Anything after this was added after shipping HL2.
	virtual int GetVertexTextureCount() const;
	virtual int GetMaxVertexTextureDimension() const;

	virtual int  MaxTextureDepth() const;

	virtual HDRType_t GetHDRType() const;
	virtual HDRType_t GetHardwareHDRType() const;

	virtual bool SupportsStreamOffset() const;

	virtual int StencilBufferBits() const;
	virtual int MaxViewports() const;

	virtual void OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport);

	virtual int GetShadowFilterMode() const;

	virtual int NeedsShaderSRGBConversion() const;

	virtual bool UsesSRGBCorrectBlending() const;

	virtual bool HasFastVertexTextures() const;
	virtual int MaxHWMorphBatchCount() const;

	// Does the board actually support this?
	virtual bool ActuallySupportsPixelShaders_2_b() const;

	virtual bool SupportsHDRMode(HDRType_t nHDRMode) const;

	virtual bool GetHDREnabled(void) const;
	virtual void SetHDREnabled(bool bEnable);

	virtual bool SupportsBorderColor(void) const;
	virtual bool SupportsFetch4(void) const;

	inline bool ShouldAlwaysUseShaderModel2bShaders() const { return IsOpenGL(); }
	inline bool PlatformRequiresNonNullPixelShaders() const { return IsOpenGL(); }

	HWInfo_t& GetInfo() { return m_HWInfo; }

private:

	void SetupHWInfo(int nDXLevel);

	HWInfo_t m_HWInfo;

	bool m_bHDREnabled;
};


#endif // HARDWARECONFIGDX11_H