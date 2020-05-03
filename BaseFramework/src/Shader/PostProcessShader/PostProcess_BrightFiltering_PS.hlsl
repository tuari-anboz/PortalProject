// BrightFiltering PS

#include "inc_PostProcess.hlsli"

// 入力テクスチャ
Texture2D g_InputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_Smp : register(s3);

float4 main(VSOutput In) : SV_Target0
{
	// テクスチャの色を取得する
	float4 texColor = g_InputTex.Sample(g_Smp, In.UV);
	texColor.rgb = max(0,texColor.rgb - 1);
	// max(a,b) … aかb、大きい方を返す
	return texColor;
}