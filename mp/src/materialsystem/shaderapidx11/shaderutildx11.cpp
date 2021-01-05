#include "shaderutildx11.h"
#include "shaderapidx11_global.h"
#include "materialsystem/materialsystem_config.h"

static MaterialSystem_Config_t tmp_gConfig;

// Method to allow clients access to the MaterialSystem_Config
// Take from materialsystem's GetCurrentConfigForVideoCard function
MaterialSystem_Config_t& CShaderUtilDX11::GetConfig()
{
	tmp_gConfig = MaterialSystem_Config_t();//g_pMaterialSystem->GetCurrentConfigForVideoCard();
	return tmp_gConfig;
}

// Allows us to convert image formats
bool CShaderUtilDX11::ConvertImageFormat(unsigned char *src, enum ImageFormat srcImageFormat,
	unsigned char *dst, enum ImageFormat dstImageFormat,
	int width, int height, int srcStride, int dstStride)
{
	return false;
}

// Figures out the amount of memory needed by a bitmap
int CShaderUtilDX11::GetMemRequired(int width, int height, int depth, ImageFormat format, bool mipmap)
{
	return 0;
}

// Gets image format info
const ImageFormatInfo_t& CShaderUtilDX11::ImageFormatInfo(ImageFormat fmt) const
{
	return (ImageFormatInfo_t &)tmp_gConfig;
}

// Bind standard textures
void CShaderUtilDX11::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	// Do nothing for now 
}

// What are the lightmap dimensions?
void CShaderUtilDX11::GetLightmapDimensions(int *w, int *h)
{
	// LOOKS LIKE WE NEED TO REWRITE THE FUCKING RENDER CONTEXT TOO !!!!!!
	// FUN!
}

// These methods are called when the shader must eject + restore HW memory
void CShaderUtilDX11::ReleaseShaderObjects()
{
	
}
void CShaderUtilDX11::RestoreShaderObjects(CreateInterfaceFn shaderFactory, int nChangeFlags)
{

}

// Used to prevent meshes from drawing.
bool CShaderUtilDX11::IsInStubMode()
{
	return false;
}

bool CShaderUtilDX11::InFlashlightMode() const
{
	return false;
}

// For the shader API to shove the current version of aniso level into the
// "definitive" place (g_config) when the shader API decides to change it.
// Eventually, we should have a better system of who owns the definitive
// versions of config vars.
void CShaderUtilDX11::NoteAnisotropicLevel(int currentLevel)
{
	
}

// NOTE: Stuff after this is added after shipping HL2.

// Are we rendering through the editor?
bool CShaderUtilDX11::InEditorMode() const
{
	return false;
}

// Gets the bound morph's vertex format; returns 0 if no morph is bound
MorphFormat_t CShaderUtilDX11::GetBoundMorphFormat()
{
	return (MorphFormat_t)0;
}

ITexture *CShaderUtilDX11::GetRenderTargetEx(int nRenderTargetID)
{
	return (ITexture *)0;
}

// Tells the material system to draw a buffer clearing quad
void CShaderUtilDX11::DrawClearBufferQuad(unsigned char r, unsigned char g, unsigned char b, unsigned char a, bool bClearColor, bool bClearAlpha, bool bClearDepth)
{

}

// Calls from meshes to material system to handle queing/threading
bool CShaderUtilDX11::OnDrawMesh(IMesh *pMesh, int firstIndex, int numIndices)
{
	return false;
}

bool CShaderUtilDX11::OnDrawMesh(IMesh *pMesh, CPrimList *pLists, int nLists)
{
	return false;
}

bool CShaderUtilDX11::OnSetFlexMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes)
{
	return false;
}

bool CShaderUtilDX11::OnSetColorMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes)
{
	return false;
}

bool CShaderUtilDX11::OnSetPrimitiveType(IMesh *pMesh, MaterialPrimitiveType_t type)
{
	return false;
}

bool CShaderUtilDX11::OnFlushBufferedPrimitives()
{
	return false;
}

void CShaderUtilDX11::SyncMatrices()
{

}

void CShaderUtilDX11::SyncMatrix(MaterialMatrixMode_t)
{

}

void CShaderUtilDX11::BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id)
{

}

void CShaderUtilDX11::GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id)
{

}

int CShaderUtilDX11::MaxHWMorphBatchCount() const
{
	return 0;
}

// Interface for mat system to tell shaderapi about color correction
void CShaderUtilDX11::GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
{

}
// received an event while not in owning thread, handle this outside
void CShaderUtilDX11::OnThreadEvent(uint32 threadEvent)
{

}

MaterialThreadMode_t	CShaderUtilDX11::GetThreadMode()
{
	return (MaterialThreadMode_t)0;
}

bool					CShaderUtilDX11::IsRenderThreadSafe()
{
	return false;
}

// Remove any materials from memory that aren't in use as determined
// by the IMaterial's reference count.
void CShaderUtilDX11::UncacheUnusedMaterials(bool bRecomputeStateSnapshots)
{

}