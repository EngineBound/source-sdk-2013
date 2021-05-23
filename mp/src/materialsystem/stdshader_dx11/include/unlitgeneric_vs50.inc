#ifndef UNLITGENERIC_VS50_H
#define UNLITGENERIC_VS50_H

#include "shaderapi/ishaderapi.h"
#include "shaderapi/ishadershadow.h"
#include "materialsystem/imaterialvar.h"

class unlitgeneric_vs50_Static_Index
{
	unsigned int m_nVERTEXCOLOR : 2;
#ifdef _DEBUG
	bool m_bVERTEXCOLOR : 1;
#endif	// _DEBUG
public:
	void SetVERTEXCOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLOR = i;
#ifdef _DEBUG
		m_bVERTEXCOLOR = true;
#endif	// _DEBUG
	}

	unlitgeneric_vs50_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nVERTEXCOLOR = 0;
#ifdef _DEBUG
		m_bVERTEXCOLOR = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bVERTEXCOLOR );
		return ( 1 * m_nVERTEXCOLOR ) + 0;
	}
};

#define shaderStaticTest_unlitgeneric_vs50 vsh_forgot_to_set_static_VERTEXCOLOR


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