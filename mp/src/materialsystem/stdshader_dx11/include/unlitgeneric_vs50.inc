#ifndef UNLITGENERIC_VS50_H
#define UNLITGENERIC_VS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class unlitgeneric_vs50_Static_Index
{
public:
	unlitgeneric_vs50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_unlitgeneric_vs50 1


class unlitgeneric_vs50_Dynamic_Index
{
public:
	unlitgeneric_vs50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_unlitgeneric_vs50 1


#endif	// UNLITGENERIC_VS50_H