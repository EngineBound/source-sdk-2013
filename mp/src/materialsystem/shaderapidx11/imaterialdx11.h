#ifndef IMATERIALDX11_H
#define IMATERIALDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem/imaterial.h"

#define _JOIN_TOKENS(x, y) x ## y
#define _JOIN_TOKENS_INDIRECTION(x, y) _JOIN_TOKENS(x, y)
#define VIRTUAL_PASSTHROUGH() virtual void _JOIN_TOKENS_INDIRECTION(__VPS, __LINE__) (void)

abstract_class IMaterialDX11 : public IMaterial
{
public:
	// VIRTUAL PASSTHROUGH FUNCS - THESE ARE HERE TO ENSURE THEY ARE NOT MOVED BY OTHER DECLARATIONS
	// There are 42 extra virtual functions total - 14 before DrawElements

	// 0
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();

	// 5
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();

	// 10
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();
	VIRTUAL_PASSTHROUGH();

	// 14
	// Huzzah
	virtual void DrawElements(VertexCompressionType_t compression);
};

#undef VIRTUAL_PASSTHROUGH

#endif