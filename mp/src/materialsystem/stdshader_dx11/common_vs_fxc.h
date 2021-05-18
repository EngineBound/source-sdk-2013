#ifndef COMMON_VS_FXC_H
#define COMMON_VS_FXC_H

cbuffer SceneMatrices : register(b0)
{
    float4x4 cMVPMatrix;
};

#endif