// �萔�o�b�t�@(���C�g)
cbuffer cbLight : register(b8)
{
	//-------------------------
	// ����
	//-------------------------
	float3 g_AmbientLight;

	//-------------------------
	// ���s(�f�B���N�V���i��)��
	//-------------------------
	int g_DL_Cnt;		// ���s���̐�
	struct DLData
	{
		float3 Dir;		// ����
		float3 Color;	// �F
		float tmp;
	};
	DLData  g_DL[3];	// ���s���̔z��		

	//-------------------------
	// �_(�|�C���g)��
	//-------------------------
	int g_PL_Cnt;	// �_���̐�

	struct PLData
	{
		float4 Pos;		// ���W
		float3 Color;	// �F
		float  Radius;	// ���a
	};
	PLData g_PL[100];		// �_���̔z��

	//-------------------------
	// �X�|�b�g��
	//-------------------------

	//-------------------------
	// IBL
	//-------------------------
	float g_IBLIntensity;		// IBL�P�x

	//-------------------------
	// �V���h�E�}�b�s���O�p(���s����0�Ԗڐ�p)
	//-------------------------
	//// ���C�g�J�����̃r���[�{�ˉe�s��
	//row_major float4x4 g_DirLightVP;
	// ���C�g�J�����̃r���[�{�ˉe�s��z��
	row_major float4x4 g_DirLightVP[4];
	int g_CascadeNowIdx;		// ���݉��Ԗڂ̃��C�g�J�������g���Ă�H
	int g_CascadeNum;			// �J�X�P�[�h��(������1�`4)
	float4 g_CascadeDist;		// 0�`3�̋���
	// float g_CascadeDist[4]; ����_��
}