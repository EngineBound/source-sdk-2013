#ifndef BIK_PS50_H
#define BIK_PS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class bik_ps50_Static_Index
{
public:
	bik_ps50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_bik_ps50 1


class bik_ps50_Dynamic_Index
{
public:
	bik_ps50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_bik_ps50 1


#endif	// BIK_PS50_H