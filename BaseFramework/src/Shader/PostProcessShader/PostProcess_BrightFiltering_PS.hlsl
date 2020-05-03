// BrightFiltering PS

#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��
Texture2D g_InputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_Smp : register(s3);

float4 main(VSOutput In) : SV_Target0
{
	// �e�N�X�`���̐F���擾����
	float4 texColor = g_InputTex.Sample(g_Smp, In.UV);
	texColor.rgb = max(0,texColor.rgb - 1);
	// max(a,b) �c a��b�A�傫������Ԃ�
	return texColor;
}