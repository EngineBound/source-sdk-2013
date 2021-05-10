#ifndef ISHADERAPIDX11_H
#define ISHADERAPIDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishaderapi.h"
#include "iconstantbufferdx11.h"
#include "materialsystem/idebugtextureinfo.h"

abstract_class IShaderAPIDX11 : public IShaderAPI
{
public:
	virtual void BindConstantBuffer(IConstantBufferDX11 *pConstantBuffer, int nOffsetInBytes) = 0;
};

abstract_class IDebugTextureInfoDX11 : public IDebugTextureInfo
{

};

#endif