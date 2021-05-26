#ifndef COMMON_VS_FXC_H
#define COMMON_VS_FXC_H

cbuffer SceneMatrices : register(b0)
{
    float4x4 cModelMat;
    float4x4 cViewMat;
    float4x4 cProjMat;
};

float4 ProjectPosition(float3 pos)
{
	float4 outVec = float4(pos.xyz, 1);
	outVec = mul(outVec, cModelMat);
	outVec = mul(outVec, cViewMat);
	outVec = mul(outVec, cProjMat); // THIS IS HORRID BUT IT IS GUARANTEED TO WORK FOR NOW BECAUSE MATRIX MULTIPLICATION IS ANNOYINGLY CONFUSING

	//outVec.z = abs(outVec.z);

	return outVec;
}

#endif