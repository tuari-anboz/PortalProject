// ToneMapping PS

#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��
Texture2D g_InputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_Smp : register(s3);

// ACES Filmic Tonemapping
// x       �c HDR�J���[(RGB)
// �߂�l  �c ToneMap��̐F
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

	// �f�K���}
	texColor.rgb = pow(texColor.rgb, 1 / 2.2);

    return texColor;
}
