
#include "dx11global.h"
#include "apitexturedx11.h"
#include "ishaderapi.h"

#include "shaderdevicedx11.h"

extern CShaderDeviceDX11 *g_pShaderDeviceDX11;

CAPITextureDX11::CAPITextureDX11()
{
	m_pD3DTexture = NULL;
}

CAPITextureDX11::~CAPITextureDX11()
{
	if (m_pD3DTexture)
		m_pD3DTexture->Release();
}

ID3D11Resource *CAPITextureDX11::InitTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags)
{
	ImageFormat supportedFormat = ResolveToSupportedFormat(dstImageFormat);
	DXGI_FORMAT dxgiFormat = GetDXGIFormat(supportedFormat);

	UINT miscFlags = 0;
	UINT bindFlags = 0;
	UINT CPUAccessFlags = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;

	if (flags & TEXTURE_CREATE_CUBEMAP)
	{
		depth = 6;
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	if (flags & TEXTURE_CREATE_RENDERTARGET)
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	if (flags & TEXTURE_CREATE_DEPTHBUFFER)
	{
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}

	if (flags & TEXTURE_CREATE_DYNAMIC)
	{
		CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = depth;
	desc.Format = dxgiFormat;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = numMipLevels;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = CPUAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.Usage = usage;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	HRESULT hr = g_pShaderDeviceDX11->GetDevice()->CreateTexture2D(&desc, NULL, &m_pD3DTexture);

	Assert(!FAILED(hr));

	return m_pD3DTexture;
}

void CAPITextureDX11::LoadFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	_AssertMsg(0, "Not Implemented! " __FUNCTION__, 0, 0);
	return;

	unsigned char *imageData = pVTF->ImageData();

	g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DTexture, 0, NULL, imageData, 0, 0);
}

ImageFormat CAPITextureDX11::ResolveToSupportedFormat(ImageFormat fmt)
{
	switch (fmt)
	{
	case IMAGE_FORMAT_BGR888:
	case IMAGE_FORMAT_BGR888_BLUESCREEN: // SRGB
		return IMAGE_FORMAT_BGRA8888;

	case IMAGE_FORMAT_RGB888:
	case IMAGE_FORMAT_RGB888_BLUESCREEN: // SRGB
		return IMAGE_FORMAT_RGBA8888;

	default:
		return fmt;
	}
}

DXGI_FORMAT CAPITextureDX11::GetDXGIFormat(ImageFormat fmt)
{
	switch(fmt)
	{
	case IMAGE_FORMAT_RGBA8888:
	case IMAGE_FORMAT_ARGB8888:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case IMAGE_FORMAT_BGRA8888:
	case IMAGE_FORMAT_ABGR8888:
		return DXGI_FORMAT_B8G8R8A8_UNORM;

	case IMAGE_FORMAT_BGR565:
		return DXGI_FORMAT_B5G6R5_UNORM;

	case IMAGE_FORMAT_I8:
		return DXGI_FORMAT_R8_UNORM;
	case IMAGE_FORMAT_IA88:
		return DXGI_FORMAT_R8G8_UNORM;
	case IMAGE_FORMAT_P8:
		return DXGI_FORMAT_P8;
	case IMAGE_FORMAT_A8:
		return DXGI_FORMAT_A8_UNORM;
	
	case IMAGE_FORMAT_DXT1:
	case IMAGE_FORMAT_DXT1_ONEBITALPHA:
		return DXGI_FORMAT_BC1_UNORM;
	case IMAGE_FORMAT_DXT3:
		return DXGI_FORMAT_BC3_UNORM;
	case IMAGE_FORMAT_DXT5:
		return DXGI_FORMAT_BC5_UNORM;

	case IMAGE_FORMAT_BGRX8888:
		return DXGI_FORMAT_B8G8R8X8_UNORM;

	case IMAGE_FORMAT_BGRX5551:
	case IMAGE_FORMAT_BGRA5551:
		return DXGI_FORMAT_B5G5R5A1_UNORM;

	case IMAGE_FORMAT_BGRA4444:
		return DXGI_FORMAT_B4G4R4A4_UNORM;

	case IMAGE_FORMAT_UV88:
		return DXGI_FORMAT_R8G8_UNORM;

	case IMAGE_FORMAT_UVWQ8888:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case IMAGE_FORMAT_RGBA16161616F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;

	case IMAGE_FORMAT_RGBA16161616:
		return DXGI_FORMAT_R16G16B16A16_UNORM;

	case IMAGE_FORMAT_UVLX8888:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case IMAGE_FORMAT_R32F:
		return DXGI_FORMAT_R32_FLOAT;

	case IMAGE_FORMAT_RGB323232F:
		return DXGI_FORMAT_R32G32B32_FLOAT;

	case IMAGE_FORMAT_RGBA32323232F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case IMAGE_FORMAT_NV_DST16:
	case IMAGE_FORMAT_ATI_DST16:
		return DXGI_FORMAT_R16_TYPELESS;

	case IMAGE_FORMAT_NV_DST24:
	case IMAGE_FORMAT_ATI_DST24:
		return DXGI_FORMAT_R24G8_TYPELESS;

	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

ImageFormat CAPITextureDX11::GetImageFormat(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return IMAGE_FORMAT_RGBA8888;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return IMAGE_FORMAT_BGRA8888;

	case DXGI_FORMAT_B5G6R5_UNORM:
		return IMAGE_FORMAT_BGR565;

	case DXGI_FORMAT_R8_UNORM:
		return IMAGE_FORMAT_I8;
	case DXGI_FORMAT_R8G8_UNORM:
		return IMAGE_FORMAT_IA88;
	case DXGI_FORMAT_P8:
		return IMAGE_FORMAT_P8;
	case DXGI_FORMAT_A8_UNORM:
		return IMAGE_FORMAT_A8;

	case DXGI_FORMAT_BC1_UNORM:
		return IMAGE_FORMAT_DXT1;
	case DXGI_FORMAT_BC3_UNORM:
		return IMAGE_FORMAT_DXT3;
	case DXGI_FORMAT_BC5_UNORM:
		return IMAGE_FORMAT_DXT5;

	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return IMAGE_FORMAT_BGRX8888;

	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return IMAGE_FORMAT_BGRA5551;

	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return IMAGE_FORMAT_BGRA4444;

	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return IMAGE_FORMAT_RGBA16161616F;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return IMAGE_FORMAT_RGBA16161616;

	case DXGI_FORMAT_R32_FLOAT:
		return IMAGE_FORMAT_R32F;

	case DXGI_FORMAT_R32G32B32_FLOAT:
		return IMAGE_FORMAT_RGB323232F;

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return IMAGE_FORMAT_RGBA32323232F;

	case DXGI_FORMAT_R16_TYPELESS: // Check for vendor probly
		return IMAGE_FORMAT_NV_DST16;

	case DXGI_FORMAT_R24G8_TYPELESS:
		return IMAGE_FORMAT_NV_DST24;

	default:
		return IMAGE_FORMAT_UNKNOWN;
	};
}