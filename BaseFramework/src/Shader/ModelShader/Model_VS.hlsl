// 
#include "inc_Model.hlsli"

// �J����
#include "../Common/inc_Camera.hlsli"

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, // ���W
			  float2 uv : TEXCOORD0, // UV���W
			  float3 tangent : TANGENT,	// �ڐ�
			  float3 binormal : BINORMAL,	// �]�@��
			  float3 normal : NORMAL // �@���x�N�g��
			)
{
	VSOutput Out;

	// ���W�ϊ�
	Out.Pos = mul(pos, g_mW);		// Local���W����World���W
	Out.wPos = Out.Pos.xyz;			// ���[���h���W���擾
	Out.Pos = mul(Out.Pos, g_mV);	// World���W����View���W
	Out.Pos = mul(Out.Pos, g_mP);	// VIew���W����ˉe���W

	// ���[���h�@�����o��
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	Out.wB = normalize(mul(binormal, (float3x3)g_mW));
	Out.wN = normalize(mul(normal, (float3x3)g_mW));

	// UV���W
	Out.UV = uv;
	
	return Out;
}