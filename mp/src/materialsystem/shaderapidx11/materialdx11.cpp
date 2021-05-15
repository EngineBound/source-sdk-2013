#include "dx11global.h"

#include "materialdx11.h"

#include "memdbgon.h"

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
	ALERT_NOT_IMPLEMENTED();
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}

// Get a preview image at the specified width/height and bitDepth.
// Will do resampling if necessary.(not yet!!! :) )
// Will do color format conversion. (works now.)
PreviewImageRetVal_t CMaterialDX11::GetPreviewImage(unsigned char *data,
	int width, int height,
	ImageFormat imageFormat) const
{
	ALERT_NOT_IMPLEMENTED();
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}
// 
int	CMaterialDX11::GetMappingWidth()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

int	CMaterialDX11::GetMappingHeight()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

int	CMaterialDX11::GetNumAnimationFrames()
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
bool CMaterialDX11::InMaterialPage(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}
void CMaterialDX11::GetMaterialOffset(float *pOffset)
{
	ALERT_NOT_IMPLEMENTED();
}

void CMaterialDX11::GetMaterialScale(float *pScale)
{
	ALERT_NOT_IMPLEMENTED();
}

IMaterial *CMaterialDX11::GetMaterialPage(void)
{
	ALERT_NOT_IMPLEMENTED();
	return (IMaterial *)NULL;
}

// find a vmt variable.
// This is how game code affects how a material is rendered.
// The game code must know about the params that are used by
// the shader for the material that it is trying to affect.
IMaterialVar *CMaterialDX11::FindVar(const char *varName, bool *found, bool complain /*= true*/)
{
	ALERT_NOT_IMPLEMENTED();
	return (IMaterialVar *)NULL;
}

// The user never allocates or deallocates materials.  Reference counting is
// used instead.  Garbage collection is done upon a call to 
// IMaterialSystem::UncacheUnusedMaterials.
void CMaterialDX11::IncrementReferenceCount(void)
{
	ALERT_NOT_IMPLEMENTED();
}

void CMaterialDX11::DecrementReferenceCount(void)
{
	ALERT_NOT_IMPLEMENTED();
}

// Each material is assigned a number that groups it with like materials
// for sorting in the application.
int CMaterialDX11::GetEnumerationID(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

void CMaterialDX11::GetLowResColorSample(float s, float t, float *color) const
{
	ALERT_NOT_IMPLEMENTED();
}

// This computes the state snapshots for this material
void CMaterialDX11::RecomputeStateSnapshots()
{
	ALERT_NOT_IMPLEMENTED();
}

// Are we translucent?
bool CMaterialDX11::IsTranslucent()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Are we alphatested?
bool CMaterialDX11::IsAlphaTested()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Are we vertex lit?
bool CMaterialDX11::IsVertexLit()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Gets the vertex format
VertexFormat_t CMaterialDX11::GetVertexFormat() const
{
	ALERT_NOT_IMPLEMENTED();
	return VERTEX_FORMAT_UNKNOWN;
}

// returns true if this material uses a material proxy
bool CMaterialDX11::HasProxy(void) const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CMaterialDX11::UsesEnvCubemap(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CMaterialDX11::NeedsTangentSpace(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CMaterialDX11::NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CMaterialDX11::NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// returns true if the shader doesn't do skinning itself and requires
// the data that is sent to it to be preskinned.
bool CMaterialDX11::NeedsSoftwareSkinning(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Apply constant color or alpha modulation
void CMaterialDX11::AlphaModulate(float alpha)
{
	ALERT_NOT_IMPLEMENTED();
}

void CMaterialDX11::ColorModulate(float r, float g, float b)
{
	ALERT_NOT_IMPLEMENTED();
}

// Material Var flags...
void CMaterialDX11::SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
{
	ALERT_NOT_IMPLEMENTED();
}

bool CMaterialDX11::GetMaterialVarFlag(MaterialVarFlags_t flag) const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Gets material reflectivity
void CMaterialDX11::GetReflectivity(Vector& reflect)
{
	ALERT_NOT_IMPLEMENTED();
}

// Gets material property flags
bool CMaterialDX11::GetPropertyFlag(MaterialPropertyTypes_t type)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Is the material visible from both sides?
bool CMaterialDX11::IsTwoSided()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Sets the shader associated with the material
void CMaterialDX11::SetShader(const char *pShaderName)
{
	ALERT_NOT_IMPLEMENTED();
}

// Can't be const because the material might have to precache itself.
int CMaterialDX11::GetNumPasses(void)
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

// Can't be const because the material might have to precache itself.
int	CMaterialDX11::GetTextureMemoryBytes(void)
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

// Meant to be used with materials created using CreateMaterial
// It updates the materials to reflect the current values stored in the material vars
void CMaterialDX11::Refresh()
{
	ALERT_NOT_IMPLEMENTED();
}

// GR - returns true is material uses lightmap alpha for blending
bool CMaterialDX11::NeedsLightmapBlendAlpha(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// returns true if the shader doesn't do lighting itself and requires
// the data that is sent to it to be prelighted
bool CMaterialDX11::NeedsSoftwareLighting(void)
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

// Gets at the shader parameters
int CMaterialDX11::ShaderParamCount() const
{
	ALERT_NOT_IMPLEMENTED();
	return -1;
}

IMaterialVar **CMaterialDX11::GetShaderParams(void)
{
	ALERT_NOT_IMPLEMENTED();
	return (IMaterialVar**)NULL;
}

// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
// the material can't be found.
bool CMaterialDX11::IsErrorMaterial() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CMaterialDX11::SetUseFixedFunctionBakedLighting(bool bEnable)
{
	ALERT_NOT_IMPLEMENTED();
}

// Gets the current alpha modulation
float CMaterialDX11::GetAlphaModulation()
{
	ALERT_NOT_IMPLEMENTED();
	return -1.f;
}

void CMaterialDX11::GetColorModulation(float *r, float *g, float *b)
{
	ALERT_NOT_IMPLEMENTED();
}

// Gets the morph format
MorphFormat_t CMaterialDX11::GetMorphFormat() const
{
	ALERT_NOT_IMPLEMENTED();
	return MORPH_POSITION;
}

// fast find that stores the index of the found var in the string table in local cache
IMaterialVar *CMaterialDX11::FindVarFast(char const *pVarName, unsigned int *pToken)
{
	ALERT_NOT_IMPLEMENTED();
	return (IMaterialVar *)NULL;
}

// Sets new VMT shader parameters for the material
void CMaterialDX11::SetShaderAndParams(KeyValues *pKeyValues)
{
	ALERT_NOT_IMPLEMENTED();
}

const char *CMaterialDX11::GetShaderName() const
{
	ALERT_NOT_IMPLEMENTED();
	return "Epic Shader";
}

void CMaterialDX11::DeleteIfUnreferenced()
{
	ALERT_NOT_IMPLEMENTED();
}

bool CMaterialDX11::IsSpriteCard()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

void CMaterialDX11::CallBindProxy(void *proxyData)
{
	ALERT_NOT_IMPLEMENTED();
}

IMaterial *CMaterialDX11::CheckProxyReplacement(void *proxyData)
{
	ALERT_NOT_IMPLEMENTED();
	return (IMaterial *)NULL;
}

void CMaterialDX11::RefreshPreservingMaterialVars()
{
	ALERT_NOT_IMPLEMENTED();
}

bool CMaterialDX11::WasReloadedFromWhitelist()
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}

bool CMaterialDX11::IsPrecached() const
{
	ALERT_NOT_IMPLEMENTED();
	return false;
}