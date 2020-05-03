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

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, // ���_���W(���[�J��)
	float2 uv : TEXCOORD0 // UV
)
{
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