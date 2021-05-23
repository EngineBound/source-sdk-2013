
#include "BaseVSShader.h"

#include "unlitgeneric_ps50.inc"
#include "unlitgeneric_vs50.inc"

BEGIN_VS_SHADER(UnlitGeneric, "")

BEGIN_SHADER_PARAMS
	SHADER_PARAM(BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "Base Texture")
END_SHADER_PARAMS

SHADER_INIT_PARAMS()
{
}

SHADER_FALLBACK
{
	return 0;
}

SHADER_INIT
{
	if (params[BASETEXTURE]->IsDefined())
	{
		LoadTexture(BASETEXTURE);
	}
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		unsigned int flags = VERTEX_POSITION;
		int nTexCoordCount = 0;

		bool bVertexColor = false;

		if (IS_FLAG_SET(MATERIAL_VAR_VERTEXCOLOR) || IS_FLAG_SET(MATERIAL_VAR_VERTEXALPHA))
		{
			flags |= VERTEX_COLOR;

			bVertexColor = true;
		}

		if (params[BASETEXTURE]->IsDefined())
		{
			pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
			++nTexCoordCount;
		}

		pShaderShadow->VertexShaderVertexFormat(flags, nTexCoordCount, 0, 0);

		DECLARE_STATIC_VERTEX_SHADER(unlitgeneric_vs50);
		SET_STATIC_VERTEX_SHADER_COMBO(VERTEXCOLOR, bVertexColor);
		SET_STATIC_VERTEX_SHADER(unlitgeneric_vs50);

		DECLARE_STATIC_PIXEL_SHADER(unlitgeneric_ps50);
		SET_STATIC_PIXEL_SHADER(unlitgeneric_ps50);
	}

	DYNAMIC_STATE
	{
		if (params[BASETEXTURE]->IsDefined())
		{
			BindTexture(SHADER_SAMPLER0, BASETEXTURE, 0);
		}

		DECLARE_DYNAMIC_VERTEX_SHADER(unlitgeneric_vs50);
		SET_DYNAMIC_VERTEX_SHADER(unlitgeneric_vs50);

		DECLARE_DYNAMIC_PIXEL_SHADER(unlitgeneric_ps50);
		SET_DYNAMIC_PIXEL_SHADER(unlitgeneric_ps50);
	}

	Draw();
}

END_SHADER