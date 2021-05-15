#ifndef TESTSHADER_DX11_PS50_H
#define TESTSHADER_DX11_PS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class testshader_dx11_ps50_Static_Index
{
public:
	testshader_dx11_ps50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_testshader_dx11_ps50 1


class testshader_dx11_ps50_Dynamic_Index
{
public:
	testshader_dx11_ps50_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_testshader_dx11_ps50 1


#endif	// TESTSHADER_DX11_PS50_H