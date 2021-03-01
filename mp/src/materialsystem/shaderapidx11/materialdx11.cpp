#include "materialdx11.h"

// Get the name of the material.  This is a full path to 
// the vmt file starting from "hl2/materials" (or equivalent) without
// a file extension.
const char *CMaterialDX11::GetName() const
{
	return "Epic Material";
}
const char *CMaterialDX11::GetTextureGroupName() const
{
	return "Epic Group";
}

// Get the preferred size/bitDepth of a preview image of a material.
// This is the sort of image that you would use for a thumbnail view
// of a material, or in WorldCraft until it uses materials to render.
// separate this for the tools maybe
PreviewImageRetVal_t CMaterialDX11::GetPreviewImageProperties(int *width, int *height,
	ImageFormat *imageFormat, bool* isTranslucent) const
{
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}

// Get a preview image at the specified width/height and bitDepth.
// Will do resampling if necessary.(not yet!!! :) )
// Will do color format conversion. (works now.)
PreviewImageRetVal_t CMaterialDX11::GetPreviewImage(unsigned char *data,
	int width, int height,
	ImageFormat imageFormat) const
{
	return PreviewImageRetVal_t::MATERIAL_NO_PREVIEW_IMAGE;
}
// 
int	CMaterialDX11::GetMappingWidth()
{
	return -1;
}

int	CMaterialDX11::GetMappingHeight()
{
	return -1;
}

int	CMaterialDX11::GetNumAnimationFrames()
{
	return -1;
}

// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
bool CMaterialDX11::InMaterialPage(void)
{
	return false;
}
void CMaterialDX11::GetMaterialOffset(float *pOffset)
{
	
}

void CMaterialDX11::GetMaterialScale(float *pScale)
{

}

IMaterial *CMaterialDX11::GetMaterialPage(void)
{
	return (IMaterial *)NULL;
}

// find a vmt variable.
// This is how game code affects how a material is rendered.
// The game code must know about the params that are used by
// the shader for the material that it is trying to affect.
IMaterialVar *CMaterialDX11::FindVar(const char *varName, bool *found, bool complain /*= true*/)
{
	return (IMaterialVar *)NULL;
}

// The user never allocates or deallocates materials.  Reference counting is
// used instead.  Garbage collection is done upon a call to 
// IMaterialSystem::UncacheUnusedMaterials.
void CMaterialDX11::IncrementReferenceCount(void)
{
	
}

void CMaterialDX11::DecrementReferenceCount(void)
{

}

// Each material is assigned a number that groups it with like materials
// for sorting in the application.
int CMaterialDX11::GetEnumerationID(void) const
{
	return -1;
}

void CMaterialDX11::GetLowResColorSample(float s, float t, float *color) const
{

}

// This computes the state snapshots for this material
void CMaterialDX11::RecomputeStateSnapshots()
{

}

// Are we translucent?
bool CMaterialDX11::IsTranslucent()
{
	return false;
}

// Are we alphatested?
bool CMaterialDX11::IsAlphaTested()
{
	return false;
}

// Are we vertex lit?
bool CMaterialDX11::IsVertexLit()
{
	return false;
}

// Gets the vertex format
VertexFormat_t CMaterialDX11::GetVertexFormat() const
{
	return VERTEX_FORMAT_UNKNOWN;
}

// returns true if this material uses a material proxy
bool CMaterialDX11::HasProxy(void) const
{
	return false;
}

bool CMaterialDX11::UsesEnvCubemap(void)
{
	return false;
}

bool CMaterialDX11::NeedsTangentSpace(void)
{
	return false;
}

bool CMaterialDX11::NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	return false;
}

bool CMaterialDX11::NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame /*= true*/)
{
	return false;
}

// returns true if the shader doesn't do skinning itself and requires
// the data that is sent to it to be preskinned.
bool CMaterialDX11::NeedsSoftwareSkinning(void)
{
	return false;
}

// Apply constant color or alpha modulation
void CMaterialDX11::AlphaModulate(float alpha)
{

}

void CMaterialDX11::ColorModulate(float r, float g, float b)
{

}

// Material Var flags...
void CMaterialDX11::SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
{

}

bool CMaterialDX11::GetMaterialVarFlag(MaterialVarFlags_t flag) const
{
	return false;
}

// Gets material reflectivity
void CMaterialDX11::GetReflectivity(Vector& reflect)
{

}

// Gets material property flags
bool CMaterialDX11::GetPropertyFlag(MaterialPropertyTypes_t type)
{
	return false;
}

// Is the material visible from both sides?
bool CMaterialDX11::IsTwoSided()
{
	return false;
}

// Sets the shader associated with the material
void CMaterialDX11::SetShader(const char *pShaderName)
{

}

// Can't be const because the material might have to precache itself.
int CMaterialDX11::GetNumPasses(void)
{
	return -1;
}

// Can't be const because the material might have to precache itself.
int	CMaterialDX11::GetTextureMemoryBytes(void)
{
	return -1;
}

// Meant to be used with materials created using CreateMaterial
// It updates the materials to reflect the current values stored in the material vars
void CMaterialDX11::Refresh()
{

}

// GR - returns true is material uses lightmap alpha for blending
bool CMaterialDX11::NeedsLightmapBlendAlpha(void)
{
	return false;
}

// returns true if the shader doesn't do lighting itself and requires
// the data that is sent to it to be prelighted
bool CMaterialDX11::NeedsSoftwareLighting(void)
{
	return false;
}

// Gets at the shader parameters
int CMaterialDX11::ShaderParamCount() const
{
	return -1;
}

IMaterialVar **CMaterialDX11::GetShaderParams(void)
{
	return (IMaterialVar**)NULL;
}

// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
// the material can't be found.
bool CMaterialDX11::IsErrorMaterial() const
{
	return false;
}

void CMaterialDX11::SetUseFixedFunctionBakedLighting(bool bEnable)
{

}

// Gets the current alpha modulation
float CMaterialDX11::GetAlphaModulation()
{
	return -1.f;
}

void CMaterialDX11::GetColorModulation(float *r, float *g, float *b)
{

}

// Gets the morph format
MorphFormat_t CMaterialDX11::GetMorphFormat() const
{
	return MORPH_POSITION;
}

// fast find that stores the index of the found var in the string table in local cache
IMaterialVar *CMaterialDX11::FindVarFast(char const *pVarName, unsigned int *pToken)
{
	return (IMaterialVar *)NULL;
}

// Sets new VMT shader parameters for the material
void CMaterialDX11::SetShaderAndParams(KeyValues *pKeyValues)
{

}

const char *CMaterialDX11::GetShaderName() const
{
	return "Epic Shader";
}

void CMaterialDX11::DeleteIfUnreferenced()
{

}

bool CMaterialDX11::IsSpriteCard()
{
	return false;
}

void CMaterialDX11::CallBindProxy(void *proxyData)
{

}

IMaterial *CMaterialDX11::CheckProxyReplacement(void *proxyData)
{
	return (IMaterial *)NULL;
}

void CMaterialDX11::RefreshPreservingMaterialVars()
{

}

bool CMaterialDX11::WasReloadedFromWhitelist()
{
	return false;
}

bool CMaterialDX11::IsPrecached() const
{
	return false;
}

int CMaterialDX11::GetReferenceCount() const
{
	return -1;
}

void CMaterialDX11::SetEnumerationID(int)
{

}

void CMaterialDX11::SetNeedsWhiteLightmap(bool)
{

}

bool CMaterialDX11::GetNeedsWhiteLightmap() const
{
	return false;
}

void CMaterialDX11::Uncache(bool)
{

}

void CMaterialDX11::Precache()
{

}

void CMaterialDX11::PrecacheVars(KeyValues*, KeyValues*, CUtlVector<void*>*, int)
{

}

void CMaterialDX11::ReloadTextures()
{

}

void CMaterialDX11::SetMinLightmapPageID(int)
{

}

void CMaterialDX11::SetMaxLightmapPageID(int)
{

}

int CMaterialDX11::GetMinLightmapPageID()
{
	return -1;
}

int CMaterialDX11::GetMaxLightmapPageID()
{
	return -1;
}

IShader* CMaterialDX11::GetShader()
{
	return (IShader *)NULL;
}

bool CMaterialDX11::IsPrecachedVars()
{
	return false;
}

void CMaterialDX11::DrawMesh(VertexCompressionType_t compression)
{

}

VertexFormat_t CMaterialDX11::GetVertexUsage() const
{
	return VERTEX_FORMAT_UNKNOWN;
}
