// Copy PS

#include "inc_PostProcess.hlsli"

// �萔�o�b�t�@
cbuffer cb : register(b0)
{
    float4 g_Color;
};

// ���̓e�N�X�`��
Texture2D g_InputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_Smp : register(s3);

float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_InputTex.Sample(g_Smp, In.UV);
    return texColor * g_Color;
}
