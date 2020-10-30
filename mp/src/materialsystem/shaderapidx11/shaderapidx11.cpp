#include "shaderapidx11.h"
#include "shaderapidx11_global.h"


static CShaderAPIDX11 s_ShaderAPIDx11;
CShaderAPIDX11 *g_pShaderAPIDx11 = &s_ShaderAPIDx11;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IShaderAPI,
	SHADERAPI_INTERFACE_VERSION, s_ShaderAPIDx11)

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShaderAPIDX11, IDebugTextureInfo,
	DEBUG_TEXTURE_INFO_VERSION, s_ShaderAPIDx11)

// Globals
IShaderUtil* g_pShaderUtil = 0;
CShaderDeviceDX11 *g_pShaderDevice = 0;
CShaderDeviceMgrDX11 *g_pShaderDeviceMgr = 0;
CShaderAPIDX11 *g_pShaderAPI = 0;
CShaderShadowDX11 *g_pShaderShadow = 0;

CHardwareConfigDX11 *g_pHardwareConfig = 0;

CShaderAPIDX11::CShaderAPIDX11()
{
	_ldtmp = LightDesc_t();
	_vectmp = Vector(0, 0, 0);
}

void CShaderAPIDX11::SetViewports(int nCount, const ShaderViewport_t* pViewports) 
{
	return;
}

int CShaderAPIDX11::GetViewports(ShaderViewport_t* pViewports, int nMax) const
{
	return 0;
}

double CShaderAPIDX11::CurrentTime() const
{
	return 0.0;
}

void CShaderAPIDX11::GetLightmapDimensions(int *w, int *h)
{
	return;
}

MaterialFogMode_t CShaderAPIDX11::GetSceneFogMode()
{
	return MATERIAL_FOG_NONE;
}


void CShaderAPIDX11::GetSceneFogColor(unsigned char *rgb)
{
	return;
}

// stuff related to matrix stacks
void CShaderAPIDX11::MatrixMode(MaterialMatrixMode_t matrixMode)
{
	return;
}

void CShaderAPIDX11::PushMatrix()
{
	return;
}

void CShaderAPIDX11::PopMatrix()
{
	return;
}

void CShaderAPIDX11::LoadMatrix(float *m)
{
	return;
}

void CShaderAPIDX11::MultMatrix(float *m)
{
	return;
}

void CShaderAPIDX11::MultMatrixLocal(float *m)
{
	return;
}

void CShaderAPIDX11::GetMatrix(MaterialMatrixMode_t matrixMode, float *dst)
{
	return;
}

void CShaderAPIDX11::LoadIdentity(void)
{
	return;
}

void CShaderAPIDX11::LoadCameraToWorld(void)
{
	return;
}

void CShaderAPIDX11::Ortho(double left, double right, double bottom, double top, double zNear, double zFar)
{
	return;
}

void CShaderAPIDX11::PerspectiveX(double fovx, double aspect, double zNear, double zFar)
{
	return;
}

void CShaderAPIDX11::PickMatrix(int x, int y, int width, int height)
{
	return;
}

void CShaderAPIDX11::Rotate(float angle, float x, float y, float z)
{
	return;
}

void CShaderAPIDX11::Translate(float x, float y, float z)
{
	return;
}

void CShaderAPIDX11::Scale(float x, float y, float z)
{
	return;
}

void CShaderAPIDX11::ScaleXY(float x, float y)
{
	return;
}


// Sets the color to modulate by
void CShaderAPIDX11::Color3f(float r, float g, float b)
{
	return;
}

void CShaderAPIDX11::Color3fv(float const* pColor)
{
	return;
}

void CShaderAPIDX11::Color4f(float r, float g, float b, float a)
{
	return;
}

void CShaderAPIDX11::Color4fv(float const* pColor)
{
	return;
}


void CShaderAPIDX11::Color3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

void CShaderAPIDX11::Color3ubv(unsigned char const* pColor)
{
	return;
}

void CShaderAPIDX11::Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return;
}

void CShaderAPIDX11::Color4ubv(unsigned char const* pColor)
{
	return;
}


// Sets the constant register for vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderConstant(int var, float const* pVec, int numConst/* = 1*/, bool bForce/* = false*/)
{
	return;
}

void CShaderAPIDX11::SetPixelShaderConstant(int var, float const* pVec, int numConst/* = 1*/, bool bForce/* = false*/)
{
	return;
}


// Sets the default *dynamic* state
void CShaderAPIDX11::SetDefaultState()
{
	return;
}


// Get the current camera position in world space.
void CShaderAPIDX11::GetWorldSpaceCameraPosition(float* pPos) const
{
	return;
}


int CShaderAPIDX11::GetCurrentNumBones(void) const
{
	return 0;
}

int CShaderAPIDX11::GetCurrentLightCombo(void) const
{
	return 0;
}


MaterialFogMode_t CShaderAPIDX11::GetCurrentFogType(void) const
{
	return MATERIAL_FOG_NONE;
}


// fixme: move this to shadow state
void CShaderAPIDX11::SetTextureTransformDimension(TextureStage_t textureStage, int dimension, bool projected)
{
	return;
}

void CShaderAPIDX11::DisableTextureTransform(TextureStage_t textureStage)
{
	return;
}

void CShaderAPIDX11::SetBumpEnvMatrix(TextureStage_t textureStage, float m00, float m01, float m10, float m11)
{
	return;
}


// Sets the vertex and pixel shaders
void CShaderAPIDX11::SetVertexShaderIndex(int vshIndex/* = -1*/)
{
	return;
}

void CShaderAPIDX11::SetPixelShaderIndex(int pshIndex)
{
	return;
}


// Get the dimensions of the back buffer.
void CShaderAPIDX11::GetBackBufferDimensions(int& width, int& height) const
{
	return;
}


// FIXME: The following 6 methods used to live in IShaderAPI
// and were moved for stdshader_dx8. Let's try to move them back!

// Get the lights
int CShaderAPIDX11::GetMaxLights(void) const
{
	return 0;
}

const LightDesc_t& CShaderAPIDX11::GetLight(int lightNum) const
{
	return _ldtmp;
}


void CShaderAPIDX11::SetPixelShaderFogParams(int reg)
{
	return;
}


// Render state for the ambient light cube
void CShaderAPIDX11::SetVertexShaderStateAmbientLightCube()
{
	return;
}

void CShaderAPIDX11::SetPixelShaderStateAmbientLightCube(int pshReg, bool bForceToBlack/* = false*/)
{
	return;
}

void CShaderAPIDX11::CommitPixelShaderLighting(int pshReg)
{
	return;
}


// Use this to get the mesh builder that allows us to modify vertex data
CMeshBuilder* CShaderAPIDX11::GetVertexModifyBuilder()
{
	return (CMeshBuilder *)0;
}

bool CShaderAPIDX11::InFlashlightMode() const
{
	return false;
}

const FlashlightState_t &CShaderAPIDX11::GetFlashlightState(VMatrix &worldToTexture) const
{
	return (FlashlightState_t &)worldToTexture;
}

bool CShaderAPIDX11::InEditorMode() const
{
	return false;
}


// Gets the bound morph's vertex format; returns 0 if no morph is bound
MorphFormat_t CShaderAPIDX11::GetBoundMorphFormat()
{
	return 0;
}


// Binds a standard texture
void CShaderAPIDX11::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	return;
}


ITexture *CShaderAPIDX11::GetRenderTargetEx(int nRenderTargetID)
{
	return (ITexture *)0;
}


void CShaderAPIDX11::SetToneMappingScaleLinear(const Vector &scale)
{
	return;
}

const Vector &CShaderAPIDX11::GetToneMappingScaleLinear(void) const
{
	return _vectmp;
}

float CShaderAPIDX11::GetLightMapScaleFactor(void) const
{
	return 0.f;
}


void CShaderAPIDX11::LoadBoneMatrix(int boneIndex, const float *m)
{
	return;
}


void CShaderAPIDX11::PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right)
{
	return;
}


void CShaderAPIDX11::GetDXLevelDefaults(uint &max_dxlevel, uint &recommended_dxlevel)
{
	return;
}


const FlashlightState_t &CShaderAPIDX11::GetFlashlightStateEx(VMatrix &worldToTexture, ITexture **pFlashlightDepthTexture) const
{
	return (FlashlightState_t &)worldToTexture;
}


float CShaderAPIDX11::GetAmbientLightCubeLuminance()
{
	return 0.f;
}


void CShaderAPIDX11::GetDX9LightState(LightState_t *state) const
{
	return;
}

int CShaderAPIDX11::GetPixelFogCombo() //0 is either range fog, or no fog simulated with rigged range fog values. 1 is height fog
{
	return 0;
}


void CShaderAPIDX11::BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id)
{
	return;
}


// Is hardware morphing enabled?
bool CShaderAPIDX11::IsHWMorphingEnabled() const
{
	return false;
}


void CShaderAPIDX11::GetStandardTextureDimensions(int *pWidth, int *pHeight, StandardTextureId_t id)
{
	return;
}


void CShaderAPIDX11::SetBooleanVertexShaderConstant(int var, BOOL const* pVec, int numBools/* = 1*/, bool bForce/* = false*/)
{
	return;
}

void CShaderAPIDX11::SetIntegerVertexShaderConstant(int var, int const* pVec, int numIntVecs/* = 1*/, bool bForce/* = false*/)
{
	return;
}

void CShaderAPIDX11::SetBooleanPixelShaderConstant(int var, BOOL const* pVec, int numBools/* = 1*/, bool bForce/* = false*/)
{
	return;
}

void CShaderAPIDX11::SetIntegerPixelShaderConstant(int var, int const* pVec, int numIntVecs/* = 1*/, bool bForce/* = false*/)
{
	return;
}


//Are we in a configuration that needs access to depth data through the alpha channel later?
bool CShaderAPIDX11::ShouldWriteDepthToDestAlpha(void) const
{
	return false;
}



// deformations
void CShaderAPIDX11::PushDeformation(DeformationBase_t const *Deformation)
{
	return;
}

void CShaderAPIDX11::PopDeformation()
{
	return;
}

int CShaderAPIDX11::GetNumActiveDeformations() const
{
	return 0;
}



// for shaders to set vertex shader constants. returns a packed state which can be used to set
// the dynamic combo. returns # of active deformations
int CShaderAPIDX11::GetPackedDeformationInformation(int nMaskOfUnderstoodDeformations,
	float *pConstantValuesOut,
	int nBufferSize,
	int nMaximumDeformations,
	int *pNumDefsOut) const
{
	return 0;
}


// This lets the lower level system that certain vertex fields requested 
// in the shadow state aren't actually being read given particular state
// known only at dynamic state time. It's here only to silence warnings.
void CShaderAPIDX11::MarkUnusedVertexFields(unsigned int nFlags, int nTexCoordCount, bool *pUnusedTexCoords)
{
	return;
}



void CShaderAPIDX11::ExecuteCommandBuffer(uint8 *pCmdBuffer)
{
	return;
}


// interface for mat system to tell shaderapi about standard texture handles
void CShaderAPIDX11::SetStandardTextureHandle(StandardTextureId_t nId, ShaderAPITextureHandle_t nHandle)
{
	return;
}


// Interface for mat system to tell shaderapi about color correction
void CShaderAPIDX11::GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* pInfo)
{
	return;
}


void CShaderAPIDX11::SetPSNearAndFarZ(int pshReg)
{
	return;
}


// Buffer clearing
void CShaderAPIDX11::ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil, int renderTargetWidth, int renderTargetHeight)
{
	return;
}

void CShaderAPIDX11::ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

void CShaderAPIDX11::ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return;
}


// Methods related to binding shaders
void CShaderAPIDX11::BindVertexShader(VertexShaderHandle_t hVertexShader)
{
	return;
}

void CShaderAPIDX11::BindGeometryShader(GeometryShaderHandle_t hGeometryShader)
{
	return;
}

void CShaderAPIDX11::BindPixelShader(PixelShaderHandle_t hPixelShader)
{
	return;
}


// Methods related to state objects
void CShaderAPIDX11::SetRasterState(const ShaderRasterState_t& state)
{
	return;
}


//
// NOTE: These methods have not yet been ported to DX10
//

// Sets the mode...
bool CShaderAPIDX11::SetMode(void* hwnd, int nAdapter, const ShaderDeviceInfo_t &info)
{
	return false;
}


void CShaderAPIDX11::ChangeVideoMode(const ShaderDeviceInfo_t &info)
{
	return;
}


// Returns the snapshot id for the shader state
StateSnapshot_t	CShaderAPIDX11::TakeSnapshot()
{
	return StateSnapshot_t();
}


void CShaderAPIDX11::TexMinFilter(ShaderTexFilterMode_t texFilterMode)
{
	return;
}

void CShaderAPIDX11::TexMagFilter(ShaderTexFilterMode_t texFilterMode)
{
	return;
}

void CShaderAPIDX11::TexWrap(ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode)
{
	return;
}


void CShaderAPIDX11::CopyRenderTargetToTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Binds a particular material to render with
void CShaderAPIDX11::Bind(IMaterial* pMaterial)
{
	return;
}


// Flushes any primitives that are buffered
void CShaderAPIDX11::FlushBufferedPrimitives()
{
	return;
}


// Gets the dynamic mesh { } note that you've got to render the mesh
// before calling this function a second time. Clients should *not*
// call DestroyStaticMesh on the mesh returned by this call.
IMesh* CShaderAPIDX11::GetDynamicMesh(IMaterial* pMaterial, int nHWSkinBoneCount, bool bBuffered/* = true*/,
	IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	return (IMesh *)0;
}

IMesh* CShaderAPIDX11::GetDynamicMeshEx(IMaterial* pMaterial, VertexFormat_t vertexFormat, int nHWSkinBoneCount,
	bool bBuffered/* = true*/, IMesh* pVertexOverride, IMesh* pIndexOverride)
{
	return (IMesh *)0;
}


// Methods to ask about particular state snapshots
bool CShaderAPIDX11::IsTranslucent(StateSnapshot_t id) const
{
	return false;
}

bool CShaderAPIDX11::IsAlphaTested(StateSnapshot_t id) const
{
	return false;
}

bool CShaderAPIDX11::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	return false;
}

bool CShaderAPIDX11::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	return false;
}


// Gets the vertex format for a set of snapshot ids
VertexFormat_t CShaderAPIDX11::ComputeVertexFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	return (VertexFormat_t)0;
}


// What fields in the vertex do we actually use?
VertexFormat_t CShaderAPIDX11::ComputeVertexUsage(int numSnapshots, StateSnapshot_t* pIds) const
{
	return (VertexFormat_t)0;
}


// Begins a rendering pass
void CShaderAPIDX11::BeginPass(StateSnapshot_t snapshot)
{
	return;
}


// Renders a single pass of a material
void CShaderAPIDX11::RenderPass(int nPass, int nPassCount)
{
	return;
}


// Set the number of bone weights
void CShaderAPIDX11::SetNumBoneWeights(int numBones)
{
	return;
}


// Sets the lights
void CShaderAPIDX11::SetLight(int lightNum, const LightDesc_t& desc)
{
	return;
}


// Lighting origin for the current model
void CShaderAPIDX11::SetLightingOrigin(Vector vLightingOrigin)
{
	return;
}


void CShaderAPIDX11::SetAmbientLight(float r, float g, float b)
{
	return;
}

void CShaderAPIDX11::SetAmbientLightCube(Vector4D cube[6])
{
	return;
}


// The shade mode
void CShaderAPIDX11::ShadeMode(ShaderShadeMode_t mode)
{
	return;
}


// The cull mode
void CShaderAPIDX11::CullMode(MaterialCullMode_t cullMode)
{
	return;
}


// Force writes only when z matches. . . useful for stenciling things out
// by rendering the desired Z values ahead of time.
void CShaderAPIDX11::ForceDepthFuncEquals(bool bEnable)
{
	return;
}


// Forces Z buffering to be on or off
void CShaderAPIDX11::OverrideDepthEnable(bool bEnable, bool bDepthEnable)
{
	return;
}


void CShaderAPIDX11::SetHeightClipZ(float z)
{
	return;
}

void CShaderAPIDX11::SetHeightClipMode(enum MaterialHeightClipMode_t heightClipMode)
{
	return;
}


void CShaderAPIDX11::SetClipPlane(int index, const float *pPlane)
{
	return;
}

void CShaderAPIDX11::EnableClipPlane(int index, bool bEnable)
{
	return;
}


// Put all the model matrices into vertex shader constants.
void CShaderAPIDX11::SetSkinningMatrices()
{
	return;
}


// Returns the nearest supported format
ImageFormat CShaderAPIDX11::GetNearestSupportedFormat(ImageFormat fmt, bool bFilteringRequired/* = true*/) const
{
	return IMAGE_FORMAT_A8;
}

ImageFormat CShaderAPIDX11::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	return IMAGE_FORMAT_A8;
}


// When AA is enabled, render targets are not AA and require a separate
// depth buffer.
bool CShaderAPIDX11::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	return false;
}


// Texture management methods
// For CreateTexture also see CreateTextures below
ShaderAPITextureHandle_t CShaderAPIDX11::CreateTexture(
	int width,
	int height,
	int depth,
	ImageFormat dstImageFormat,
	int numMipLevels,
	int numCopies,
	int flags,
	const char *pDebugName,
	const char *pTextureGroupName)
{
	return ShaderAPITextureHandle_t();
}


void CShaderAPIDX11::DeleteTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}


ShaderAPITextureHandle_t CShaderAPIDX11::CreateDepthTexture(
	ImageFormat renderTargetFormat,
	int width,
	int height,
	const char *pDebugName,
	bool bTexture)
{
	return ShaderAPITextureHandle_t();
}


bool CShaderAPIDX11::IsTexture(ShaderAPITextureHandle_t textureHandle)
{
	return false;
}

bool CShaderAPIDX11::IsTextureResident(ShaderAPITextureHandle_t textureHandle)
{
	return false;
}


// Indicates we're going to be modifying this texture
// TexImage2D, TexSubImage2D, TexWrap, TexMinFilter, and TexMagFilter
// all use the texture specified by this function.
void CShaderAPIDX11::ModifyTexture(ShaderAPITextureHandle_t textureHandle)
{
	return;
}


void CShaderAPIDX11::TexImage2D(
	int level,
	int cubeFaceID,
	ImageFormat dstFormat,
	int zOffset,
	int width,
	int height,
	ImageFormat srcFormat,
	bool bSrcIsTiled,		// NOTE: for X360 only
	void *imageData)
{
	return;
}


void CShaderAPIDX11::TexSubImage2D(
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
	void *imageData)
{
	return;
}

void CShaderAPIDX11::TexImageFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	return;
}


// An alternate (and faster) way of writing image data
// (locks the current Modify Texture). Use the pixel writer to write the data
// after Lock is called
// Doesn't work for compressed textures 
bool CShaderAPIDX11::TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
	int width, int height, CPixelWriter& writer)
{
	return false;
}

void CShaderAPIDX11::TexUnlock()
{
	return;
}


// These are bound to the texture
void CShaderAPIDX11::TexSetPriority(int priority)
{
	return;
}


// Sets the texture state
void CShaderAPIDX11::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTarget(ShaderAPITextureHandle_t colorTextureHandle/* = SHADER_RENDERTARGET_BACKBUFFER*/,
	ShaderAPITextureHandle_t depthTextureHandle/* = SHADER_RENDERTARGET_DEPTHBUFFER*/)
{
	return;
}


// stuff that isn't to be used from within a shader
void CShaderAPIDX11::ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth)
{
	return;
}

void CShaderAPIDX11::ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat)
{
	return;
}

void CShaderAPIDX11::ReadPixels(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *data, ImageFormat dstFormat, int nDstStride)
{
	return;
}


void CShaderAPIDX11::FlushHardware()
{
	return;
}


// Use this to begin and end the frame
void CShaderAPIDX11::BeginFrame()
{
	return;
}

void CShaderAPIDX11::EndFrame()
{
	return;
}


// Selection mode methods
int CShaderAPIDX11::SelectionMode(bool selectionMode)
{
	return 0;
}

void CShaderAPIDX11::SelectionBuffer(unsigned int* pBuffer, int size)
{
	return;
}

void CShaderAPIDX11::ClearSelectionNames()
{
	return;
}

void CShaderAPIDX11::LoadSelectionName(int name)
{
	return;
}

void CShaderAPIDX11::PushSelectionName(int name)
{
	return;
}

void CShaderAPIDX11::PopSelectionName()
{
	return;
}


// Force the hardware to finish whatever it's doing
void CShaderAPIDX11::ForceHardwareSync()
{
	return;
}


// Used to clear the transition table when we know it's become invalid.
void CShaderAPIDX11::ClearSnapshots()
{
	return;
}


void CShaderAPIDX11::FogStart(float fStart)
{
	return;
}

void CShaderAPIDX11::FogEnd(float fEnd)
{
	return;
}

void CShaderAPIDX11::SetFogZ(float fogZ)
{
	return;
}

// Scene fog state.
void CShaderAPIDX11::SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	return;
}

void CShaderAPIDX11::SceneFogMode(MaterialFogMode_t fogMode)
{
	return;
}


// Can we download textures?
bool CShaderAPIDX11::CanDownloadTextures() const
{
	return false;
}


void CShaderAPIDX11::ResetRenderState(bool bFullReset/* = true*/)
{
	return;
}


// We use smaller dynamic VBs during level transitions, to free up memory
int CShaderAPIDX11::GetCurrentDynamicVBSize(void)
{
	return 0;
}

void CShaderAPIDX11::DestroyVertexBuffers(bool bExitingLevel/* = false*/)
{
	return;
}


void CShaderAPIDX11::EvictManagedResources()
{
	return;
}


// Level of anisotropic filtering
void CShaderAPIDX11::SetAnisotropicLevel(int nAnisotropyLevel)
{
	return;
}


// For debugging and building recording files. This will stuff a token into the recording file,
// then someone doing a playback can watch for the token.
void CShaderAPIDX11::SyncToken(const char *pToken)
{
	return;
}


// Setup standard vertex shader constants (that don't change)
// This needs to be called anytime that overbright changes.
void CShaderAPIDX11::SetStandardVertexShaderConstants(float fOverbright)
{
	return;
}


//
// Occlusion query support
//

// Allocate and delete query objects.
ShaderAPIOcclusionQuery_t CShaderAPIDX11::CreateOcclusionQueryObject(void)
{
	return INVALID_SHADERAPI_OCCLUSION_QUERY_HANDLE;
}

void CShaderAPIDX11::DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t)
{
	return;
}


// Bracket drawing with begin and end so that we can get counts next frame.
void CShaderAPIDX11::BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	return;
}

void CShaderAPIDX11::EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t)
{
	return;
}


// OcclusionQuery_GetNumPixelsRendered
//	Get the number of pixels rendered between begin and end on an earlier frame.
//	Calling this in the same frame is a huge perf hit!
// Returns iQueryResult:
//	iQueryResult >= 0					-	iQueryResult is the number of pixels rendered
//	OCCLUSION_QUERY_RESULT_PENDING		-	query results are not available yet
//	OCCLUSION_QUERY_RESULT_ERROR		-	query failed
// Use OCCLUSION_QUERY_FINISHED( iQueryResult ) to test if query finished.
int CShaderAPIDX11::OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t hQuery, bool bFlush/* = false*/)
{
	return 0;
}


void CShaderAPIDX11::SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture)
{
	return;
}


void CShaderAPIDX11::ClearVertexAndPixelShaderRefCounts()
{
	return;
}

void CShaderAPIDX11::PurgeUnusedVertexAndPixelShaders()
{
	return;
}


// Called when the dx support level has changed
void CShaderAPIDX11::DXSupportLevelChanged()
{
	return;
}


// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
// planes (which are specified in world space) to generate projection-space user clip planes
// Occasionally (for the particle system in hl2, for example), we want to override that
// behavior and explictly specify a View transform for user clip planes. The PROJECTION
// will be mutliplied against this instead of the normal VIEW matrix.
void CShaderAPIDX11::EnableUserClipTransformOverride(bool bEnable)
{
	return;
}

void CShaderAPIDX11::UserClipTransform(const VMatrix &worldToView)
{
	return;
}


// ----------------------------------------------------------------------------------
// Everything after this point added after HL2 shipped.
// ----------------------------------------------------------------------------------

// What fields in the morph do we actually use?
MorphFormat_t CShaderAPIDX11::ComputeMorphFormat(int numSnapshots, StateSnapshot_t* pIds) const
{
	return MorphFormat_t();
}


// Set the render target to a texID.
// Set to SHADER_RENDERTARGET_BACKBUFFER if you want to use the regular framebuffer.
// Set to SHADER_RENDERTARGET_DEPTHBUFFER if you want to use the regular z buffer.
void CShaderAPIDX11::SetRenderTargetEx(int nRenderTargetID,
	ShaderAPITextureHandle_t colorTextureHandle/* = SHADER_RENDERTARGET_BACKBUFFER*/,
	ShaderAPITextureHandle_t depthTextureHandle/* = SHADER_RENDERTARGET_DEPTHBUFFER*/)
{
	return;
}


void CShaderAPIDX11::CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t textureHandle, int nRenderTargetID, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}

void CShaderAPIDX11::CopyTextureToRenderTargetEx(int nRenderTargetID, ShaderAPITextureHandle_t textureHandle, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}


// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
void CShaderAPIDX11::HandleDeviceLost()
{
	return;
}


void CShaderAPIDX11::EnableLinearColorSpaceFrameBuffer(bool bEnable)
{
	return;
}


// Lets the shader know about the full-screen texture so it can 
void CShaderAPIDX11::SetFullScreenTextureHandle(ShaderAPITextureHandle_t h)
{
	return;
}


// Rendering parameters control special drawing modes withing the material system, shader
// system, shaders, and engine. renderparm.h has their definitions.
void CShaderAPIDX11::SetFloatRenderingParameter(int parm_number, float value)
{
	return;
}

void CShaderAPIDX11::SetIntRenderingParameter(int parm_number, int value)
{
	return;
}

void CShaderAPIDX11::SetVectorRenderingParameter(int parm_number, Vector const &value)
{
	return;
}


float CShaderAPIDX11::GetFloatRenderingParameter(int parm_number) const
{
	return 0.f;
}

int CShaderAPIDX11::GetIntRenderingParameter(int parm_number) const
{
	return 0;
}

Vector CShaderAPIDX11::GetVectorRenderingParameter(int parm_number) const
{
	return Vector(0, 0, 0);
}


void CShaderAPIDX11::SetFastClipPlane(const float *pPlane)
{
	return;
}

void CShaderAPIDX11::EnableFastClip(bool bEnable)
{
	return;
}


// Returns the number of vertices + indices we can render using the dynamic mesh
// Passing true in the second parameter will return the max # of vertices + indices
// we can use before a flush is provoked and may return different values 
// if called multiple times in succession. 
// Passing false into the second parameter will return
// the maximum possible vertices + indices that can be rendered in a single batch
void CShaderAPIDX11::GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices)
{
	return;
}


// Returns the max number of vertices we can render for a given material
int CShaderAPIDX11::GetMaxVerticesToRender(IMaterial *pMaterial)
{
	return 0;
}

int CShaderAPIDX11::GetMaxIndicesToRender()
{
	return 0;
}


// stencil methods
void CShaderAPIDX11::SetStencilEnable(bool onoff)
{
	return;
}

void CShaderAPIDX11::SetStencilFailOperation(StencilOperation_t op)
{
	return;
}

void CShaderAPIDX11::SetStencilZFailOperation(StencilOperation_t op)
{
	return;
}

void CShaderAPIDX11::SetStencilPassOperation(StencilOperation_t op)
{
	return;
}

void CShaderAPIDX11::SetStencilCompareFunction(StencilComparisonFunction_t cmpfn)
{
	return;
}

void CShaderAPIDX11::SetStencilReferenceValue(int ref)
{
	return;
}

void CShaderAPIDX11::SetStencilTestMask(uint32 msk)
{
	return;
}

void CShaderAPIDX11::SetStencilWriteMask(uint32 msk)
{
	return;
}

void CShaderAPIDX11::ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value)
{
	return;
}


// disables all local lights
void CShaderAPIDX11::DisableAllLocalLights()
{
	return;
}

int CShaderAPIDX11::CompareSnapshots(StateSnapshot_t snapshot0, StateSnapshot_t snapshot1)
{
	return 0;
}


IMesh *CShaderAPIDX11::GetFlexMesh()
{
	return (IMesh *)0;
}


void CShaderAPIDX11::SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture)
{
	return;
}


bool CShaderAPIDX11::SupportsMSAAMode(int nMSAAMode)
{
	return false;
}


#if defined( _X360 )
HXUIFONT CShaderAPIDX11::OpenTrueTypeFont(const char *pFontname, int tall, int style)
{
	return (HXUIFONT)0;
}

void CShaderAPIDX11::CloseTrueTypeFont(HXUIFONT hFont)
{
	return;
}

bool CShaderAPIDX11::GetTrueTypeFontMetrics(HXUIFONT hFont, XUIFontMetrics *pFontMetrics, XUICharMetrics charMetrics[256])
{
	return false;
}

// Render a sequence of characters and extract the data into a buffer
// For each character, provide the width+height of the font texture subrect,
// an offset to apply when rendering the glyph, and an offset into a buffer to receive the RGBA data
bool CShaderAPIDX11::GetTrueTypeGlyphs(HXUIFONT hFont, int numChars, wchar_t *pWch, int *pOffsetX, int *pOffsetY, int *pWidth, int *pHeight, unsigned char *pRGBA, int *pRGBAOffset)
{
	return false;
}

ShaderAPITextureHandle_t CShaderAPIDX11::CreateRenderTargetSurface(int width, int height, ImageFormat format, const char *pDebugName, const char *pTextureGroupName)
{
	return INVALID_SHADERAPI_TEXTURE_HANDLE;
}

void CShaderAPIDX11::PersistDisplay()
{
	return;
}

bool CShaderAPIDX11::PostQueuedTexture(const void CShaderAPIDX11::*pData, int nSize, ShaderAPITextureHandle_t *pHandles, int nHandles, int nWidth, int nHeight, int nDepth, int nMips, int *pRefCount)
{
	return false;
}

void *CShaderAPIDX11::GetD3DDevice()
{
	return;
}


void CShaderAPIDX11::PushVertexShaderGPRAllocation(int iVertexShaderCount = 64)
{
	return;
}

void CShaderAPIDX11::PopVertexShaderGPRAllocation(void)
{
	return;
}


void CShaderAPIDX11::EnableVSync_360(bool bEnable) //360 allows us to bypass vsync blocking up to 60 fps without creating a new device
{
	return;
}

#endif

bool CShaderAPIDX11::OwnGPUResources(bool bEnable)
{
	return false;
}


//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
void CShaderAPIDX11::GetFogDistances(float *fStart, float *fEnd, float *fFogZ)
{
	return;
}


// Hooks for firing PIX events from outside the Material System...
void CShaderAPIDX11::BeginPIXEvent(unsigned long color, const char *szName)
{
	return;
}

void CShaderAPIDX11::EndPIXEvent()
{
	return;
}

void CShaderAPIDX11::SetPIXMarker(unsigned long color, const char *szName)
{
	return;
}


// Enables and disables for Alpha To Coverage
void CShaderAPIDX11::EnableAlphaToCoverage()
{
	return;
}

void CShaderAPIDX11::DisableAlphaToCoverage()
{
	return;
}


// Computes the vertex buffer pointers 
void CShaderAPIDX11::ComputeVertexDescription(unsigned char* pBuffer, VertexFormat_t vertexFormat, MeshDesc_t& desc) const
{
	return;
}


bool CShaderAPIDX11::SupportsShadowDepthTextures(void)
{
	return false;
}


void CShaderAPIDX11::SetDisallowAccess(bool)
{
	return;
}

void CShaderAPIDX11::EnableShaderShaderMutex(bool)
{
	return;
}

void CShaderAPIDX11::ShaderLock()
{
	return;
}

void CShaderAPIDX11::ShaderUnlock()
{
	return;
}


ImageFormat CShaderAPIDX11::GetShadowDepthTextureFormat(void)
{
	return IMAGE_FORMAT_A8;
}


bool CShaderAPIDX11::SupportsFetch4(void)
{
	return false;
}

void CShaderAPIDX11::SetShadowDepthBiasFactors(float fShadowSlopeScaleDepthBias, float fShadowDepthBias)
{
	return;
}


// ------------ New Vertex/Index Buffer interface ----------------------------
void CShaderAPIDX11::BindVertexBuffer(int nStreamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions/* = 1*/)
{
	return;
}

void CShaderAPIDX11::BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes)
{
	return;
}

void CShaderAPIDX11::Draw(MaterialPrimitiveType_t primitiveType, int nFirstIndex, int nIndexCount)
{
	return;
}

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
void CShaderAPIDX11::PerformFullScreenStencilOperation(void)
{
	return;
}


void CShaderAPIDX11::SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor)
{
	return;
}


// nVidia CSAA modes, different from SupportsMSAAMode()
bool CShaderAPIDX11::SupportsCSAAMode(int nNumSamples, int nQualityLevel)
{
	return false;
}


//Notifies the shaderapi to invalidate the current set of delayed constants because we just finished a draw pass. Either actual or not.
void CShaderAPIDX11::InvalidateDelayedShaderConstants(void)
{
	return;
}


// Gamma<->Linear conversions according to the video hardware we're running on
float CShaderAPIDX11::GammaToLinear_HardwareSpecific(float fGamma) const
{
	return 0.f;
}

float CShaderAPIDX11::LinearToGamma_HardwareSpecific(float fLinear) const
{
	return 0.f;
}


//Set's the linear->gamma conversion textures to use for this hardware for both srgb writes enabled and disabled(identity)
void CShaderAPIDX11::SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t hSRGBWriteEnabledTexture, ShaderAPITextureHandle_t hIdentityTexture)
{
	return;
}


ImageFormat CShaderAPIDX11::GetNullTextureFormat(void)
{
	return IMAGE_FORMAT_A8;
}


void CShaderAPIDX11::BindVertexTexture(VertexTextureSampler_t nSampler, ShaderAPITextureHandle_t textureHandle)
{
	return;
}


// Enables hardware morphing
void CShaderAPIDX11::EnableHWMorphing(bool bEnable)
{
	return;
}


// Sets flexweights for rendering
void CShaderAPIDX11::SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights)
{
	return;
}


void CShaderAPIDX11::FogMaxDensity(float flMaxDensity)
{
	return;
}


// Create a multi-frame texture (equivalent to calling "CreateTexture" multiple times, but more efficient)
void CShaderAPIDX11::CreateTextures(
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
	const char *pTextureGroupName)
{
	return;
}


void CShaderAPIDX11::AcquireThreadOwnership()
{
	return;
}

void CShaderAPIDX11::ReleaseThreadOwnership()
{
	return;
}


bool CShaderAPIDX11::SupportsNormalMapCompression() const
{
	return false;
}


// Only does anything on XBox360. This is useful to eliminate stalls
void CShaderAPIDX11::EnableBuffer2FramesAhead(bool bEnable)
{
	return;
}


void CShaderAPIDX11::SetDepthFeatheringPixelShaderConstant(int iConstant, float fDepthBlendScale)
{
	return;
}


// debug logging
// only implemented in some subclasses
void CShaderAPIDX11::PrintfVA(char *fmt, va_list vargs)
{
	return;
}

void CShaderAPIDX11::Printf(PRINTF_FORMAT_STRING const char *fmt, ...)
{
	return;
}

float CShaderAPIDX11::Knob(char *knobname, float *setvalue/* = NULL*/)
{
	return 0.f;
}

// Allows us to override the alpha write setting of a material
void CShaderAPIDX11::OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable)
{
	return;
}

void CShaderAPIDX11::OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable)
{
	return;
}


//extended clear buffers function with alpha independent from color
void CShaderAPIDX11::ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth)
{
	return;
}


// Allows copying a render target to another texture by specifying them both.
void CShaderAPIDX11::CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect/* = NULL*/, Rect_t *pDstRect/* = NULL*/)
{
	return;
}


// Allows locking and unlocking of very specific surface types.
void CShaderAPIDX11::LockRect(void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead)
{
	return;
}

void CShaderAPIDX11::UnlockRect(ShaderAPITextureHandle_t texHandle, int mipmap)
{
	return;
}

// IDebugTextureInfo
// Use this to turn on the mode where it builds the debug texture list.
	// At the end of the next frame, GetDebugTextureList() will return a valid list of the textures.
void CShaderAPIDX11::EnableDebugTextureList(bool bEnable)
{
	return;
}

// If this is on, then it will return all textures that exist, not just the ones that were bound in the last frame.
// It is required to enable debug texture list to get this.
void CShaderAPIDX11::EnableGetAllTextures(bool bEnable)
{
	return;
}


// Use this to get the results of the texture list.
// Do NOT release the KeyValues after using them.
// There will be a bunch of subkeys, each with these values:
//    Name   - the texture's filename
//    Binds  - how many times the texture was bound
//    Format - ImageFormat of the texture
//    Width  - Width of the texture
//    Height - Height of the texture
// It is required to enable debug texture list to get this.
KeyValues* CShaderAPIDX11::GetDebugTextureList()
{
	return (KeyValues *)0;
}


// This returns how much memory was used.
int CShaderAPIDX11::GetTextureMemoryUsed(CShaderAPIDX11::TextureMemoryType eTextureMemory)
{
	return 0;
}


// Use this to determine if texture debug info was computed within last numFramesAllowed frames.
bool CShaderAPIDX11::IsDebugTextureListFresh(int numFramesAllowed/* = 1*/)
{
	return false;
}


// Enable debug texture rendering when texture binds should not count towards textures
// used during a frame. Returns the old state of debug texture rendering flag to use
// it for restoring the mode.
bool CShaderAPIDX11::SetDebugTextureRendering(bool bEnable)
{
	return false;
}

