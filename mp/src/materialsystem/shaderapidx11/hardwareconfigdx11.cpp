
#include "hardwareconfigdx11.h"
#include "shaderdevicedx11.h"
#include "ishaderutil.h"
#include "shaderapidx11_global.h"
#include "materialsystem/materialsystem_config.h"

// LAST INCLUDE AND WHATEVER
#include "memdbgon.h"

static CHardwareConfigDX11 s_HardwareConfigDx11;
CHardwareConfigDX11 *g_pHardwareConfigDx11 = &s_HardwareConfigDx11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHardwareConfigDX11, IMaterialSystemHardwareConfig,
	MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfigDx11)


CHardwareConfigDX11::CHardwareConfigDX11()
{
	Q_memset(&m_HWInfo, 0, sizeof(m_HWInfo));
	m_bHDREnabled = false;
}

void CHardwareConfigDX11::SetupHWInfo(int nDXLevel)
{
	m_HWInfo.m_nDXSupportLevel = 9000000; // MEGA GRAPHICS
}

bool CHardwareConfigDX11::HasDestAlphaBuffer() const
{
	if (!g_pShaderDevice)
		return false;

	return (g_pShaderDevice->GetBackBufferFormat() == IMAGE_FORMAT_BGRA8888);
}

bool CHardwareConfigDX11::HasStencilBuffer() const
{
	return StencilBufferBits() > 0;
}

int	 CHardwareConfigDX11::GetFrameBufferColorDepth() const
{
	if (!g_pShaderDevice)
		return 0;

	return ShaderUtil()->ImageFormatInfo(g_pShaderDevice->GetBackBufferFormat()).m_NumBytes;
}

int  CHardwareConfigDX11::GetSamplerCount() const
{
	return m_HWInfo.m_nSamplerCount;
}

bool CHardwareConfigDX11::HasSetDeviceGammaRamp() const
{
	return m_HWInfo.m_bHasSetDeviceGammaRamp;
}

bool CHardwareConfigDX11::SupportsCompressedTextures() const
{
	return m_HWInfo.m_bSupportsCompressedTextures;
}

VertexCompressionType_t CHardwareConfigDX11::SupportsCompressedVertices() const
{
	return m_HWInfo.m_VertexCompressionType;
}

bool CHardwareConfigDX11::SupportsNormalMapCompression() const
{
	return m_HWInfo.m_bSupportsNormalMapCompression;
}

bool CHardwareConfigDX11::SupportsVertexAndPixelShaders() const // DX80+
{
	if (GetDXSupportLevel() < 80)
		return false;

	return m_HWInfo.m_bSupportsVertexAndPixelShaders;
}

bool CHardwareConfigDX11::SupportsPixelShaders_1_4() const // DX81+
{
	if (GetDXSupportLevel() < 81)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_1_4;
}

bool CHardwareConfigDX11::SupportsPixelShaders_2_0() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_2_0;
}

bool CHardwareConfigDX11::SupportsVertexShaders_2_0() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsVertexShaders_2_0;
}

bool CHardwareConfigDX11::SupportsPixelShaders_2_b() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_2_b;
}

bool CHardwareConfigDX11::SupportsShaderModel_3_0() const // DX95+
{
	if (GetDXSupportLevel() < 95)
		return false;

	return m_HWInfo.m_bSupportsShaderModel_3_0;
}


bool CHardwareConfigDX11::SupportsStaticControlFlow() const
{
	return m_HWInfo.m_bSupportsStaticControlFlow;
}

int  CHardwareConfigDX11::MaximumAnisotropicLevel() const	// 0 means no anisotropic filtering
{
	return m_HWInfo.m_nMaximumAnisotropicLevel;
}

int  CHardwareConfigDX11::MaxTextureWidth() const
{
	return m_HWInfo.m_nMaxTextureWidth;
}

int  CHardwareConfigDX11::MaxTextureHeight() const
{
	return m_HWInfo.m_nMaxTextureHeight;
}

int	 CHardwareConfigDX11::TextureMemorySize() const
{
	return m_HWInfo.m_nTextureMemorySize;
}

bool CHardwareConfigDX11::SupportsOverbright() const
{
	return m_HWInfo.m_bSupportsOverbright;
}

bool CHardwareConfigDX11::SupportsCubeMaps() const // DX70+
{ 
	return m_HWInfo.m_bSupportsCubemaps;
}

bool CHardwareConfigDX11::SupportsMipmappedCubemaps() const // DX70+
{
	return m_HWInfo.m_bSupportsMipmappedCubemaps;
}

bool CHardwareConfigDX11::SupportsNonPow2Textures() const
{
	return m_HWInfo.m_bSupportsNonPow2Textures;
}


// The number of texture stages represents the number of computations
// we can do in the fixed-function pipeline, it is *not* related to the
// simultaneous number of textures we can use
int  CHardwareConfigDX11::GetTextureStageCount() const
{
	return m_HWInfo.m_nTextureStageCount;
}

int	 CHardwareConfigDX11::NumVertexShaderConstants() const
{
	return m_HWInfo.m_nNumVertexShaderConstants;
}

int	 CHardwareConfigDX11::NumPixelShaderConstants() const
{
	return m_HWInfo.m_nNumPixelShaderConstants;
}

int	 CHardwareConfigDX11::MaxNumLights() const
{
	return m_HWInfo.m_nMaxNumLights;
}

bool CHardwareConfigDX11::SupportsHardwareLighting() const // DX70+
{
	if (GetDXSupportLevel() < 70)
		return false;

	return m_HWInfo.m_bSupportsHardwareLighting;
}

int	 CHardwareConfigDX11::MaxBlendMatrices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxBlendMatrices;
}

int	 CHardwareConfigDX11::MaxBlendMatrixIndices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxBlendMatrixIndices;
}

int	 CHardwareConfigDX11::MaxTextureAspectRatio() const
{
	return m_HWInfo.m_nMaxTextureAspectRatio;
}

int	 CHardwareConfigDX11::MaxVertexShaderBlendMatrices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxVertexShaderBlendMatrices;
}

int	 CHardwareConfigDX11::MaxUserClipPlanes() const
{
	return m_HWInfo.m_nMaxUserClipPlanes;
}

bool CHardwareConfigDX11::UseFastClipping() const
{
	return m_HWInfo.m_bUseFastClipping;
}


// This here should be the major item looked at when checking for compat
// from anywhere other than the material system	shaders
int CHardwareConfigDX11::GetDXSupportLevel() const // Needs to consider dxconfig specific dxlevel
{
	return m_HWInfo.m_nDXSupportLevel;
}

const char *CHardwareConfigDX11::GetShaderDLLName() const
{
	if (!(m_HWInfo.m_pShaderDLLName && m_HWInfo.m_pShaderDLLName[0]))
		return "shaderapidx11";

	return m_HWInfo.m_pShaderDLLName;
}


bool CHardwareConfigDX11::ReadPixelsFromFrontBuffer() const
{
	return false; // Always false
}


// Are dx dynamic textures preferred?
bool CHardwareConfigDX11::PreferDynamicTextures() const
{
	return m_HWInfo.m_bPreferDynamicTextures;
}


bool CHardwareConfigDX11::SupportsHDR() const
{
	return m_HWInfo.m_bSupportsHDR;
}


bool CHardwareConfigDX11::HasProjectedBumpEnv() const
{
	return m_HWInfo.m_bHasProjectedBumpEnv;
}

bool CHardwareConfigDX11::SupportsSpheremapping() const
{
	return m_HWInfo.m_bSupportsSpheremapping;
}

bool CHardwareConfigDX11::NeedsAAClamp() const
{
	return m_HWInfo.m_bNeedsAAClamp;
}

bool CHardwareConfigDX11::NeedsATICentroidHack() const
{
	return m_HWInfo.m_bNeedsATICentroidHack;
}


bool CHardwareConfigDX11::SupportsColorOnSecondStream() const
{
	return m_HWInfo.m_bSupportsColorOnSecondStream;
}

bool CHardwareConfigDX11::SupportsStaticPlusDynamicLighting() const // DX80+ iff
{
	if (GetDXSupportLevel() < 80)
		return false;

	return m_HWInfo.m_bSupportsStaticPlusDynamicLighting;
}


// Does our card have a hard time with fillrate 
// relative to other cards w/ the same dx level?
bool CHardwareConfigDX11::PreferReducedFillrate() const
{
	return m_HWInfo.m_bPreferReducedFillrate;
}


// This is the max dx support level supported by the card
int	 CHardwareConfigDX11::GetMaxDXSupportLevel() const
{
	return m_HWInfo.m_nMaxDXSupportLevel;
}


// Does the card specify fog color in linear space when sRGBWrites are enabled?
bool CHardwareConfigDX11::SpecifiesFogColorInLinearSpace() const
{
	return m_HWInfo.m_bSpecifiesFogColorInLinearSpace;
}


// Does the card support sRGB reads/writes?
bool CHardwareConfigDX11::SupportsSRGB() const
{
	return m_HWInfo.m_bSupportsSRGB;
}

bool CHardwareConfigDX11::FakeSRGBWrite() const
{
	return m_HWInfo.m_bFakeSRGBWrite; // Investigate
}

bool CHardwareConfigDX11::CanDoSRGBReadFromRTs() const
{
	return m_HWInfo.m_bCanDoSRGBReadFromRTs;
}


bool CHardwareConfigDX11::SupportsGLMixedSizeTargets() const
{
	return m_HWInfo.m_bSupportsGLMixedSizeTargets;
}


bool CHardwareConfigDX11::IsAAEnabled() const	// Is antialiasing being used?
{
	if (!g_pShaderDevice)
		return false;

	return g_pShaderDevice->IsAAEnabled();
}


// NOTE: Anything after this was added after shipping HL2.
int CHardwareConfigDX11::GetVertexTextureCount() const
{
	return m_HWInfo.m_nVertexTextureCount;
}

int CHardwareConfigDX11::GetMaxVertexTextureDimension() const
{
	return m_HWInfo.m_nMaxVertexTextureDimension;
}


int  CHardwareConfigDX11::MaxTextureDepth() const
{
	return m_HWInfo.m_nMaxTextureDepth;
}


HDRType_t CHardwareConfigDX11::GetHDRType() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return HDR_TYPE_NONE;

	return m_HWInfo.m_HDRType;
}

HDRType_t CHardwareConfigDX11::GetHardwareHDRType() const
{
	return m_HWInfo.m_HDRType;
}


bool CHardwareConfigDX11::SupportsStreamOffset() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsStreamOffset;
}


int CHardwareConfigDX11::StencilBufferBits() const
{
	if (!g_pShaderDevice)
		return 0;

	return g_pShaderDevice->StencilBufferBits();
}

int CHardwareConfigDX11::MaxViewports() const
{
	return m_HWInfo.m_nMaxViewports;
}


void CHardwareConfigDX11::OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport)
{
	if (!bOverrideEnabled)
	{
		m_HWInfo.m_bSupportsStreamOffset = m_HWInfo.m_bSupportsStreamOffsetActual;
		return;
	}

	m_HWInfo.m_bSupportsStreamOffset = bEnableSupport && m_HWInfo.m_bSupportsStreamOffsetActual;
}


int CHardwareConfigDX11::GetShadowFilterMode() const
{
	// defined in common_ps_fxc.h

	return m_HWInfo.m_nShadowFilterMode; // 0 for now
}


int CHardwareConfigDX11::NeedsShaderSRGBConversion() const
{
	return 0; // 0 for now
}


bool CHardwareConfigDX11::UsesSRGBCorrectBlending() const
{
	return m_HWInfo.m_bUsesSRGBCorrectBlending;
}


bool CHardwareConfigDX11::HasFastVertexTextures() const
{
	return false; //m_HWInfo.m_bHasFastVertexTextures;
}

int CHardwareConfigDX11::MaxHWMorphBatchCount() const
{
	return ShaderUtil()->MaxHWMorphBatchCount();
}


// Does the board actually support this?
bool CHardwareConfigDX11::ActuallySupportsPixelShaders_2_b() const
{
	return m_HWInfo.m_bActuallySupportsPixelShaders_2_b;
}


bool CHardwareConfigDX11::SupportsHDRMode(HDRType_t nHDRMode) const
{
	return nHDRMode <= m_HWInfo.m_MaxHDRMode;
}


bool CHardwareConfigDX11::GetHDREnabled(void) const
{
	return m_bHDREnabled;
}

void CHardwareConfigDX11::SetHDREnabled(bool bEnable)
{
	m_bHDREnabled = bEnable && SupportsHDRMode(HDR_TYPE_INTEGER);
}


bool CHardwareConfigDX11::SupportsBorderColor(void) const
{
	return m_HWInfo.m_bSupportsBorderColor;
}

bool CHardwareConfigDX11::SupportsFetch4(void) const
{
	return m_HWInfo.m_bSupportsFetch4;
}

