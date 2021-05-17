#ifndef STATESDX11_H
#define STATESDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "utlstack.h"

#include "IShaderDevice.h" // Lol,,,
#include "dx11global.h"

#include <d3d11.h>
#include <DirectXMath.h>

#define MAX_DX11_VIEWPORTS 16
#define MAX_DX11_SAMPLERS 16

struct DynamicStateDX11 {
	int m_nViewportCount;
	D3D11_VIEWPORT m_pViewports[MAX_DX11_VIEWPORTS];

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	VertexShaderHandle_t m_hVertexShader;

	ID3D11InputLayout* m_pInputLayout;
	VertexFormat_t m_VertexFormat;

	ID3D11Buffer* m_pVertexBuffer;
	UINT m_VBStride;
	UINT m_VBOffset;

	ID3D11Buffer* m_pIndexBuffer;
	DXGI_FORMAT m_IBFmt;
	UINT m_IBOffset;

	ID3D11Buffer* m_pConstantBuffer;
	UINT m_CBOffset;

	D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

	ID3D11ShaderResourceView *m_ppTextures[MAX_DX11_SAMPLERS];
	ID3D11SamplerState *m_ppSamplers[MAX_DX11_SAMPLERS];
	int m_nNumSamplers;

	FLOAT m_ClearColor[4];

	DynamicStateDX11()
	{
		V_memset(this, 0, sizeof(*this));

		m_nViewportCount = 1;

		m_pVertexBuffer = NULL;
		m_pPixelShader = NULL;

		m_hVertexShader = 0;

		m_pInputLayout = NULL;
		m_VertexFormat = VERTEX_FORMAT_UNKNOWN;

		m_pVertexBuffer = NULL;
		m_VBStride = 0;
		m_VBOffset = 0;

		m_pIndexBuffer = NULL;
		m_IBFmt = DXGI_FORMAT_UNKNOWN;
		m_IBOffset = 0;

		m_pConstantBuffer = NULL;
		m_CBOffset = 0;

		for (int i = 0; i < MAX_DX11_SAMPLERS; ++i)
		{
			m_ppTextures[i] = NULL;
			m_ppSamplers[i] = NULL;
		}

		m_nNumSamplers = 0;

		m_pViewports[0].Width = 640;
		m_pViewports[0].Height = 480;

		m_pViewports[0].MinDepth = 0;
		m_pViewports[0].MaxDepth = 1;

		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		for (int i = 0; i < 4; ++i)
			m_ClearColor[i] = 0;
	}
};

struct ShaderStateDX11 {

	// Matrices
	CUtlStack<DirectX::XMMATRIX> m_MatrixStacks[NUM_MATRIX_MODES];

	ShaderStateDX11()
	{
		V_memset(this, 0, sizeof(*this));

		for (int i = 0; i < NUM_MATRIX_MODES; ++i)
		{
			m_MatrixStacks[i].Clear();
			m_MatrixStacks[i].Push();
			m_MatrixStacks[i].Top() = DirectX::XMMatrixIdentity();
		}

	}
};

struct ShadowStateDX11 {
	StaticShaderHandle_t m_hStaticVertexShader;
	StaticShaderHandle_t m_hStaticPixelShader;

	ShaderHandle_t m_hVertexShader;
	ShaderHandle_t m_hPixelShader;

	ShadowStateDX11()
	{
		V_memset(this, 0, sizeof(*this));
	}
};

#endif