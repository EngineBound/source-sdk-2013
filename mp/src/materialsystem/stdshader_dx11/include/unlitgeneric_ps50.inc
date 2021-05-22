#ifndef UNLITGENERIC_PS50_H
#define UNLITGENERIC_PS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class unlitgeneric_ps50_Static_Index
{
public:
	unlitgeneric_ps50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_unlitgeneric_ps50 1


class unlitgeneric_ps50_Dynamic_Index
{
public:
	unlitgeneric_ps50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_unlitgeneric_ps50 1


#endif	// UNLITGENERIC_PS50_H