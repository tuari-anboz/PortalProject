// �萔�o�b�t�@��b7�ȍ~�͌Œ�(����)�o�b�t�@�Ƃ���
// �萔�o�b�t�@(�J����)
cbuffer cbCamera : register(b7)
{
	row_major float4x4	g_mV;		// �r���[�s��
	row_major float4x4	g_mP;		// �ˉe�s��
	float3				g_CamPos;	// �J�������W

	// �����t�H�O
	float g_DistanceFogDensity;		// �t�H�O���x
	float3 g_DistanceFogColor;		// �t�H�O�F
}