#ifndef CONSTANTBUFFERDX11_H
#define CONSTANTBUFFERDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "iconstantbufferdx11.h"
#include "IShaderDevice.h"

struct ID3D11Buffer;
typedef unsigned int UINT;

class CConstantBufferDX11 : public IConstantBufferDX11
{
public:
	CConstantBufferDX11(ShaderBufferType_t type, int nBufferSize, const char *pBudgetGroup);
	~CConstantBufferDX11();

	virtual bool CreateBuffer();
	virtual void DestroyBuffer();

public:
	// Is this constant buffer dynamic?
	FORCEINLINE bool CConstantBufferDX11::IsDynamic() const
	{
		return m_bIsDynamic;
	}

	// Returns the number of bytes that can still be written into the buffer
	virtual int GetRoomRemaining() const;

	// Locks, unlocks the constant buffer
	virtual bool Lock(int nSize, bool bAppend, ConstantDesc_t &desc);
	virtual void Unlock(int nBytesWritten);
public:
	inline ID3D11Buffer* GetBuffer()
	{
		return m_pD3DBuffer;
	}

	inline UINT GetBufferSize()
	{
		return m_nBufferSize;
	}

private:

	bool m_bIsDynamic;

	size_t m_nBufferSize;
	size_t m_nBufferPosition;

	bool m_bIsLocked;

	unsigned char* m_pConstantBuffer;

	ID3D11Buffer *m_pD3DBuffer;
};

#endif