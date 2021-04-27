#ifndef IMESHDX11_H
#define IMESHDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem/imesh.h"

abstract_class IVertexBufferDX11 : public IVertexBuffer
{

};

abstract_class IIndexBufferDX11 : public IIndexBuffer
{

};

abstract_class IMeshDX11 : public IMesh, public IVertexBufferDX11, public IIndexBufferDX11
{

};

#endif