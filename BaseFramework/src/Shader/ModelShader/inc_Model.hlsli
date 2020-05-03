// ���_�V�F�[�_�o�͗p�\����

// �萔�o�b�t�@(�I�u�W�F�N�g)
cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;	// ���[���h�s��
	float4 g_MulColor;			// �����F
	int g_LightEnable;			// ���C�gOn/Off
	float g_Roughness;			// �e��(0�`1)
	float g_Metallic;			// �����x(0�`1)
	int g_RefractEnable;		// ���ܕ\��On/Off
	int g_FogEnable;			// �t�H�OOn/Off
};

// �萔�o�b�t�@(�}�e���A��)
cbuffer cbMaterial : register(b1)
{
	float4 g_Diffuse;	// �g�U�F
	float3 g_Specular;	// ���ːF
	float  g_SpePower;	// ���˂̉s��
	float3 g_Emissive;	// �����F
};

// �萔�o�b�t�@(�{�[��)
cbuffer cBone : register(b2)
{
	// �{�[���s��z��
	row_major float4x4 g_mBones[1024];
}

// ���_�V�F�[�_�o�͗p�\����
struct VSOutput
{
	float4 Pos : SV_Position;		// �ˉe���W
	float2 UV : TEXCOORD1;			// UV���W
	float3 wPos : TEXCOORD2;		// ���[���h���W

	float3 wT : TEXCOORD3;			// ���[���h�ڐ�
	float3 wB : TEXCOORD4;			// ���[���h�]�@��
	float3 wN : TEXCOORD0;			// ���[���h�@��
};