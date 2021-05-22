
#include "BaseVSShader.h"

#include "wireframe_ps50.inc"
#include "wireframe_vs50.inc"

BEGIN_VS_SHADER(Wireframe, "")

BEGIN_SHADER_PARAMS
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
}

SHADER_DRAW
{
	SHADOW_STATE
	{
		pShaderShadow->PolyMode(SHADER_POLYMODEFACE_FRONT_AND_BACK, SHADER_POLYMODE_LINE);
		pShaderShadow->EnableCulling(false);

		pShaderShadow->VertexShaderVertexFormat(VERTEX_POSITION | VERTEX_COLOR, 0, 0, 0);

		DECLARE_STATIC_VERTEX_SHADER(wireframe_vs50);
		SET_STATIC_VERTEX_SHADER(wireframe_vs50);

		DECLARE_STATIC_PIXEL_SHADER(wireframe_ps50);
		SET_STATIC_PIXEL_SHADER(wireframe_ps50);
	}

	DYNAMIC_STATE
	{
		DECLARE_DYNAMIC_VERTEX_SHADER(wireframe_vs50);
		SET_DYNAMIC_VERTEX_SHADER(wireframe_vs50);

		DECLARE_DYNAMIC_PIXEL_SHADER(wireframe_ps50);
		SET_DYNAMIC_PIXEL_SHADER(wireframe_ps50);
	}

	Draw();
}

END_SHADER