
#include "BaseVSShader.h"

#include "testshader_dx11_ps50.inc"
#include "testshader_dx11_vs50.inc"

BEGIN_VS_SHADER(Bik, "") // Lole

BEGIN_SHADER_PARAMS
SHADER_PARAM(YTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "Y Bink Texture")
SHADER_PARAM(CRTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "Cr Bink Texture")
SHADER_PARAM(CBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "Cb Bink Texture")
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
	if (params[YTEXTURE]->IsDefined())
	{
		LoadTexture(YTEXTURE);
	}
	if (params[CRTEXTURE]->IsDefined())
	{
		LoadTexture(CRTEXTURE);
	}
	if (params[CBTEXTURE]->IsDefined())
	{
		LoadTexture(CBTEXTURE);
	}
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER1, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER2, true);

		pShaderShadow->VertexShaderVertexFormat(VERTEX_POSITION, 1, 0, 0);

		DECLARE_STATIC_VERTEX_SHADER(testshader_dx11_vs50);
		SET_STATIC_VERTEX_SHADER(testshader_dx11_vs50);

		DECLARE_STATIC_PIXEL_SHADER(testshader_dx11_ps50);
		SET_STATIC_PIXEL_SHADER(testshader_dx11_ps50);

		pShaderShadow->EnableSRGBWrite(false);
	}

	DYNAMIC_STATE
	{
		BindTexture(SHADER_SAMPLER0, YTEXTURE, FRAME);
		BindTexture(SHADER_SAMPLER1, CRTEXTURE, FRAME);
		BindTexture(SHADER_SAMPLER2, CBTEXTURE, FRAME);

		DECLARE_DYNAMIC_VERTEX_SHADER(testshader_dx11_vs50);
		SET_DYNAMIC_VERTEX_SHADER(testshader_dx11_vs50);

		DECLARE_DYNAMIC_PIXEL_SHADER(testshader_dx11_ps50);
		SET_DYNAMIC_PIXEL_SHADER(testshader_dx11_ps50);
	}

	Draw();
}

END_SHADER