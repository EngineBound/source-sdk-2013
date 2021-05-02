#ifndef STATESDX11_H
#define STATESDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "utlstack.h"

#include <d3d11.h>
#include <DirectXMath.h>

#define MAX_DX11_VIEWPORTS 16

struct DynamicStateDX11 {
	int m_nViewportCount;
	D3D11_VIEWPORT m_pViewports[MAX_DX11_VIEWPORTS];

	FLOAT m_ClearColor[4];

	DynamicStateDX11()
	{
		V_memset(this, 0, sizeof(*this));
		m_nViewportCount = 1;

		m_pViewports[0].Width = 640;
		m_pViewports[0].Height = 480;

		m_pViewports[0].MinDepth = 0;
		m_pViewports[0].MaxDepth = 1;

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
			m_MatrixStacks[i].Push(DirectX::XMMatrixIdentity());
		}

	}
};

#endif