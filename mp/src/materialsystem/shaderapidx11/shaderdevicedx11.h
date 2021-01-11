#ifndef SHADERDEVICEDX11_H
#define SHADERDEVICEDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "IShaderDevice.h"
#include "meshdx11.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#include "tier1/utllinkedlist.h"

// Implemented IShaderBuffer (shader memory block)

class CShaderBuffer : public IShaderBuffer
{
public:
	CShaderBuffer(ID3DBlob *pD3DBlob) : m_pD3DBlob(pD3DBlob) {}

	virtual size_t GetSize() const
	{
		if (!m_pD3DBlob)
			return 0;

		return m_pD3DBlob->GetBufferSize();
	}

	virtual const void* GetBits() const
	{
		if (!m_pD3DBlob)
			return NULL;

		return m_pD3DBlob->GetBufferPointer();
	}

	virtual void Release()
	{
		if (m_pD3DBlob)
		{
			m_pD3DBlob->Release();
		}
		
		delete this;
	}

private:
	ID3DBlob *m_pD3DBlob;

};

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

	inline ID3D11VertexShader *GetVertexShader(VertexShaderHandle_t handle)
	{
		if (handle == VERTEX_SHADER_HANDLE_INVALID)
			return NULL;

		return m_VertexShaders[(VertexShaderIndex_t)handle].m_pShader;
	}

	inline ID3D11GeometryShader *GetGeometryShader(GeometryShaderHandle_t handle)
	{
		if (handle == GEOMETRY_SHADER_HANDLE_INVALID)
			return NULL;

		return m_GeometryShaders[(GeometryShaderIndex_t)handle].m_pShader;
	}

	inline ID3D11PixelShader *GetPixelShader(PixelShaderHandle_t handle)
	{
		if (handle == PIXEL_SHADER_HANDLE_INVALID)
			return NULL;

		return m_PixelShaders[(PixelShaderIndex_t)handle].m_pShader;
	}

private:

	int m_nCurrentAdapter;
	bool m_bDeviceInitialized;

	IDXGISwapChain *m_pDXGISwapChain;
	IDXGIOutput *m_pDXGIOutput;
	ID3D11Device *m_pDXGIDevice;
	ID3D11DeviceContext* m_pDXGIDeviceContext;

	struct VertexShader_t
	{
		ID3D11VertexShader *m_pShader;
		ID3D11ShaderReflection *m_pReflection;
	};

	struct GeometryShader_t
	{
		ID3D11GeometryShader *m_pShader;
		ID3D11ShaderReflection *m_pReflection;
	};

	struct PixelShader_t
	{
		ID3D11PixelShader *m_pShader;
		ID3D11ShaderReflection *m_pReflection;
	};

	CUtlFixedLinkedList< VertexShader_t > m_VertexShaders;
	CUtlFixedLinkedList< GeometryShader_t > m_GeometryShaders;
	CUtlFixedLinkedList< PixelShader_t > m_PixelShaders;

	typedef CUtlFixedLinkedList< VertexShader_t >::IndexType_t VertexShaderIndex_t;
	typedef CUtlFixedLinkedList< GeometryShader_t >::IndexType_t GeometryShaderIndex_t;
	typedef CUtlFixedLinkedList< PixelShader_t >::IndexType_t PixelShaderIndex_t;

	void *m_CurrenthWnd;
	int m_nWndWidth, m_nWndHeight;

	friend class CShaderDeviceMgrDX11;

	CMeshDX11 m_Mesh;
	CMeshDX11 m_DynamicMesh;

};

// Singleton
extern CShaderDeviceDX11 *g_pShaderDeviceDX11;

#endif // SHADERDEVICEDX11_H