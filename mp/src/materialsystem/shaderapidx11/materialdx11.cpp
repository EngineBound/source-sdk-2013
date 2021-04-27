#include "materialdx11.h"

CMaterialDX11::CMaterialDX11(char const* materialName, const char *pTextureGroupName, KeyValues *pKeyValues)
{
	int nNameLen = Q_strlen(materialName);
	char *pTmpName = (char *)_alloca(nNameLen + 1);

	Q_StripExtension(materialName, pTmpName, nNameLen + 1);
	Q_strlower(pTmpName);

	m_Name = pTmpName;
	m_TextureGroupName = pTextureGroupName;
	
	m_pKeyValues = pKeyValues;
}

CMaterialDX11::~CMaterialDX11()
{
	if (m_pKeyValues)
	{
		m_pKeyValues->deleteThis();
		m_pKeyValues = NULL;
	}
}

// Get the name of the material.  This is a full path to 
// the vmt file starting from "hl2/materials" (or equivalent) without
// a file extension.
const char *CMaterialDX11::GetName() const
{
	return m_Name.String();
}
const char *CMaterialDX11::GetTextureGroupName() const
{
	return m_TextureGroupName.String();
}

// Get the preferred size/bitDepth of a preview image of a material.
// This is the sort of image that you would use for a thumbnail view
// of a material, or in WorldCraft until it uses materials to render.
// separate this for the tools maybe
PreviewImageRetVal_t CMaterialDX11::GetPreviewImageProperties(int *width, int *height,
	ImageFormat *imageFormat, bool* isTranslucent) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}

// Get a preview image at the specified width/height and bitDepth.
// Will do resampling if necessary.(not yet!!! :) )
// Will do color format conversion. (works now.)
PreviewImageRetVal_t CMaterialDX11::GetPreviewImage(unsigned char *data,
	int width, int height,
	ImageFormat imageFormat) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}
// 
int	CMaterialDX11::GetMappingWidth()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

int	CMaterialDX11::GetMappingHeight()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

int	CMaterialDX11::GetNumAnimationFrames()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
bool CMaterialDX11::InMaterialPage(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}
void CMaterialDX11::GetMaterialOffset(float *pOffset)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMaterialDX11::GetMaterialScale(float *pScale)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

IMaterial *CMaterialDX11::GetMaterialPage(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return (IMaterial *)NULL;
}

// find a vmt variable.
// This is how game code affects how a material is rendered.
// The game code must know about the params that are used by
// the shader for the material that it is trying to affect.
IMaterialVar *CMaterialDX11::FindVar(const char *varName, bool *found, bool complain /*= true*/)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return (IMaterialVar *)NULL;
}

// The user never allocates or deallocates materials.  Reference counting is
// used instead.  Garbage collection is done upon a call to 
// IMaterialSystem::UncacheUnusedMaterials.
void CMaterialDX11::IncrementReferenceCount(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMaterialDX11::DecrementReferenceCount(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Each material is assigned a number that groups it with like materials
// for sorting in the application.
int CMaterialDX11::GetEnumerationID(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

void CMaterialDX11::GetLowResColorSample(float s, float t, float *color) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// This computes the state snapshots for this material
void CMaterialDX11::RecomputeStateSnapshots()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Are we translucent?
bool CMaterialDX11::IsTranslucent()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Are we alphatested?
bool CMaterialDX11::IsAlphaTested()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Are we vertex lit?
bool CMaterialDX11::IsVertexLit()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Gets the vertex format
VertexFormat_t CMaterialDX11::GetVertexFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return VERTEX_FORMAT_UNKNOWN;
}

// returns true if this material uses a material proxy
bool CMaterialDX11::HasProxy(void) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CMaterialDX11::UsesEnvCubemap(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CMaterialDX11::NeedsTangentSpace(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CMaterialDX11::NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CMaterialDX11::NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// returns true if the shader doesn't do skinning itself and requires
// the data that is sent to it to be preskinned.
bool CMaterialDX11::NeedsSoftwareSkinning(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Apply constant color or alpha modulation
void CMaterialDX11::AlphaModulate(float alpha)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

void CMaterialDX11::ColorModulate(float r, float g, float b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Material Var flags...
void CMaterialDX11::SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

bool CMaterialDX11::GetMaterialVarFlag(MaterialVarFlags_t flag) const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Gets material reflectivity
void CMaterialDX11::GetReflectivity(Vector& reflect)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Gets material property flags
bool CMaterialDX11::GetPropertyFlag(MaterialPropertyTypes_t type)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Is the material visible from both sides?
bool CMaterialDX11::IsTwoSided()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Sets the shader associated with the material
void CMaterialDX11::SetShader(const char *pShaderName)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Can't be const because the material might have to precache itself.
int CMaterialDX11::GetNumPasses(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

// Can't be const because the material might have to precache itself.
int	CMaterialDX11::GetTextureMemoryBytes(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

// Meant to be used with materials created using CreateMaterial
// It updates the materials to reflect the current values stored in the material vars
void CMaterialDX11::Refresh()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// GR - returns true is material uses lightmap alpha for blending
bool CMaterialDX11::NeedsLightmapBlendAlpha(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// returns true if the shader doesn't do lighting itself and requires
// the data that is sent to it to be prelighted
bool CMaterialDX11::NeedsSoftwareLighting(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

// Gets at the shader parameters
int CMaterialDX11::ShaderParamCount() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1;
}

IMaterialVar **CMaterialDX11::GetShaderParams(void)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return (IMaterialVar**)NULL;
}

// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
// the material can't be found.
bool CMaterialDX11::IsErrorMaterial() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CMaterialDX11::SetUseFixedFunctionBakedLighting(bool bEnable)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Gets the current alpha modulation
float CMaterialDX11::GetAlphaModulation()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return -1.f;
}

void CMaterialDX11::GetColorModulation(float *r, float *g, float *b)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

// Gets the morph format
MorphFormat_t CMaterialDX11::GetMorphFormat() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return MORPH_POSITION;
}

// fast find that stores the index of the found var in the string table in local cache
IMaterialVar *CMaterialDX11::FindVarFast(char const *pVarName, unsigned int *pToken)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return (IMaterialVar *)NULL;
}

// Sets new VMT shader parameters for the material
void CMaterialDX11::SetShaderAndParams(KeyValues *pKeyValues)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

const char *CMaterialDX11::GetShaderName() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return "Epic Shader";
}

void CMaterialDX11::DeleteIfUnreferenced()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

bool CMaterialDX11::IsSpriteCard()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

void CMaterialDX11::CallBindProxy(void *proxyData)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

IMaterial *CMaterialDX11::CheckProxyReplacement(void *proxyData)
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return (IMaterial *)NULL;
}

void CMaterialDX11::RefreshPreservingMaterialVars()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
}

bool CMaterialDX11::WasReloadedFromWhitelist()
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}

bool CMaterialDX11::IsPrecached() const
{
	_AssertMsg(0, "Not implemented! " __FUNCTION__, 0, 0);
	return false;
}