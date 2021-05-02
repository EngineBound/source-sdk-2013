#ifndef SHADERAPIDX11_H
#define SHADERAPIDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishaderapidx11.h"
#include "statesdx11.h"
#include "apitexturedx11.h"

class IMeshDX11;

class CShaderAPIDX11 : public IShaderAPIDX11, public IDebugTextureInfoDX11
{
public:
	CShaderAPIDX11();
	~CShaderAPIDX11();

	// ------------------------------------------------------- //
	//                       IShaderAPI                        //
	// ------------------------------------------------------- //

	// Viewport methods
	virtual void SetViewports(int nCount, const ShaderViewport_t* pViewports);
	virtual int GetViewports(ShaderViewport_t* pViewports, int nMax) const;

	// returns the current time in seconds....
	virtual double CurrentTime() const;

	// Gets the lightmap dimensions
	virtual void GetLightmapDimensions(int *w, int *h);

	// Scene fog state.
	// This is used by the shaders for picking the proper vertex shader for fogging based on dynamic state.
	virtual MaterialFogMode_t GetSceneFogMode();
	virtual void GetSceneFogColor(unsigned char *rgb);

	// stuff related to matrix stacks
	virtual void MatrixMode(MaterialMatrixMode_t matrixMode);
	virtual void PushMatrix();
	virtual void PopMatrix();
	virtual void LoadMatrix(float *m);
	virtual void MultMatrix(float *m);
	virtual void MultMatrixLocal(float *m);
	virtual void GetMatrix(MaterialMatrixMode_t matrixMode, float *dst);
	virtual void LoadIdentity(void);
	virtual void LoadCameraToWorld(void);
	virtual void Ortho(double left, double right, double bottom, double top, double zNear, double zFar);
	virtual void PerspectiveX(double fovx, double aspect, double zNear, double zFar);
	virtual	void PickMatrix(int x, int y, int width, int height);
	virtual void Rotate(float angle, float x, float y, float z);
	virtual void Translate(float x, float y, float z);
	virtual void Scale(float x, float y, float z);
	virtual void ScaleXY(float x, float y);

	// Sets the color to modulate by
	virtual void Color3f(float r, float g, float b);
	virtual void Color3fv(float const* pColor);
	virtual void Color4f(float r, float g, float b, float a);
	virtual void Color4fv(float const* pColor);

	virtual void Color3ub(unsigned char r, unsigned char g, unsigned char b);
	virtual void Color3ubv(unsigned char const* pColor);
	virtual void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	virtual void Color4ubv(unsigned char const* pColor);

	// Sets the constant register for vertex and pixel shaders
	virtual void SetVertexShaderConstant(int var, float const* pVec, int numConst = 1, bool bForce = false);
	virtual void SetPixelShaderConstant(int var, float const* pVec, int numConst = 1, bool bForce = false);

	// Sets the default *dynamic* state
	virtual void SetDefaultState();

	// Get the current camera position in world space.
	virtual void GetWorldSpaceCameraPosition(float* pPos) const;

	virtual int GetCurrentNumBones(void) const;
	virtual int GetCurrentLightCombo(void) const;

	virtual MaterialFogMode_t GetCurrentFogType(void) const;

	// fixme: move this to shadow state
	virtual void SetTextureTransformDimension(TextureStage_t textureStage, int dimension, bool projected);
	virtual void DisableTextureTransform(TextureStage_t textureStage);
	virtual void SetBumpEnvMatrix(TextureStage_t textureStage, float m00, float m01, float m10, float m11);

	// Sets the vertex and pixel shaders
	virtual void SetVertexShaderIndex(int vshIndex = -1);
	virtual void SetPixelShaderIndex(int pshIndex);

	// Get the dimensions of the back buffer.
	virtual void GetBackBufferDimensions(int& width, int& height) const;

	// FIXME: The following 6 methods used to live in IShaderAPI
	// and were moved for stdshader_dx8. Let's try to move them back!

	// Get the lights
	virtual int GetMaxLights(void) const;
	virtual const LightDesc_t& GetLight(int lightNum) const;

	virtual void SetPixelShaderFogParams(int reg);

	// Render state for the ambient light cube
	virtual void SetVertexShaderStateAmbientLightCube();
	virtual void SetPixelShaderStateAmbientLightCube(int pshReg, bool bForceToBlack = false);
	virtual void CommitPixelShaderLighting(int pshReg);

	// Use this to get the mesh builder that allows us to modify vertex data
	virtual CMeshBuilder* GetVertexModifyBuilder();
	virtual bool InFlashlightMode() const;
	virtual const FlashlightState_t &GetFlashlightState(VMatrix &worldToTexture) const;
	virtual bool InEditorMode() const;

	// Gets the bound morph's vertex format; returns 0 if no morph is bound
	virtual MorphFormat_t GetBoundMorphFormat();

	// Binds a standard texture
	virtual void BindStandardTexture(Sampler_t sampler, StandardTextureId_t id);

	virtual ITexture *GetRenderTargetEx(int nRenderTargetID);

	virtual void SetToneMappingScaleLinear(const Vector &scale);
	virtual const Vector &GetToneMappingScaleLinear(void) const;
	virtual float GetLightMapScaleFactor(void) const;

	virtual void LoadBoneMatrix(int boneIndex, const float *m);

	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right);

	virtual void GetDXLevelDefaults(uint &max_dxlevel, uint &recommended_dxlevel);

	virtual const FlashlightState_t &GetFlashlightStateEx(VMatrix &worldToTexture, ITexture **pFlashlightDepthTexture) const;

	virtual float GetAmbientLightCubeLuminance();

	virtual void GetDX9LightState(LightState_t *state) const;
	virtual int GetPixelFogCombo(); //0 is either range fog, or no fog simulated with rigged range fog values. 1 is height fog

	virtual void BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id);

	// Is hardware morphing enabled?
	virtual bool IsHWMorphingEnabled() const;

	virtual void GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id);

	virtual void SetBooleanVertexShaderConstant(int var, BOOL const* pVec, int numBools = 1, bool bForce = false);
	virtual void SetIntegerVertexShaderConstant(int var, int const* pVec, int numIntVecs = 1, bool bForce = false);
	virtual void SetBooleanPixelShaderConstant(int var, BOOL const* pVec, int numBools = 1, bool bForce = false);
	virtual void SetIntegerPixelShaderConstant(int var, int const* pVec, int numIntVecs = 1, bool bForce = false);

	//Are we in a configuration that needs access to depth data through the alpha channel later?
	virtual bool ShouldWriteDepthToDestAlpha(void) const;


	// deformations
	virtual void PushDeformation(DeformationBase_t const *Deformation);
	virtual void PopDeformation();
	virtual int GetNumActiveDeformations() const;


	// for shaders to set vertex shader constants. returns a packed state which can be used to set
	// the dynamic combo. returns # of active deformations
	virtual int GetPackedDeformationInformation(int nMaskOfUnderstoodDeformations,
		float *pConstantValuesOut,
		int nBufferSize,
		int nMaximumDeformations,
		int *pNumDefsOut) const;

	// This lets the lower level system that certain vertex fields requested 
	// in the shadow state aren't actually being read given particular state
	// known only at dynamic state time. It's here only to silence warnings.
	virtual void MarkUnusedVertexFields(unsigned int nFlags, int nTexCoordCount, bool *pUnusedTexCoords);


	virtual void ExecuteCommandBuffer(uint8 *pCmdBuffer);

	// interface for mat system to tell shaderapi about standard texture handles
	virtual void SetStandardTextureHandle(StandardTextureId_t nId, ShaderAPITextureHandle_t nHandle);

	// Interface for mat system to tell shaderapi about color correction
	virtual void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo);

	virtual void SetPSNearAndFarZ(int pshReg);

	virtual void SetDepthFeatheringPixelShaderConstant(int iConstant, float fDepthBlendScale);

	// Buffer clearing
	virtual void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil, int renderTargetWidth, int renderTargetHeight);
	virtual void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b);
	virtual void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	// Methods related to binding shaders
	virtual void BindVertexShader(VertexShaderHandle_t hVertexShader);
	virtual void BindGeometryShader(GeometryShaderHandle_t hGeometryShader);
	virtual void BindPixelShader(PixelShaderHandle_t hPixelShader);

	// Methods related to state objects
	virtual void SetRasterState(const ShaderRasterState_t& state);

	//
	// NOTE: These methods have not yet been ported to DX10
	//

	// Sets the mode...
	virtual bool SetMode(void* hwnd, int nAdapter, const ShaderDeviceInfo_t &info);

	virtual void ChangeVideoMode(const ShaderDeviceInfo_t &info);

	// Returns the snapshot id for the shader state
	virtual StateSnapshot_t	 TakeSnapshot();

	virtual void TexMinFilter(ShaderTexFilterMode_t texFilterMode);
	virtual void TexMagFilter(ShaderTexFilterMode_t texFilterMode);
	virtual void TexWrap(ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode);

	virtual void CopyRenderTargetToTexture(ShaderAPITextureHandle_t textureHandle);

	// Binds a particular material to render with
	virtual void Bind(IMaterial* pMaterial);

	// Flushes any primitives that are buffered
	virtual void FlushBufferedPrimitives();

	// Gets the dynamic mesh; note that you've got to render the mesh
	// before calling this function a second time. Clients should *not*
	// call DestroyStaticMesh on the mesh returned by this call.
	virtual IMesh* GetDynamicMesh(IMaterial* pMaterial, int nHWSkinBoneCount, bool bBuffered = true,
		IMesh* pVertexOverride = NULL, IMesh* pIndexOverride = NULL);
	virtual IMesh* GetDynamicMeshEx(IMaterial* pMaterial, VertexFormat_t vertexFormat, int nHWSkinBoneCount,
		bool bBuffered = true, IMesh* pVertexOverride = NULL, IMesh* pIndexOverride = NULL);

	// Methods to ask about particular state snapshots
	virtual bool IsTranslucent(StateSnapshot_t id) const;
	virtual bool IsAlphaTested(StateSnapshot_t id) const;
	virtual bool UsesVertexAndPixelShaders(StateSnapshot_t id) const;
	virtual bool IsDepthWriteEnabled(StateSnapshot_t id) const;

	// Gets the vertex format for a set of snapshot ids
	virtual VertexFormat_t ComputeVertexFormat(int numSnapshots, StateSnapshot_t* pIds) const;

	// What fields in the vertex do we actually use?
	virtual VertexFormat_t ComputeVertexUsage(int numSnapshots, StateSnapshot_t* pIds) const;

	// Begins a rendering pass
	virtual void BeginPass(StateSnapshot_t snapshot);

	// Renders a single pass of a material
	virtual void RenderPass(int nPass, int nPassCount);

	// Set the number of bone weights
	virtual void SetNumBoneWeights(int numBones);

	// Sets the lights
	virtual void SetLight(int lightNum, const LightDesc_t& desc);

	// Lighting origin for the current model
	virtual void SetLightingOrigin(Vector vLightingOrigin);

	virtual void SetAmbientLight(float r, float g, float b);
	virtual void SetAmbientLightCube(Vector4D cube[6]);

	// The shade mode
	virtual void ShadeMode(ShaderShadeMode_t mode);

	// The cull mode
	virtual void CullMode(MaterialCullMode_t cullMode);

	// Force writes only when z matches. . . useful for stenciling things out
	// by rendering the desired Z values ahead of time.
	virtual void ForceDepthFuncEquals(bool bEnable);

	// Forces Z buffering to be on or off
	virtual void OverrideDepthEnable(bool bEnable, bool bDepthEnable);

	virtual void SetHeightClipZ(float z);
	virtual void SetHeightClipMode(enum MaterialHeightClipMode_t heightClipMode);

	virtual void SetClipPlane(int index, const float *pPlane);
	virtual void EnableClipPlane(int index, bool bEnable);

	// Put all the model matrices into vertex shader constants.
	virtual void SetSkinningMatrices();

	// Returns the nearest supported format
	virtual ImageFormat GetNearestSupportedFormat(ImageFormat fmt, bool bFilteringRequired = true) const;
	virtual ImageFormat GetNearestRenderTargetFormat(ImageFormat fmt) const;

	// When AA is enabled, render targets are not AA and require a separate
	// depth buffer.
	virtual bool DoRenderTargetsNeedSeparateDepthBuffer() const;

	// Texture management methods
	// For CreateTexture also see CreateTextures below
	virtual ShaderAPITextureHandle_t CreateTexture(
		int width,
		int height,
		int depth,
		ImageFormat dstImageFormat,
		int numMipLevels,
		int numCopies,
		int flags,
		const char *pDebugName,
		const char *pTextureGroupName);

	virtual void DeleteTexture(ShaderAPITextureHandle_t textureHandle);

	virtual ShaderAPITextureHandle_t CreateDepthTexture(
		ImageFormat renderTargetFormat,
		int width,
		int height,
		const char *pDebugName,
		bool bTexture);

	virtual bool IsTexture(ShaderAPITextureHandle_t textureHandle);
	virtual bool IsTextureResident(ShaderAPITextureHandle_t textureHandle);

	// Indicates we're going to be modifying this texture
	// TexImage2D, TexSubImage2D, TexWrap, TexMinFilter, and TexMagFilter
	// all use the texture specified by this function.
	virtual void ModifyTexture(ShaderAPITextureHandle_t textureHandle);

	virtual void TexImage2D(
		int level,
		int cubeFaceID,
		ImageFormat dstFormat,
		int zOffset,
		int width,
		int height,
		ImageFormat srcFormat,
		bool bSrcIsTiled,		// NOTE: for X360 only
		void *imageData);

	virtual void TexSubImage2D(
		int level,
		int cubeFaceID,
		int xOffset,
		int yOffset,
		int zOffset,
		int width,
		int height,
		ImageFormat srcFormat,
		int srcStride,
		bool bSrcIsTiled,		// NOTE: for X360 only
		void *imageData);

	virtual void TexImageFromVTF(IVTFTexture* pVTF, int iVTFFrame);

	// An alternate (and faster) way of writing image data
	// (locks the current Modify Texture). Use the pixel writer to write the data
	// after Lock is called
	// Doesn't work for compressed textures 
	virtual bool TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
		int width, int height, CPixelWriter& writer);
	virtual void TexUnlock();

	// These are bound to the texture
	virtual void TexSetPriority(int priority);

	// Sets the texture state
	virtual void BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle);

	// Set the render target to a texID.
	// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
	// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
	virtual void SetRenderTarget(ShaderAPITextureHandle_t colorTextureHandle = SHADER_RENDERTARGET_BACKBUFFER,
		ShaderAPITextureHandle_t depthTextureHandle = SHADER_RENDERTARGET_DEPTHBUFFER);

	// stuff that isn't to be used from within a shader
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth);
	virtual void ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat);
	virtual void ReadPixels(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *data, ImageFormat dstFormat, int nDstStride);

	virtual void FlushHardware();

	// Use this to begin and end the frame
	virtual void BeginFrame();
	virtual void EndFrame();

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode);
	virtual void SelectionBuffer(unsigned int* pBuffer, int size);
	virtual void ClearSelectionNames();
	virtual void LoadSelectionName(int name);
	virtual void PushSelectionName(int name);
	virtual void PopSelectionName();

	// Force the hardware to finish whatever it's doing
	virtual void ForceHardwareSync();

	// Used to clear the transition table when we know it's become invalid.
	virtual void ClearSnapshots();

	virtual void FogStart(float fStart);
	virtual void FogEnd(float fEnd);
	virtual void SetFogZ(float fogZ);
	// Scene fog state.
	virtual void SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b);
	virtual void SceneFogMode(MaterialFogMode_t fogMode);

	// Can we download textures?
	virtual bool CanDownloadTextures() const;

	virtual void ResetRenderState(bool bFullReset = true);

	// We use smaller dynamic VBs during level transitions, to free up memory
	virtual int  GetCurrentDynamicVBSize(void);
	virtual void DestroyVertexBuffers(bool bExitingLevel = false);

	virtual void EvictManagedResources();

	// Level of anisotropic filtering
	virtual void SetAnisotropicLevel(int nAnisotropyLevel);

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void SyncToken(const char *pToken);

	// Setup standard vertex shader constants (that don't change)
	// This needs to be called anytime that overbright changes.
	virtual void SetStandardVertexShaderConstants(float fOverbright);

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual ShaderAPIOcclusionQuery_t CreateOcclusionQueryObject(void);
	virtual void DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t);

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t);
	virtual void EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t);

	// OcclusionQuery_GetNumPixelsRendered
	//	Get the number of pixels rendered between begin and end on an earlier frame.
	//	Calling this in the same frame is a huge perf hit!
	// Returns iQueryResult:
	//	iQueryResult >= 0					-	iQueryResult is the number of pixels rendered
	//	OCCLUSION_QUERY_RESULT_PENDING		-	query results are not available yet
	//	OCCLUSION_QUERY_RESULT_ERROR		-	query failed
	// Use OCCLUSION_QUERY_FINISHED( iQueryResult ) to test if query finished.
	virtual int OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t hQuery, bool bFlush = false);

	virtual void SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture);

	virtual void ClearVertexAndPixelShaderRefCounts();
	virtual void PurgeUnusedVertexAndPixelShaders();

	// Called when the dx support level has changed
	virtual void DXSupportLevelChanged();

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a View transform for user clip planes. The PROJECTION
	// will be mutliplied against this instead of the normal VIEW matrix.
	virtual void EnableUserClipTransformOverride(bool bEnable);
	virtual void UserClipTransform(const VMatrix &worldToView);

	// ----------------------------------------------------------------------------------
	// Everything after this point added after HL2 shipped.
	// ----------------------------------------------------------------------------------

	// What fields in the morph do we actually use?
	virtual MorphFormat_t ComputeMorphFormat(int numSnapshots, StateSnapshot_t* pIds) const;

	// Set the render target to a texID.
	// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
	// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
	virtual void SetRenderTargetEx(int nRenderTargetID,
		ShaderAPITextureHandle_t colorTextureHandle = SHADER_RENDERTARGET_BACKBUFFER,
		ShaderAPITextureHandle_t depthTextureHandle = SHADER_RENDERTARGET_DEPTHBUFFER);

	virtual void CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t textureHandle, int nRenderTargetID, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL);
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ShaderAPITextureHandle_t textureHandle, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL);

	// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
	virtual void HandleDeviceLost();

	virtual void EnableLinearColorSpaceFrameBuffer(bool bEnable);

	// Lets the shader know about the full-screen texture so it can 
	virtual void SetFullScreenTextureHandle(ShaderAPITextureHandle_t h);

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value);
	virtual void SetIntRenderingParameter(int parm_number, int value);
	virtual void SetVectorRenderingParameter(int parm_number, Vector const &value);

	virtual float GetFloatRenderingParameter(int parm_number) const;
	virtual int GetIntRenderingParameter(int parm_number) const;
	virtual Vector GetVectorRenderingParameter(int parm_number) const;

	virtual void SetFastClipPlane(const float *pPlane);
	virtual void EnableFastClip(bool bEnable);

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices);

	// Returns the max number of vertices we can render for a given material
	virtual int GetMaxVerticesToRender(IMaterial *pMaterial);
	virtual int GetMaxIndicesToRender();

	// stencil methods
	virtual void SetStencilEnable(bool onoff);
	virtual void SetStencilFailOperation(StencilOperation_t op);
	virtual void SetStencilZFailOperation(StencilOperation_t op);
	virtual void SetStencilPassOperation(StencilOperation_t op);
	virtual void SetStencilCompareFunction(StencilComparisonFunction_t cmpfn);
	virtual void SetStencilReferenceValue(int ref);
	virtual void SetStencilTestMask(uint32 msk);
	virtual void SetStencilWriteMask(uint32 msk);
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value);

	// disables all local lights
	virtual void DisableAllLocalLights();
	virtual int CompareSnapshots(StateSnapshot_t snapshot0, StateSnapshot_t snapshot1);

	virtual IMesh *GetFlexMesh();

	virtual void SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture);

	virtual bool SupportsMSAAMode(int nMSAAMode);

	virtual bool OwnGPUResources(bool bEnable);

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ);

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char *szName);
	virtual void EndPIXEvent();
	virtual void SetPIXMarker(unsigned long color, const char *szName);

	// Enables and disables for Alpha To Coverage
	virtual void EnableAlphaToCoverage();
	virtual void DisableAlphaToCoverage();

	// Computes the vertex buffer pointers 
	virtual void ComputeVertexDescription(unsigned char* pBuffer, VertexFormat_t vertexFormat, MeshDesc_t& desc) const;

	virtual bool SupportsShadowDepthTextures(void);

	virtual void SetDisallowAccess(bool);
	virtual void EnableShaderShaderMutex(bool);
	virtual void ShaderLock();
	virtual void ShaderUnlock();

	virtual ImageFormat GetShadowDepthTextureFormat(void);

	virtual bool SupportsFetch4(void);
	virtual void SetShadowDepthBiasFactors(float fShadowSlopeScaleDepthBias, float fShadowDepthBias);

	// ------------ New Vertex/Index Buffer interface ----------------------------
	virtual void BindVertexBuffer(int nStreamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1);
	virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes);
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int nFirstIndex, int nIndexCount);
	void DrawMesh(IMesh *pMesh);
	// ------------ End ----------------------------


		// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void PerformFullScreenStencilOperation(void);

	virtual void SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor);

	// nVidia CSAA modes, different from SupportsMSAAMode()
	virtual bool SupportsCSAAMode(int nNumSamples, int nQualityLevel);

	//Notifies the shaderapi to invalidate the current set of delayed constants because we just finished a draw pass. Either actual or not.
	virtual void InvalidateDelayedShaderConstants(void);

	// Gamma<->Linear conversions according to the video hardware we're running on
	virtual float GammaToLinear_HardwareSpecific(float fGamma) const;
	virtual float LinearToGamma_HardwareSpecific(float fLinear) const;

	//Set's the linear->gamma conversion textures to use for this hardware for both srgb writes enabled and disabled(identity)
	virtual void SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t hSRGBWriteEnabledTexture, ShaderAPITextureHandle_t hIdentityTexture);

	virtual ImageFormat GetNullTextureFormat(void);

	virtual void BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle);

	// Enables hardware morphing
	virtual void EnableHWMorphing(bool bEnable);

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights);

	virtual void FogMaxDensity(float flMaxDensity);

	// Create a multi-frame texture (equivalent to calling "CreateTexture" multiple times, but more efficient)
	virtual void CreateTextures(
		ShaderAPITextureHandle_t *pHandles,
		int count,
		int width,
		int height,
		int depth,
		ImageFormat dstImageFormat,
		int numMipLevels,
		int numCopies,
		int flags,
		const char *pDebugName,
		const char *pTextureGroupName);

	virtual void AcquireThreadOwnership();
	virtual void ReleaseThreadOwnership();

	virtual bool SupportsNormalMapCompression() const;

	// Only does anything on XBox360. This is useful to eliminate stalls
	virtual void EnableBuffer2FramesAhead(bool bEnable);

	// debug logging
	// only implemented in some subclasses
	virtual void PrintfVA(char *fmt, va_list vargs);
	virtual void Printf(PRINTF_FORMAT_STRING const char *fmt, ...);
	virtual float Knob(char *knobname, float *setvalue = NULL);
	// Allows us to override the alpha write setting of a material
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable);
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable);

	//extended clear buffers function with alpha independent from color
	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth);

	// Allows copying a render target to another texture by specifying them both.
	virtual void CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL);

	// Allows locking and unlocking of very specific surface types.
	virtual void LockRect(void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead);
	virtual void UnlockRect(ShaderAPITextureHandle_t texHandle, int mipmap);

	// ------------------------------------------------------- //
	//                      IDebugTextureInfo                  //
	// ------------------------------------------------------- //

	// Use this to turn on the mode where it builds the debug texture list.
	// At the end of the next frame, GetDebugTextureList() will return a valid list of the textures.
	virtual void EnableDebugTextureList(bool bEnable);

	// If this is on, then it will return all textures that exist, not just the ones that were bound in the last frame.
	// It is required to enable debug texture list to get this.
	virtual void EnableGetAllTextures(bool bEnable);

	// Use this to get the results of the texture list.
	// Do NOT release the KeyValues after using them.
	// There will be a bunch of subkeys, each with these values:
	//    Name   - the texture's filename
	//    Binds  - how many times the texture was bound
	//    Format - ImageFormat of the texture
	//    Width  - Width of the texture
	//    Height - Height of the texture
	// It is required to enable debug texture list to get this.
	virtual KeyValues* GetDebugTextureList();

	// This returns how much memory was used.
	virtual int GetTextureMemoryUsed(TextureMemoryType eTextureMemory);

	// Use this to determine if texture debug info was computed within last numFramesAllowed frames.
	virtual bool IsDebugTextureListFresh(int numFramesAllowed = 1);

	// Enable debug texture rendering when texture binds should not count towards textures
	// used during a frame. Returns the old state of debug texture rendering flag to use
	// it for restoring the mode.
	virtual bool SetDebugTextureRendering(bool bEnable);

public:
	void OnDeviceInitialised();
	void OnDeviceShutdown();

	void CreateTextureHandles(ShaderAPITextureHandle_t* pHandles, int count);

private:
	MaterialMatrixMode_t m_MatrixMode;
	DirectX::XMMATRIX *m_pCurMatrix;

	DynamicStateDX11 m_DynamicState;
	ShaderStateDX11 m_ShaderState;

	IMeshDX11 *m_pDynamicMesh;


	CUtlVector<CAPITextureDX11> m_vTextures;
	ShaderAPITextureHandle_t m_ModifyTextureHandle;

	ShaderAPITextureHandle_t m_BackBufferHandle;
};

#endif