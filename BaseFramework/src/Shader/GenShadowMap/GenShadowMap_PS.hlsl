#include "inc_GenShadowMap.hlsli"

// テクスチャ
Texture2D g_DiffuseTex : register(t0);

// サンプラ
SamplerState g_WrapSmp : register(s0);

float4 main(VSOutput In) : SV_Target0
{
	float4 texCol = g_DiffuseTex.Sample(g_WrapSmp,In.UV);
	// 透明ピクセルは捨てる
	if (texCol.a <= 0.1)
		discard;

	// 射影行列で変換した座標は必ずwで割る
	return float4(In.wvpPos.z / In.wvpPos.w, 0, 0, 1);
}