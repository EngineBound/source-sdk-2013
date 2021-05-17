#ifndef WIREFRAME_PS50_H
#define WIREFRAME_PS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class wireframe_ps50_Static_Index
{
public:
	wireframe_ps50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_wireframe_ps50 1


class wireframe_ps50_Dynamic_Index
{
public:
	wireframe_ps50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_wireframe_ps50 1


#endif	// WIREFRAME_PS50_H