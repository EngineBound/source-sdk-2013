#ifndef MATERIALDX11_H
#define MATERIALDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "imaterialdx11.h"

class CMaterialDX11 : public IMaterialExtended
{
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName() const;
	virtual const char *	GetTextureGroupName() const;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
		ImageFormat *imageFormat, bool* isTranslucent) const;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
		int width, int height,
		ImageFormat imageFormat) const;
	// 
	virtual int				GetMappingWidth();
	virtual int				GetMappingHeight();

	virtual int				GetNumAnimationFrames();

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage(void);
	virtual	void			GetMaterialOffset(float *pOffset);
	virtual void			GetMaterialScale(float *pScale);
	virtual IMaterial		*GetMaterialPage(void);

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true);

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void);
	virtual void			DecrementReferenceCount(void);

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const;

	virtual void			GetLowResColorSample(float s, float t, float *color) const;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots();

	// Are we translucent?
	virtual bool			IsTranslucent();

	// Are we alphatested?
	virtual bool			IsAlphaTested();

	// Are we vertex lit?
	virtual bool			IsVertexLit();

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat() const;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const;

	virtual bool			UsesEnvCubemap(void);

	virtual bool			NeedsTangentSpace(void);

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true);
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true);

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void);

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha);
	virtual void			ColorModulate(float r, float g, float b);

	// Material Var flags...
	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on);
	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector& reflect);

	// Gets material property flags
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type);

	// Is the material visible from both sides?
	virtual bool			IsTwoSided();

	// Sets the shader associated with the material
	virtual void			SetShader(const char *pShaderName);

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void);

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void);

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh();

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void);

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void);

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const;
	virtual IMaterialVar	**GetShaderParams(void);

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable);

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation();
	virtual void			GetColorModulation(float *r, float *g, float *b);

	// Gets the morph format
	virtual MorphFormat_t	GetMorphFormat() const;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar *	FindVarFast(char const *pVarName, unsigned int *pToken);

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(KeyValues *pKeyValues);
	virtual const char *	GetShaderName() const;

	virtual void			DeleteIfUnreferenced();

	virtual bool			IsSpriteCard();

	virtual void			CallBindProxy(void *proxyData);

	virtual IMaterial		*CheckProxyReplacement(void *proxyData);

	virtual void			RefreshPreservingMaterialVars();

	virtual bool			WasReloadedFromWhitelist();

	virtual bool			IsPrecached() const;

	virtual int GetReferenceCount() const;
	virtual void SetEnumerationID(int);
	virtual void SetNeedsWhiteLightmap(bool);
	virtual bool GetNeedsWhiteLightmap() const;
	virtual void Uncache(bool);
	virtual void Precache();
	virtual void PrecacheVars(KeyValues*, KeyValues*, CUtlVector<void*>*, int);
	virtual void ReloadTextures();
	virtual void SetMinLightmapPageID(int);
	virtual void SetMaxLightmapPageID(int);
	virtual int GetMinLightmapPageID();
	virtual int GetMaxLightmapPageID();
	virtual IShader* GetShader();
	virtual bool IsPrecachedVars();
	virtual void DrawMesh(VertexCompressionType_t compression);
	virtual VertexFormat_t GetVertexUsage() const;
};

#endif