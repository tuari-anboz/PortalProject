// ToneMapping PS

#include "inc_PostProcess.hlsli"

// 入力テクスチャ
Texture2D g_InputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_Smp : register(s3);

// ACES Filmic Tonemapping
// x       … HDRカラー(RGB)
// 戻り値  … ToneMap後の色
float3 ACESFilmicToneMapping(float3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return saturate(x * (a*x+b) / (x * (c*x+d)+e));
}
// 
float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_InputTex.Sample(g_Smp, In.UV);
    texColor.rgb = ACESFilmicToneMapping(texColor.rgb);

	// デガンマ
	texColor.rgb = pow(texColor.rgb, 1 / 2.2);

    return texColor;
}
