#ifndef ICONSTANTBUFFERDX11_H
#define ICONSTANTBUFFERDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#define DYNAMIC_CONSTANT_BUFFER_MEMORY (1024 * 1024)

struct ConstantDesc_t
{
	unsigned char *m_pData;
};

abstract_class IConstantBufferDX11
{
public:
	// Add a virtual destructor to silence the clang warning.
	// This is harmless but not important since the only derived class
	// doesn't have a destructor.
	virtual ~IConstantBufferDX11() {}

	// Is this constant buffer dynamic?
	virtual bool IsDynamic() const = 0;

	// Returns the number of bytes that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0;

	// Locks, unlocks the constant buffer
	virtual bool Lock(int nSize, bool bAppend, ConstantDesc_t &desc) = 0;
	virtual void Unlock(int nBytesWritten) = 0;
};

#endif