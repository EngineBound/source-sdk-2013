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

	ID3D11Resource *InitTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags);

	void LoadFromVTF(IVTFTexture* pVTF, int iVTFFrame);

	static ImageFormat ResolveToSupportedFormat(ImageFormat fmt);
	static DXGI_FORMAT GetDXGIFormat(ImageFormat fmt);
	static ImageFormat GetImageFormat(DXGI_FORMAT fmt);
private:

	ID3D11Texture2D *m_pD3DTexture;
};

#endif