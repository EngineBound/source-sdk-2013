#ifndef ISHADERDEVICEDX11_H
#define ISHADERDEVICEDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "IShaderDevice.h"
#include "iconstantbufferdx11.h"

abstract_class IShaderDeviceDX11 : public IShaderDevice
{
public:
	virtual IConstantBufferDX11 *CreateConstantBuffer(ShaderBufferType_t bufferType, int nBufferSize, const char *pBudgetGroup) = 0;
	virtual void DestroyConstantBuffer(IConstantBufferDX11 *pConstantBuffer) = 0;

	// virtual IIndexBuffer *GetDynamicConstantBuffer(bool bBuffered = true);
};

#endif