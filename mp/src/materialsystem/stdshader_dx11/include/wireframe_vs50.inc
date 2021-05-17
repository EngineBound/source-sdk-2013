#ifndef WIREFRAME_VS50_H
#define WIREFRAME_VS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class wireframe_vs50_Static_Index
{
public:
	wireframe_vs50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_wireframe_vs50 1


class wireframe_vs50_Dynamic_Index
{
public:
	wireframe_vs50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_wireframe_vs50 1


#endif	// WIREFRAME_VS50_H