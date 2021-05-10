
#include "dx11global.h"
#include "shaderdevicedx11.h"

#include "constantbufferdx11.h"

#include <d3d11.h>

extern CShaderDeviceDX11 *g_pShaderDeviceDX11;

//-------------------------------------------//
//             CConstantBufferDX11           //
//-------------------------------------------//

CConstantBufferDX11::CConstantBufferDX11(ShaderBufferType_t type, int nBufferSize, const char *pBudgetGroup)
{
	Assert(nBufferSize != 0);
	if (nBufferSize == 0)
		return;

	m_bIsDynamic = IsDynamicBufferType(type);

	m_nBufferSize = nBufferSize;
	m_nBufferPosition = 0;
	m_bIsLocked = false;

	m_pD3DBuffer = NULL;

	if (!m_bIsDynamic)
		m_pConstantBuffer = (unsigned char*)malloc(m_nBufferSize);
	else
		m_pConstantBuffer = NULL;
}

CConstantBufferDX11::~CConstantBufferDX11()
{
	DestroyBuffer();

	if (m_pConstantBuffer)
		free(m_pConstantBuffer);
}

bool CConstantBufferDX11::CreateBuffer()
{
	DestroyBuffer();

	D3D11_BUFFER_DESC bufferDesc;
	if (m_bIsDynamic)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	bufferDesc.ByteWidth = m_nBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.MiscFlags = 0;

	HRESULT hr = g_pShaderDeviceDX11->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_pD3DBuffer);
	Assert(!FAILED(hr));

	m_nBufferPosition = 0;

	m_bIsLocked = false;

	return !FAILED(hr);
}

void CConstantBufferDX11::DestroyBuffer()
{
	if (m_pD3DBuffer)
		m_pD3DBuffer->Release();
	m_pD3DBuffer = NULL;
}

// Returns the number of bytes that can still be written into the buffer
int CConstantBufferDX11::GetRoomRemaining() const
{
	return m_nBufferSize - m_nBufferPosition;
}


bool CConstantBufferDX11::Lock(int nSize, bool bAppend, ConstantDesc_t &desc)
{
	Assert(!m_bIsLocked);

	g_ShaderAPIMutex.Lock();

	if (m_bIsDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		D3D11_MAP map = D3D11_MAP_WRITE_NO_OVERWRITE;
		if (!bAppend || GetRoomRemaining() < nSize)
		{
			map = D3D11_MAP_WRITE_DISCARD;
			m_nBufferPosition = 0;
		}

		HRESULT hr = g_pShaderDeviceDX11->GetDeviceContext()->Map(m_pD3DBuffer, 0, bAppend ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			Assert(0);
			desc.m_pData = NULL;

			g_ShaderAPIMutex.Unlock();

			return false;
		}

		desc.m_pData = (unsigned char *)(mappedResource.pData) + m_nBufferPosition;
	}
	else
	{
		if (GetRoomRemaining() < nSize)
		{
			AssertMsg(0, "%d < %d !", GetRoomRemaining(), nSize);
			desc.m_pData = NULL;

			g_ShaderAPIMutex.Unlock();

			return false;
		}

		// Don't append ever for now
		m_nBufferPosition = 0;

		desc.m_pData = m_pConstantBuffer;
	}

	m_bIsLocked = true;
	return true;

}

void CConstantBufferDX11::Unlock(int nBytesWritten)
{
	if (!m_bIsLocked) return;

	Assert((UINT)nBytesWritten <= m_nBufferSize /*- m_nBufferPosition*/);

	if (m_bIsDynamic)
	{
		g_pShaderDeviceDX11->GetDeviceContext()->Unmap(m_pD3DBuffer, 0);

	}
	else
	{
		D3D11_BOX box;
		box.left = m_nBufferPosition;
		box.right = m_nBufferPosition + nBytesWritten;
		box.top = 0;
		box.bottom = 1;
		box.back = 1;
		box.front = 0;

		g_pShaderDeviceDX11->GetDeviceContext()->UpdateSubresource(m_pD3DBuffer, 0, &box, m_pConstantBuffer + m_nBufferPosition, 0, 0);
	}

	m_nBufferPosition += nBytesWritten;
	m_bIsLocked = false;

	g_ShaderAPIMutex.Unlock();
}