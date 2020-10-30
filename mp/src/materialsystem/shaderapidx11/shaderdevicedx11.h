#ifndef SHADERDEVICEDX11_H
#define SHADERDEVICEDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "IShaderDevice.h"
#include <d3d11.h>

class CShaderDeviceDX11 : public IShaderDevice
{
public:
	CShaderDeviceDX11();
	//~CShaderDeviceDX11();

	bool Initialize(void *hWnd, int nAdapter, const ShaderDeviceInfo_t mode);
	void Shutdown();

	// Releases/reloads resources when other apps want some memory
	virtual void ReleaseResources();
	virtual void ReacquireResources();

	// returns the backbuffer format and dimensions
	virtual ImageFormat GetBackBufferFormat() const;
	virtual void GetBackBufferDimensions(int& width, int& height) const;

	// Returns the current adapter in use
	virtual int GetCurrentAdapter() const;

	// Are we using graphics?
	virtual bool IsUsingGraphics() const;

	// Use this to spew information about the 3D layer 
	virtual void SpewDriverInfo() const;

	// What's the bit depth of the stencil buffer?
	virtual int StencilBufferBits() const;

	// Are we using a mode that uses MSAA
	virtual bool IsAAEnabled() const;

	// Does a page flip
	virtual void Present();

	// Returns the window size
	virtual void GetWindowSize(int &nWidth, int &nHeight) const;

	// Gamma ramp control
	virtual void SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled);

	// Creates/ destroys a child window
	virtual bool AddView(void* hWnd);
	virtual void RemoveView(void* hWnd);

	// Activates a view
	virtual void SetView(void* hWnd);

	// Shader compilation
	virtual IShaderBuffer* CompileShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion);

	// Shader creation, destruction
	virtual VertexShaderHandle_t CreateVertexShader(IShaderBuffer* pShaderBuffer);
	virtual void DestroyVertexShader(VertexShaderHandle_t hShader);
	virtual GeometryShaderHandle_t CreateGeometryShader(IShaderBuffer* pShaderBuffer);
	virtual void DestroyGeometryShader(GeometryShaderHandle_t hShader);
	virtual PixelShaderHandle_t CreatePixelShader(IShaderBuffer* pShaderBuffer);
	virtual void DestroyPixelShader(PixelShaderHandle_t hShader);

	// Utility methods to make shader creation simpler
	// NOTE: For the utlbuffer version, use a binary buffer for a compiled shader
	// and a text buffer for a source-code (.fxc) shader
	VertexShaderHandle_t CreateVertexShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion);
	VertexShaderHandle_t CreateVertexShader(CUtlBuffer &buf, const char *pShaderVersion = NULL);
	GeometryShaderHandle_t CreateGeometryShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion);
	GeometryShaderHandle_t CreateGeometryShader(CUtlBuffer &buf, const char *pShaderVersion = NULL);
	PixelShaderHandle_t CreatePixelShader(const char *pProgram, size_t nBufLen, const char *pShaderVersion);
	PixelShaderHandle_t CreatePixelShader(CUtlBuffer &buf, const char *pShaderVersion = NULL);

	// NOTE: Deprecated!! Use CreateVertexBuffer/CreateIndexBuffer instead
	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t vertexFormat, const char *pTextureBudgetGroup, IMaterial * pMaterial = NULL);
	virtual void DestroyStaticMesh(IMesh* mesh);

	// Creates/destroys static vertex + index buffers
	virtual IVertexBuffer *CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char *pBudgetGroup);
	virtual void DestroyVertexBuffer(IVertexBuffer *pVertexBuffer);

	virtual IIndexBuffer *CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char *pBudgetGroup);
	virtual void DestroyIndexBuffer(IIndexBuffer *pIndexBuffer);

	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer *GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered = true);
	virtual IIndexBuffer *GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true);

	// A special path used to tick the front buffer while loading on the 360
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t *pInfo = NULL);
	virtual void RefreshFrontBufferNonInteractive();
	virtual void HandleThreadEvent(uint32 threadEvent);

#ifdef DX_TO_GL_ABSTRACTION
	virtual void DoStartupShaderPreloading(void);
#endif
	virtual char *GetDisplayDeviceName();

private:

	int m_nCurrentAdapter;
	bool m_bDeviceInitialized;

	IDXGISwapChain *m_pDXGISwapChain;
	IDXGIOutput *m_pDXGIOutput;
	ID3D11Device *m_pDXGIDevice;
	ID3D11DeviceContext* m_pDXGIDeviceContext;

	void *m_CurrenthWnd;
	int m_nWndWidth, m_nWndHeight;

	friend class CShaderDeviceMgrDX11;

};

#endif // SHADERDEVICEDX11_H