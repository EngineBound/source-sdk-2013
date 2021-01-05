#ifndef SHADERUTILDX11_H
#define SHADERUTILDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishaderutil.h"

// -------------------------------------------------------------------------------------------
// CShaderUtilDX11
//
// This system fulfils the IShaderUtil interface, because I couldn't import it from materialsystem :)
// -------------------------------------------------------------------------------------------

class CShaderUtilDX11 : IShaderUtil
{
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) { return true; }
	virtual void Disconnect() {}

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName) { return NULL; }

	// Init, shutdown
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}

	// Method to allow clients access to the MaterialSystem_Config
	virtual MaterialSystem_Config_t& GetConfig() = 0;

	// Allows us to convert image formats
	virtual bool ConvertImageFormat(unsigned char *src, enum ImageFormat srcImageFormat,
		unsigned char *dst, enum ImageFormat dstImageFormat,
		int width, int height, int srcStride = 0, int dstStride = 0) = 0;

	// Figures out the amount of memory needed by a bitmap
	virtual int GetMemRequired(int width, int height, int depth, ImageFormat format, bool mipmap) = 0;

	// Gets image format info
	virtual const ImageFormatInfo_t& ImageFormatInfo(ImageFormat fmt) const = 0;

	// Bind standard textures
	virtual void BindStandardTexture(Sampler_t sampler, StandardTextureId_t id) = 0;

	// What are the lightmap dimensions?
	virtual void GetLightmapDimensions(int *w, int *h) = 0;

	// These methods are called when the shader must eject + restore HW memory
	virtual void ReleaseShaderObjects() = 0;
	virtual void RestoreShaderObjects(CreateInterfaceFn shaderFactory, int nChangeFlags = 0) = 0;

	// Used to prevent meshes from drawing.
	virtual bool IsInStubMode() = 0;
	virtual bool InFlashlightMode() const = 0;

	// For the shader API to shove the current version of aniso level into the
	// "definitive" place (g_config) when the shader API decides to change it.
	// Eventually, we should have a better system of who owns the definitive
	// versions of config vars.
	virtual void NoteAnisotropicLevel(int currentLevel) = 0;

	// NOTE: Stuff after this is added after shipping HL2.

	// Are we rendering through the editor?
	virtual bool InEditorMode() const = 0;

	// Gets the bound morph's vertex format; returns 0 if no morph is bound
	virtual MorphFormat_t GetBoundMorphFormat() = 0;

	virtual ITexture *GetRenderTargetEx(int nRenderTargetID) = 0;

	// Tells the material system to draw a buffer clearing quad
	virtual void DrawClearBufferQuad(unsigned char r, unsigned char g, unsigned char b, unsigned char a, bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;

	// Calls from meshes to material system to handle queing/threading
	virtual bool OnDrawMesh(IMesh *pMesh, int firstIndex, int numIndices) = 0;
	virtual bool OnDrawMesh(IMesh *pMesh, CPrimList *pLists, int nLists) = 0;
	virtual bool OnSetFlexMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes) = 0;
	virtual bool OnSetColorMesh(IMesh *pStaticMesh, IMesh *pMesh, int nVertexOffsetInBytes) = 0;
	virtual bool OnSetPrimitiveType(IMesh *pMesh, MaterialPrimitiveType_t type) = 0;
	virtual bool OnFlushBufferedPrimitives() = 0;


	virtual void SyncMatrices() = 0;
	virtual void SyncMatrix(MaterialMatrixMode_t) = 0;

	virtual void BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id) = 0;
	virtual void GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id) = 0;

	virtual int MaxHWMorphBatchCount() const = 0;

	// Interface for mat system to tell shaderapi about color correction
	virtual void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo) = 0;
	// received an event while not in owning thread, handle this outside
	virtual void OnThreadEvent(uint32 threadEvent) = 0;

	virtual MaterialThreadMode_t	GetThreadMode() = 0;
	virtual bool					IsRenderThreadSafe() = 0;

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
};

#endif