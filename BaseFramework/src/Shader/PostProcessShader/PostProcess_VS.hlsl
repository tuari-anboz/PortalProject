// VS

#include "inc_PostProcess.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION,
			  float2 uv : TEXCOORD0
	)
{
	VSOutput Out;
	Out.Pos = pos;
	Out.UV = uv;
	return Out;
}