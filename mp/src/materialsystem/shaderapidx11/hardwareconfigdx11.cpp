
#include "hardwareconfigdx11.h"
#include "shaderdevicedx11.h"
#include "ishaderutil.h"
#include "dx11global.h"
#include "materialsystem/materialsystem_config.h"

// LAST INCLUDE AND WHATEVER
#include "memdbgon.h"

static CHardwareConfigDX11 s_HardwareConfigDX11;
CHardwareConfigDX11 *g_pHardwareConfigDX11 = &s_HardwareConfigDX11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHardwareConfigDX11, IMaterialSystemHardwareConfigDX11,
	MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfigDX11)

IMaterialSystemHardwareConfigDX11 *g_pHardwareConfig = g_pHardwareConfigDX11;


CHardwareConfigDX11::CHardwareConfigDX11()
{
	Q_memset(&m_HWInfo, 0, sizeof(m_HWInfo));
	m_bHDREnabled = false;
	m_HWInfo.m_nMaxDXSupportLevel = 9;
}

// Sets up HWInfo_t struct (m_HWInfo) with system capabilities
void CHardwareConfigDX11::SetupHWInfo(int nDXLevel)
{
	m_HWInfo.m_nDXSupportLevel = nDXLevel;
}

// Does the destination buffer (back buffer) have an alpha channel?
bool CHardwareConfigDX11::HasDestAlphaBuffer() const
{
	/*
	if (!g_pShaderDeviceDX11)
		return false;

	// Not exactly correct right now but it'll do for a bit
	return (g_pShaderDeviceDX11->GetBackBufferFormat() == IMAGE_FORMAT_BGRA8888);
	*/

	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Is there a stencil buffer?
bool CHardwareConfigDX11::HasStencilBuffer() const
{
	return StencilBufferBits() > 0;
}

// Get the bit depth of the framebuffer
int	 CHardwareConfigDX11::GetFrameBufferColorDepth() const
{
	/*
	if (!g_pShaderDeviceDX11)
		return 0;

	return ShaderUtil()->ImageFormatInfo(g_pShaderDeviceDX11->GetBackBufferFormat()).m_NumBytes;
	*/

	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

// Get the number of usable samplers (textures, sort of)
int  CHardwareConfigDX11::GetSamplerCount() const
{
	return m_HWInfo.m_nSamplerCount;
}

// Does the device have a gamma ramp (remapping of colours)?
bool CHardwareConfigDX11::HasSetDeviceGammaRamp() const
{
	return m_HWInfo.m_bHasSetDeviceGammaRamp;
}

// Does the device support compressed texture use? I sure hope so.
bool CHardwareConfigDX11::SupportsCompressedTextures() const
{
	return m_HWInfo.m_bSupportsCompressedTextures;
}

// Does the device support compressed vertex data? See VertexCompressionType_t.
VertexCompressionType_t CHardwareConfigDX11::SupportsCompressedVertices() const
{
	return m_HWInfo.m_VertexCompressionType;
}

// Does the device support compressed normal maps? Probably not.
bool CHardwareConfigDX11::SupportsNormalMapCompression() const
{
	return m_HWInfo.m_bSupportsNormalMapCompression;
}

// Does the device support vertex shaders and pixel shaders? If we're on dx11, probably.
bool CHardwareConfigDX11::SupportsVertexAndPixelShaders() const // DX80+
{
	if (GetDXSupportLevel() < 80)
		return false;

	return m_HWInfo.m_bSupportsVertexAndPixelShaders;
}

// Does the device support ps1.4?
bool CHardwareConfigDX11::SupportsPixelShaders_1_4() const // DX81+
{
	if (GetDXSupportLevel() < 81)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_1_4;
}

// Does the device support ps2.0?
bool CHardwareConfigDX11::SupportsPixelShaders_2_0() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_2_0;
}

// Does the device support vs2.0?
bool CHardwareConfigDX11::SupportsVertexShaders_2_0() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsVertexShaders_2_0;
}

// Does the device support ps2.0b?
bool CHardwareConfigDX11::SupportsPixelShaders_2_b() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsPixelShaders_2_b;
}

// Does the device support sm3.0?
bool CHardwareConfigDX11::SupportsShaderModel_3_0() const // DX95+
{
	if (GetDXSupportLevel() < 95)
		return false;

	return m_HWInfo.m_bSupportsShaderModel_3_0;
}

// Does the device support static flow control? if, for, switch, etc. WHAT A LUXURY.
bool CHardwareConfigDX11::SupportsStaticControlFlow() const
{
	return m_HWInfo.m_bSupportsStaticControlFlow;
}

// Returns the maximum anisotropic level allowed
int  CHardwareConfigDX11::MaximumAnisotropicLevel() const	// 0 means no anisotropic filtering
{
	return m_HWInfo.m_nMaximumAnisotropicLevel;
}

// What is the maximum allowed texture width?
int  CHardwareConfigDX11::MaxTextureWidth() const
{
	return m_HWInfo.m_nMaxTextureWidth;
}

// What is the maximum allowed texture height?
int  CHardwareConfigDX11::MaxTextureHeight() const
{
	return m_HWInfo.m_nMaxTextureHeight;
}

// Get the video memory size (maybe just for textures, but full vram seems fine)
int	 CHardwareConfigDX11::TextureMemorySize() const
{
	return m_HWInfo.m_nTextureMemorySize;
}

// I assume this means support for >1.0 or >255 channel values
bool CHardwareConfigDX11::SupportsOverbright() const
{
	return m_HWInfo.m_bSupportsOverbright;
}

// Does the device support cubemaps?
bool CHardwareConfigDX11::SupportsCubeMaps() const // DX70+
{
	return m_HWInfo.m_bSupportsCubemaps;
}

// Does the device support mipmapped cubemaps?
bool CHardwareConfigDX11::SupportsMipmappedCubemaps() const // DX70+
{
	return m_HWInfo.m_bSupportsMipmappedCubemaps;
}

// Does the device support textures with non-2^n dimensions?
bool CHardwareConfigDX11::SupportsNonPow2Textures() const
{
	return m_HWInfo.m_bSupportsNonPow2Textures;
}


// The number of texture stages represents the number of computations
// we can do in the fixed-function pipeline, it is *not* related to the
// simultaneous number of textures we can use
int  CHardwareConfigDX11::GetTextureStageCount() const
{
	// Fixed-function pipeline is barely used anymore so no real point to this
	return m_HWInfo.m_nTextureStageCount;
}

// How many vertex shader constants are allowed?
int	 CHardwareConfigDX11::NumVertexShaderConstants() const
{
	return m_HWInfo.m_nNumVertexShaderConstants;
}

// How many pixel shader constants are allowed?
int	 CHardwareConfigDX11::NumPixelShaderConstants() const
{
	return m_HWInfo.m_nNumPixelShaderConstants;
}

// How many dynamically processed lights are allowed?
int	 CHardwareConfigDX11::MaxNumLights() const
{
	return m_HWInfo.m_nMaxNumLights;
}

// Does the device support hardware lighting? YES PROBABLY
bool CHardwareConfigDX11::SupportsHardwareLighting() const // DX70+
{
	if (GetDXSupportLevel() < 70)
		return false;

	return m_HWInfo.m_bSupportsHardwareLighting;
}

// How many matrices for blending are allowed?
int	 CHardwareConfigDX11::MaxBlendMatrices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxBlendMatrices;
}

// How many blend matrix indices are allowed? WHAT DOES THIS MEAN
int	 CHardwareConfigDX11::MaxBlendMatrixIndices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxBlendMatrixIndices;
}

// What is the maximum aspect ratio for textures? 
int	 CHardwareConfigDX11::MaxTextureAspectRatio() const
{
	// Not actually sure how to find this but it's probably very large :}
	return m_HWInfo.m_nMaxTextureAspectRatio;
}

// How many matrices for blending are allowed? But vertex this time
int	 CHardwareConfigDX11::MaxVertexShaderBlendMatrices() const // 1 for < DX70
{
	if (GetDXSupportLevel() < 70)
		return 1;

	return m_HWInfo.m_nMaxVertexShaderBlendMatrices;
}

// How many clipping planes are allowed?
int	 CHardwareConfigDX11::MaxUserClipPlanes() const
{
	return m_HWInfo.m_nMaxUserClipPlanes;
}

// Should a fast clipping algorithm be used? Not sure what algorithm this is.
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

// Get the shader dll name, would be "shaderapidx11" probably
const char *CHardwareConfigDX11::GetShaderDLLName() const
{
	return NULL;

	if (!(m_HWInfo.m_pShaderDLLName && m_HWInfo.m_pShaderDLLName[0]))
		return "shaderapidx11";

	return m_HWInfo.m_pShaderDLLName;
}


// Should framebuffer be read from front buffer? No. That would be weird.
bool CHardwareConfigDX11::ReadPixelsFromFrontBuffer() const
{
	return false; // Always false
}


// Are dx dynamic textures preferred?
bool CHardwareConfigDX11::PreferDynamicTextures() const
{
	return m_HWInfo.m_bPreferDynamicTextures;
}


// Is HDR supported?
bool CHardwareConfigDX11::SupportsHDR() const
{
	return m_HWInfo.m_bSupportsHDR;
}


// Not really sure what this means to be honest
bool CHardwareConfigDX11::HasProjectedBumpEnv() const
{
	return m_HWInfo.m_bHasProjectedBumpEnv;
}

// Does the device support sphere mapping? Not relevant to base source.
bool CHardwareConfigDX11::SupportsSpheremapping() const
{
	return m_HWInfo.m_bSupportsSpheremapping;
}

// Does the device require AA (antialiasing?) clamp? Again not sure what this means.
// Could be associated with centroid hack
bool CHardwareConfigDX11::NeedsAAClamp() const
{
	return m_HWInfo.m_bNeedsAAClamp;
}

// Should centroid hack be used for antialiasing to solve sampling out of poly bounds?
bool CHardwareConfigDX11::NeedsATICentroidHack() const
{
	return m_HWInfo.m_bNeedsATICentroidHack;
}


// Does the device support color on a second (output?) stream?
bool CHardwareConfigDX11::SupportsColorOnSecondStream() const
{
	return m_HWInfo.m_bSupportsColorOnSecondStream;
}

// Does the device support both static and dynamic lighting?
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

// Does the device allow faked sRGB writes?
bool CHardwareConfigDX11::FakeSRGBWrite() const
{
	return m_HWInfo.m_bFakeSRGBWrite; // Investigate
}

// Can the device read sRGB from render targets?
bool CHardwareConfigDX11::CanDoSRGBReadFromRTs() const
{
	return m_HWInfo.m_bCanDoSRGBReadFromRTs;
}

// Does the device support mixed size targets?
bool CHardwareConfigDX11::SupportsGLMixedSizeTargets() const
{
	return m_HWInfo.m_bSupportsGLMixedSizeTargets;
}


// Is anti-aliasing enabled?
bool CHardwareConfigDX11::IsAAEnabled() const	// Is antialiasing being used?
{
	/*
	if (!g_pShaderDeviceDX11)
		return false;

	return g_pShaderDeviceDX11->IsAAEnabled();
	*/

	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}


// NOTE: Anything after this was added after shipping HL2.

// Get number of allowed vertex textures.
int CHardwareConfigDX11::GetVertexTextureCount() const
{
	return m_HWInfo.m_nVertexTextureCount;
}

// Get maximum dimension (width/height) for vertex textures
int CHardwareConfigDX11::GetMaxVertexTextureDimension() const
{
	return m_HWInfo.m_nMaxVertexTextureDimension;
}

// Max texture bit depth
int  CHardwareConfigDX11::MaxTextureDepth() const
{
	return m_HWInfo.m_nMaxTextureDepth;
}

// Astoundingly, gets the HDR type.
HDRType_t CHardwareConfigDX11::GetHDRType() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return HDR_TYPE_NONE;

	return m_HWInfo.m_HDRType;
}

// Gets the hardware's HDR type.
HDRType_t CHardwareConfigDX11::GetHardwareHDRType() const
{
	return m_HWInfo.m_HDRType;
}


// Does the device support stream offsets signifying data format differences? (usually vertex data)
bool CHardwareConfigDX11::SupportsStreamOffset() const // DX90+
{
	if (GetDXSupportLevel() < 90)
		return false;

	return m_HWInfo.m_bSupportsStreamOffset;
}

// How many bits in the stencil buffer?
int CHardwareConfigDX11::StencilBufferBits() const
{
	/*
	if (!g_pShaderDeviceDX11)
		return 0;

	return g_pShaderDeviceDX11->StencilBufferBits();
	*/

	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

// What is the maximum number of viewports?
int CHardwareConfigDX11::MaxViewports() const
{
	return m_HWInfo.m_nMaxViewports;
}


// Overrides stream offset flag
void CHardwareConfigDX11::OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport)
{
	if (!bOverrideEnabled)
	{
		m_HWInfo.m_bSupportsStreamOffset = m_HWInfo.m_bSupportsStreamOffsetActual;
		return;
	}

	m_HWInfo.m_bSupportsStreamOffset = bEnableSupport && m_HWInfo.m_bSupportsStreamOffsetActual;
}


// What shadow filtering mode is used?
int CHardwareConfigDX11::GetShadowFilterMode() const
{
	// defined in common_ps_fxc.h

	return m_HWInfo.m_nShadowFilterMode; // 0 for now
}


// Should sRGB conversion be done in shader? I think?
int CHardwareConfigDX11::NeedsShaderSRGBConversion() const
{
	return 0; // 0 for now
}


// Is sRGB blending done accurately? I think? again?
bool CHardwareConfigDX11::UsesSRGBCorrectBlending() const
{
	return m_HWInfo.m_bUsesSRGBCorrectBlending;
}


// Are fast vertex textures used? No.
bool CHardwareConfigDX11::HasFastVertexTextures() const
{
	return false; //m_HWInfo.m_bHasFastVertexTextures;
}

// Hardware morph batch count, not certain what this means.
int CHardwareConfigDX11::MaxHWMorphBatchCount() const
{
	// return ShaderUtil()->MaxHWMorphBatchCount();
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}


// Does the device actually support ps2.0b?
bool CHardwareConfigDX11::ActuallySupportsPixelShaders_2_b() const
{
	return m_HWInfo.m_bActuallySupportsPixelShaders_2_b;
}

// Does the device support the given HDR mode?
bool CHardwareConfigDX11::SupportsHDRMode(HDRType_t nHDRMode) const
{
	return nHDRMode <= m_HWInfo.m_MaxHDRMode;
}


// Is HDR enabled?
bool CHardwareConfigDX11::GetHDREnabled(void) const
{
	return m_bHDREnabled;
}

// Set HDR enabled to bEnable.
void CHardwareConfigDX11::SetHDREnabled(bool bEnable)
{
	m_bHDREnabled = bEnable && SupportsHDRMode(HDR_TYPE_INTEGER);
}


// Is border color supported?
// Border color is a solid color that is used out of the 0.0-1.0 range of texcoords.
bool CHardwareConfigDX11::SupportsBorderColor(void) const
{
	return m_HWInfo.m_bSupportsBorderColor;
}

// Is fetch4 supported?
// Fetch4 refers to a technology where 4 neighbouring texels of a 
// single channel texture are fetched into one float4.
bool CHardwareConfigDX11::SupportsFetch4(void) const
{
	return m_HWInfo.m_bSupportsFetch4;
}

