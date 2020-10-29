
#include "hardwareconfigdx11.h"

static CHardwareConfigDX11 s_HardwareConfig;
CHardwareConfigDX11 *g_pHardwareConfig = &s_HardwareConfig;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHardwareConfigDX11, IMaterialSystemHardwareConfig,
	MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, s_HardwareConfig)

bool CHardwareConfigDX11::HasDestAlphaBuffer() const
{
	return false;
}

bool CHardwareConfigDX11::HasStencilBuffer() const
{
	return false;
}

int	 CHardwareConfigDX11::GetFrameBufferColorDepth() const
{
	return 0;
}

int  CHardwareConfigDX11::GetSamplerCount() const
{
	return 0;
}

bool CHardwareConfigDX11::HasSetDeviceGammaRamp() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsCompressedTextures() const
{
	return false;
}

VertexCompressionType_t CHardwareConfigDX11::SupportsCompressedVertices() const
{
	return VERTEX_COMPRESSION_INVALID;
}

bool CHardwareConfigDX11::SupportsNormalMapCompression() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsVertexAndPixelShaders() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsPixelShaders_1_4() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsStaticControlFlow() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsPixelShaders_2_0() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsVertexShaders_2_0() const
{
	return false;
}

int  CHardwareConfigDX11::MaximumAnisotropicLevel() const	// 0 means no anisotropic filtering
{
	return 0;
}

int  CHardwareConfigDX11::MaxTextureWidth() const
{
	return 0;
}

int  CHardwareConfigDX11::MaxTextureHeight() const
{
	return 0;
}

int	 CHardwareConfigDX11::TextureMemorySize() const
{
	return 0;
}

bool CHardwareConfigDX11::SupportsOverbright() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsCubeMaps() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsMipmappedCubemaps() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsNonPow2Textures() const
{
	return false;
}


// The number of texture stages represents the number of computations
// we can do in the fixed-function pipeline, it is *not* related to the
// simultaneous number of textures we can use
int  CHardwareConfigDX11::GetTextureStageCount() const
{
	return 0;
}

int	 CHardwareConfigDX11::NumVertexShaderConstants() const
{
	return 0;
}

int	 CHardwareConfigDX11::NumPixelShaderConstants() const
{
	return 0;
}

int	 CHardwareConfigDX11::MaxNumLights() const
{
	return 0;
}

bool CHardwareConfigDX11::SupportsHardwareLighting() const
{
	return false;
}

int	 CHardwareConfigDX11::MaxBlendMatrices() const
{
	return 0;
}

int	 CHardwareConfigDX11::MaxBlendMatrixIndices() const
{
	return 0;
}

int	 CHardwareConfigDX11::MaxTextureAspectRatio() const
{
	return 0;
}

int	 CHardwareConfigDX11::MaxVertexShaderBlendMatrices() const
{
	return 0;
}

int	 CHardwareConfigDX11::MaxUserClipPlanes() const
{
	return 0;
}

bool CHardwareConfigDX11::UseFastClipping() const
{
	return false;
}


// This here should be the major item looked at when checking for compat
// from anywhere other than the material system	shaders
int CHardwareConfigDX11::GetDXSupportLevel() const
{
	return 900000000; // MEGA GRAPHICS MODE
}

const char *CHardwareConfigDX11::GetShaderDLLName() const
{
	return "shaderapidx11";
}


bool CHardwareConfigDX11::ReadPixelsFromFrontBuffer() const
{
	return false;
}


// Are dx dynamic textures preferred?
bool CHardwareConfigDX11::PreferDynamicTextures() const
{
	return false;
}


bool CHardwareConfigDX11::SupportsHDR() const
{
	return false;
}


bool CHardwareConfigDX11::HasProjectedBumpEnv() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsSpheremapping() const
{
	return false;
}

bool CHardwareConfigDX11::NeedsAAClamp() const
{
	return false;
}

bool CHardwareConfigDX11::NeedsATICentroidHack() const
{
	return false;
}


bool CHardwareConfigDX11::SupportsColorOnSecondStream() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsStaticPlusDynamicLighting() const
{
	return false;
}


// Does our card have a hard time with fillrate 
// relative to other cards w/ the same dx level?
bool CHardwareConfigDX11::PreferReducedFillrate() const
{
	return false;
}


// This is the max dx support level supported by the card
int	 CHardwareConfigDX11::GetMaxDXSupportLevel() const
{
	return 0;
}


// Does the card specify fog color in linear space when sRGBWrites are enabled?
bool CHardwareConfigDX11::SpecifiesFogColorInLinearSpace() const
{
	return false;
}


// Does the card support sRGB reads/writes?
bool CHardwareConfigDX11::SupportsSRGB() const
{
	return false;
}

bool CHardwareConfigDX11::FakeSRGBWrite() const
{
	return false;
}

bool CHardwareConfigDX11::CanDoSRGBReadFromRTs() const
{
	return false;
}


bool CHardwareConfigDX11::SupportsGLMixedSizeTargets() const
{
	return false;
}


bool CHardwareConfigDX11::IsAAEnabled() const	// Is antialiasing being used?
{
	return false;
}


// NOTE: Anything after this was added after shipping HL2.
int CHardwareConfigDX11::GetVertexTextureCount() const
{
	return 0;
}

int CHardwareConfigDX11::GetMaxVertexTextureDimension() const
{
	return 0;
}


int  CHardwareConfigDX11::MaxTextureDepth() const
{
	return 0;
}


HDRType_t CHardwareConfigDX11::GetHDRType() const
{
	return HDR_TYPE_NONE;
}

HDRType_t CHardwareConfigDX11::GetHardwareHDRType() const
{
	return HDR_TYPE_NONE;
}


bool CHardwareConfigDX11::SupportsPixelShaders_2_b() const
{
	return false;
}

bool CHardwareConfigDX11::SupportsStreamOffset() const
{
	return false;
}


int CHardwareConfigDX11::StencilBufferBits() const
{
	return 0;
}

int CHardwareConfigDX11::MaxViewports() const
{
	return 0;
}


void CHardwareConfigDX11::OverrideStreamOffsetSupport(bool bOverrideEnabled, bool bEnableSupport)
{
	return;
}


int CHardwareConfigDX11::GetShadowFilterMode() const
{
	return 0;
}


int CHardwareConfigDX11::NeedsShaderSRGBConversion() const
{
	return 0;
}


bool CHardwareConfigDX11::UsesSRGBCorrectBlending() const
{
	return false;
}


bool CHardwareConfigDX11::SupportsShaderModel_3_0() const
{
	return false;
}

bool CHardwareConfigDX11::HasFastVertexTextures() const
{
	return false;
}

int CHardwareConfigDX11::MaxHWMorphBatchCount() const
{
	return 0;
}


// Does the board actually support this?
bool CHardwareConfigDX11::ActuallySupportsPixelShaders_2_b() const
{
	return false;
}


bool CHardwareConfigDX11::SupportsHDRMode(HDRType_t nHDRMode) const
{
	return false;
}


bool CHardwareConfigDX11::GetHDREnabled(void) const
{
	return false;
}

void CHardwareConfigDX11::SetHDREnabled(bool bEnable)
{
	return;
}


bool CHardwareConfigDX11::SupportsBorderColor(void) const
{
	return false;
}

bool CHardwareConfigDX11::SupportsFetch4(void) const
{
	return false;
}

