// 
#include "inc_Model.hlsli"

// �J����
#include "../Common/inc_Camera.hlsli"

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, // ���W
	float2 uv : TEXCOORD0, // UV���W
	float3 tangent : TANGENT,	// �ڐ�
	float3 binormal : BINORMAL, // �]�@�� 
	float3 normal : NORMAL, // �@���x�N�g��
	float4 blendWeight : BLENDWEIGHT, // �{�[���E�F�C�gx4�@�����ɁA ���̒��_�ɉe�����Ă���{�[���̉e���x�������Ă���@�ő�4��(x,y,z,w)
	uint4 blendIndex : BLENDINDICES	 // �{�[���ԍ�4�@�����ɁA���̒��_�ɉe�����Ă���{�[���ԍ��������Ă���  �ő�S��(x,y,z,w)
			)
{
	//---------------------------------------------------------
	// �X�L�j���O����
	// ���̒��_�ɉe�����Ă��镡���̃{�[���̍s����g�p����
	// pos��normal�Ȃǂ�ϊ�����
	//---------------------------------------------------------
	row_major float4x4 mBones = 0; // �ŏI�I�ȃ��[���h�s��
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_mBones[blendIndex[i]] * blendWeight[i];
	}
	// �{�[���s��ŕϊ�(�܂����[�J�����W�n)
	pos = mul(pos, mBones);
	tangent = mul(tangent, (float3x3)mBones);
	binormal = mul(binormal, (float3x3)mBones);
	normal = mul(normal, (float3x3)mBones);
	//----------------------------------------------------------

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