
#include "dx11global.h"
#include "apitexturedx11.h"
#include "ishaderapi.h"
#include "ishaderutil.h"

#include "shaderdevicedx11.h"

#include <d3d11.h>

extern CShaderDeviceDX11 *g_pShaderDeviceDX11;

CAPITextureDX11::CAPITextureDX11()
{
	m_pD3DTexture = NULL;
	m_pRenderTargetView = NULL;
	m_pSamplerState = NULL;
	m_pResourceView = NULL;

	m_APIFormat = IMAGE_FORMAT_UNKNOWN;
	m_DXGIFormat = DXGI_FORMAT_UNKNOWN;
	m_nNumMipLevels = 0;

	m_bIsInitialised = false;
}

CAPITextureDX11::~CAPITextureDX11()
{
	Shutdown();
}

void CAPITextureDX11::InitTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int numCopies, int flags)
{
	Assert(!m_bIsInitialised);

	if (width <= 0) width = 1;
	if (height <= 0) height = 1;
	if (depth <= 0) depth = 1;

	m_APIFormat = ResolveToSupportedFormat(dstImageFormat);
	m_DXGIFormat = GetDXGIFormat(m_APIFormat);

	if (flags & TEXTURE_CREATE_RENDERTARGET)
	{
		InitRenderTarget(width, height, NULL, dstImageFormat);
		return;
	}

	CreateD3DTexture(width, height, depth, dstImageFormat, numMipLevels, flags);
	CreateSamplerState();
	CreateResourceView();

	m_bIsInitialised = true;
}

void CAPITextureDX11::Shutdown()
{
	if (m_pD3DTexture)
	{
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	if (m_pResourceView)
	{
		m_pResourceView->Release();
		m_pResourceView = NULL;
	}

	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = NULL;
	}

	m_bIsInitialised = false;
}

ID3D11Texture2D *CAPITextureDX11::CreateD3DTexture(int width, int height, int depth, ImageFormat dstImageFormat, int numMipLevels, int flags)
{
	UINT miscFlags = 0;
	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	UINT CPUAccessFlags = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;

	bool bIsCubemap = (flags & TEXTURE_CREATE_CUBEMAP) != 0;
	bool bIsRenderTarget = (flags & TEXTURE_CREATE_RENDERTARGET) != 0;
	bool bIsDepthBuffer = (flags & TEXTURE_CREATE_DEPTHBUFFER) != 0;
	bool bIsDynamic = (flags & TEXTURE_CREATE_DYNAMIC) != 0;

	m_APIFormat = ResolveToSupportedFormat(dstImageFormat);
	m_DXGIFormat = GetDXGIFormat(m_APIFormat);

	if (m_DXGIFormat == DXGI_FORMAT_UNKNOWN)
	{
		Assert(0);
		return NULL;
	}

	m_nNumMipLevels = numMipLevels;

	if (bIsCubemap)
	{
		depth = 6;
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	if (bIsRenderTarget)
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	if (bIsDepthBuffer)
	{
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}

	if (bIsDynamic)
	{
		CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = depth;
	desc.Format = m_DXGIFormat;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = numMipLevels;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = CPUAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.Usage = usage;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
#ifdef DEBUG
	HRESULT hr = 
#endif
	g_pShaderDeviceDX11->GetDevice()->CreateTexture2D(&desc, NULL, &m_pD3DTexture);
	Assert(SUCCEEDED(hr));

	return m_pD3DTexture;
}

void CAPITextureDX11::DestroyD3DTexture()
{
	if (m_pD3DTexture)
		m_pD3DTexture->Release();
	m_pD3DTexture = NULL;
}

void CAPITextureDX11::InitRenderTarget(int width, int height, ID3D11Texture2D* pRenderTarget, ImageFormat dstImageFormat)
{
	m_APIFormat = ResolveToSupportedFormat(dstImageFormat);
	m_DXGIFormat = GetDXGIFormat(m_APIFormat);

	int flags = TEXTURE_CREATE_RENDERTARGET;

	if (pRenderTarget)
		m_pD3DTexture = pRenderTarget;
	else
		m_pD3DTexture = CreateD3DTexture(width, height, 1, m_APIFormat, 1, flags);

	CreateSamplerState();
	CreateRenderTargetView();
	CreateResourceView();

	m_bIsInitialised = true;
}

void CAPITextureDX11::CreateRenderTargetView()
{
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	desc.Format = m_DXGIFormat;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

#ifdef DEBUG
	HRESULT hr =
#endif
	g_pShaderDeviceDX11->GetDevice()->CreateRenderTargetView(m_pD3DTexture, &desc, &m_pRenderTargetView);
	Assert(SUCCEEDED(hr));
}

void CAPITextureDX11::CreateResourceView()
{
	if (m_pResourceView)
	{
		m_pResourceView->Release();
		m_pResourceView = NULL;
	}

	CD3D11_SHADER_RESOURCE_VIEW_DESC desc(D3D11_SRV_DIMENSION_TEXTURE2D, m_DXGIFormat);

#ifdef DEBUG
	HRESULT hr =
#endif
	g_pShaderDeviceDX11->GetDevice()->CreateShaderResourceView(m_pD3DTexture, &desc, &m_pResourceView);
	Assert(SUCCEEDED(hr));
}

void CAPITextureDX11::CreateSamplerState()
{
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = NULL;
	}

	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.MaxAnisotropy = 0;
#ifdef DEBUG
	HRESULT hr =
#endif
	g_pShaderDeviceDX11->GetDevice()->CreateSamplerState(&desc, &m_pSamplerState);
	Assert(SUCCEEDED(hr));
}

void CAPITextureDX11::LoadFromVTF(IVTFTexture* pVTF, int iVTFFrame)
{
	_AssertMsg(0, "Not Implemented! " __FUNCTION__, 0, 0);
	return;

	unsigned char *imageData = pVTF->ImageData();

	g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DTexture, 0, NULL, imageData, 0, 0);
}

void CAPITextureDX11::LoadImage2D(
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
	if (srcFormat == IMAGE_FORMAT_UNKNOWN)
		srcFormat = m_APIFormat;

	bool isDXT1 = (m_APIFormat == IMAGE_FORMAT_DXT1 || m_APIFormat == IMAGE_FORMAT_DXT1_ONEBITALPHA || m_APIFormat == IMAGE_FORMAT_DXT1_RUNTIME);
	bool isDXTAbove1 = (m_APIFormat == IMAGE_FORMAT_DXT3 || m_APIFormat == IMAGE_FORMAT_DXT5 || m_APIFormat == IMAGE_FORMAT_DXT5_RUNTIME);
	bool isDXT = isDXT1 || isDXTAbove1;

	if (isDXT)
	{
		width = ALIGN_VALUE(width, 4);
		height = ALIGN_VALUE(height, 4);
	}

	int nImageSize = ImageLoader::GetMemRequired(width, height, zOffset, m_APIFormat, false);
	unsigned char *pConvImage = (unsigned char *)malloc(nImageSize);
	int nSizePerPixel = ImageLoader::SizeInBytes(m_APIFormat);
	int nPitch = nSizePerPixel * width;
	if (isDXT1)
	{
		nPitch = 8 * (width / 4);
	}
	else if (isDXTAbove1)
	{
		nPitch = 16 * (width / 4);
	}

	if (!g_pShaderUtil->ConvertImageFormat((unsigned char *)imageData, srcFormat, pConvImage, m_APIFormat, width, height, srcStride))
	{
		_AssertMsg(0, "Unable to convert image!", 0, 0);

		free(pConvImage);
		return;
	}

	CD3D11_BOX box;
	box.left = xOffset;
	box.right = box.left + width;
	box.top = yOffset;
	box.bottom = yOffset + height;
	box.front = zOffset;
	box.back = box.front + 1;

	UINT subResource = D3D11CalcSubresource(level, cubeFaceID, m_nNumMipLevels);

	g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DTexture, subResource, &box, pConvImage, nPitch, 0);

	free(pConvImage);

	// TODO: Mips
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
		return DXGI_FORMAT_BC2_UNORM;
	case IMAGE_FORMAT_DXT5:
		return DXGI_FORMAT_BC3_UNORM;

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
	case DXGI_FORMAT_BC2_UNORM:
		return IMAGE_FORMAT_DXT3;
	case DXGI_FORMAT_BC3_UNORM:
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