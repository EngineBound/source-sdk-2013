#ifndef BIK_VS50_H
#define BIK_VS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class bik_vs50_Static_Index
{
public:
	bik_vs50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_bik_vs50 1


class bik_vs50_Dynamic_Index
{
public:
	bik_vs50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_bik_vs50 1


#endif	// BIK_VS50_H