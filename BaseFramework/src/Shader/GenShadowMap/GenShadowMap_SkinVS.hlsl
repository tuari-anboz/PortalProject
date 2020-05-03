#include "inc_GenShadowMap.hlsli"

// �J�����f�[�^
#include "../Common/inc_Camera.hlsli"
// ���C�g�f�[�^
#include "../Common/inc_Light.hlsli"

// �萔�o�b�t�@
cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;
};

// �萔�o�b�t�@(�{�[��)
cbuffer cBone : register(b1)
{
	// �{�[���s��z��
	row_major float4x4 g_mBones[1024];
}

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, // ���_���W(���[�J��)
	float2 uv : TEXCOORD0, // UV
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
	//----------------------------------------------------------

	VSOutput result;

	// �ˉe�ϊ�
	result.Pos = mul(pos, g_mW);
	result.Pos = mul(result.Pos, g_DirLightVP[g_CascadeNowIdx]);
	// �ˉe���W���R�s�[����PS�Ɏ����Ă���
	// ��result.Pos��PS�O�ɂ���ɕϊ�����A�ˉe���W�ɂ͂Ȃ��Ă��Ȃ�����(�Z�}���e�B�N�X��POSITION�͖̂����������)
	result.wvpPos = result.Pos;

	// UV�͂��̂܂ܓ����
	result.UV = uv;

	return result;
}