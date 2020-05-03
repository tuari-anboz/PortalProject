// Copy PS

#include "inc_PostProcess.hlsli"

// 定数バッファ
cbuffer cb : register(b0)
{
    float4 g_Color;
};

// 入力テクスチャ
Texture2D g_InputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_Smp : register(s3);

float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_InputTex.Sample(g_Smp, In.UV);
    return texColor * g_Color;
}
