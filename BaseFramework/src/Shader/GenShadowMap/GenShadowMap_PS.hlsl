#include "inc_GenShadowMap.hlsli"

// �e�N�X�`��
Texture2D g_DiffuseTex : register(t0);

// �T���v��
SamplerState g_WrapSmp : register(s0);

float4 main(VSOutput In) : SV_Target0
{
	float4 texCol = g_DiffuseTex.Sample(g_WrapSmp,In.UV);
	// �����s�N�Z���͎̂Ă�
	if (texCol.a <= 0.1)
		discard;

	// �ˉe�s��ŕϊ��������W�͕K��w�Ŋ���
	return float4(In.wvpPos.z / In.wvpPos.w, 0, 0, 1);
}