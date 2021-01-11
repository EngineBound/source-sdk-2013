#ifndef STATESDX11_H
#define STATESDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "IShaderDevice.h"
#include "ishadershadow.h"
#include "ishaderapi.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include "shaderapidx11_global.h"

#include "tier1/utlstack.h"

#define MAX_DX11_VIEWPORTS 16

struct DynamicStateDX11
{
	int m_nNumViewports;
	D3D11_VIEWPORT m_pViewports[MAX_DX11_VIEWPORTS];

	DynamicStateDX11()
	{
		ZeroMemory(this, sizeof(DynamicStateDX11));

		m_nNumViewports = 1;
		m_pViewports[0].Width = 640;
		m_pViewports[0].Height = 480;
		m_pViewports[0].MaxDepth = 1;
		m_pViewports[0].MinDepth = 0;
	}
};

struct ShaderStateDX11
{
	CUtlStack<DirectX::XMMATRIX> m_pMatrixStacks[NUM_MATRIX_MODES];

	ShaderStateDX11()
	{
		ZeroMemory(this, sizeof(DynamicStateDX11));

		for (int i = 0; i < NUM_MATRIX_MODES; ++i)
		{
			m_pMatrixStacks[i].Clear();
			m_pMatrixStacks[i].Push(DirectX::XMMatrixIdentity());
		}
	}

};

#endif