#ifndef SHADERDEVICEDX11_H
#define SHADERDEVICEDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishaderdevicedx11.h"
#include "utllinkedlist.h"
#include <windows.h>

class CShaderDeviceDX11;
extern CShaderDeviceDX11* g_pShaderDeviceDX11;

struct IDXGIOutput;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11ShaderReflection;
struct ID3D11InputLayout;

class CShaderDeviceDX11 : public IShaderDeviceDX11
{
public:
	CShaderDeviceDX11();

	virtual bool Init(void *hWnd, int nAdapter, const ShaderDeviceInfo_t mode);
	virtual void Shutdown();

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

	// Don't do this man we can use dxvk
#ifdef DX_TO_GL_ABSTRACTION
	virtual void DoStartupShaderPreloading(void);
#endif

	virtual char *GetDisplayDeviceName();

	virtual bool IsActivated() const;
public:

	inline ID3D11Device *GetDevice() const { return m_pD3DDevice; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return m_pD3DDeviceContext; }
	inline IDXGISwapChain* GetSwapChain() const { return m_pDXGISwapChain; }

	inline ID3D11VertexShader *GetVertexShader(VertexShaderHandle_t hVertexShader)
	{
		return m_VertexShaders[(VertexRepIndex_t)hVertexShader].m_pShader;
	}

	inline ID3D11PixelShader *GetPixelShader(PixelShaderHandle_t hPixelShader)
	{
		return m_PixelShaders[(PixelRepIndex_t)hPixelShader].m_pShader;
	}

	inline ID3D11InputLayout *GetInputLayout(VertexShaderHandle_t hVertexShader)
	{
		return m_VertexShaders[(VertexRepIndex_t)hVertexShader].m_pInputLayout;
	}

private:
	bool m_bDeviceInitialized;
	int m_nAdapter;

	IDXGIOutput* m_pDXGIOutput;

	IDXGISwapChain* m_pDXGISwapChain;
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DDeviceContext;

	HWND m_hWnd;

	struct VertexShaderRep_t
	{
		ID3D11VertexShader *m_pShader;
		ID3D11ShaderReflection *m_pReflection;
		ID3D11InputLayout *m_pInputLayout;
	};

	struct PixelShaderRep_t
	{
		ID3D11PixelShader *m_pShader;
		ID3D11ShaderReflection *m_pReflection;
	};

	CUtlFixedLinkedList<VertexShaderRep_t> m_VertexShaders;
	CUtlFixedLinkedList<PixelShaderRep_t> m_PixelShaders;

	typedef CUtlFixedLinkedList<VertexShaderRep_t>::IndexType_t VertexRepIndex_t;
	typedef CUtlFixedLinkedList<PixelShaderRep_t>::IndexType_t PixelRepIndex_t;

	friend class CShaderDeviceMgrDX11;
};

#endif