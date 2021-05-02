#ifndef TEXTUREDX11_H
#define TEXTUREDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "bitmap/imageformat.h"
#include "vtf/vtf.h"
#include <d3d11.h>

class CAPITextureDX11
{
public:
	CAPITextureDX11();
	~CAPITextureDX11();

	void InitTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags);
	void InitRenderTarget(int width, int height, ID3D11Texture2D* pRenderTarget, ImageFormat dstImageFormat);

	void Shutdown();

	ID3D11Texture2D* CreateD3DTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int flags);
	void DestroyD3DTexture();


	void CreateRenderTargetView();

	void LoadFromVTF(IVTFTexture* pVTF, int iVTFFrame);

	static ImageFormat ResolveToSupportedFormat(ImageFormat fmt);
	static DXGI_FORMAT GetDXGIFormat(ImageFormat fmt);
	static ImageFormat GetImageFormat(DXGI_FORMAT fmt);

	inline ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }

	inline bool IsActivated() { return m_bIsInitialised; }
private:

	bool m_bIsInitialised;

	ID3D11Texture2D* m_pD3DTexture;
	ImageFormat m_APIFormat;
	DXGI_FORMAT m_DXGIFormat;

	ID3D11RenderTargetView* m_pRenderTargetView;
};

#endif