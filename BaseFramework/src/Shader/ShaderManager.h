#pragma once

// �C���N���[�h
#include "ModelShader/ModelShader.h"
#include "PostProcessShader/PostProcessShader.h"
#include "GenShadowMap/GenShadowMapShader.h"

//=================================================
//
// �V�F�[�_�[�֌W�̂��̂��Ǘ�����N���X
//
// �E�����̍쐬�����V�F�[�_�N���X
// �E�悭�g�p����X�e�[�g
// �E���C�g�̏��
// �E�J�����̏��
//
//=================================================
class ShaderManager {
public:

	//--------------------
	// �V�F�[�_�N���X
	//--------------------
	KdPrimitiveShader	m_KdPrimSh;
	KdModelShader		m_KdModelSh;
	KdSpriteShader		m_KdSpriteSh;

	ModelShader			m_ModelSh;
	PostProcessShader	m_postProcessSh;
	GenShadowMapShader	m_genShadowMapSh;




	//--------------------
	// �J����
	//--------------------
	KdMatrix		m_mView;		// �r���[�s��
	KdMatrix		m_mProj;		// �ˉe�s��

	// �萔�o�b�t�@(�J����)
	struct cbCamera
	{
		KdMatrix	mV;		// �r���[�s��
		KdMatrix	mP;		// �ˉe�s��
		KdVec3		CamPos;	// �J�������W

		float		DistanceFogDensity = 0.05f;		// �����t�H�O�̖��x
		KdVec3		DistanceFogColor = { 0,0,0 };		// �����t�H�O�̐F
		float		tmp;
	};
	KdConstantBuffer<cbCamera> m_cb7_Camera ;

	// �J���������e�V�F�[�_�փZ�b�g
	void UpdateCamera() {
		m_KdPrimSh.SetView(m_mView);
		m_KdPrimSh.SetProj(m_mProj);

		m_KdModelSh.SetView(m_mView);
		m_KdModelSh.SetProj(m_mProj);

		// �J������񏑂�����
		m_cb7_Camera.GetWork().mV = m_mView;
		KdMatrix mCam = m_mView;
		mCam.Inverse();
		m_cb7_Camera.GetWork().CamPos = mCam.GetPos();
		m_cb7_Camera.GetWork().mP = m_mProj;
		m_cb7_Camera.Write();		// GPU���փf�[�^��]������

	}

	// 3D��2D�ϊ�(�J������mView��mProj���g�p�B���݂̃r���[�|�[�g���g�p�B)
	// �Epos2D		�c ���ʂł���2D���W�������Ă���
	// �Epos3D		�c �����̊�ƂȂ�3D���W
	void Convert3Dto2D(KdVec3& pos2D, const KdVec3& pos3D);

	// 2D��3D�ϊ�(�J������mView��mProj���g�p�B���݂̃r���[�|�[�g���g�p�B)
	// �Epos3D		�c ���ʂł���3D���W�������Ă���
	// �Epos2D		�c �����̊�ƂȂ�2D���W
	void Convert2Dto3D(KdVec3& pos3D, const KdVec3& pos2D);

	// �J�X�P�[�h��(������)�ݒ�
	// �����F�Z�b�g
	void SetCascade(const int& cascadenum)
	{
		m_cb8_Light.GetWork().CascadeNum = cascadenum;
	}

	//--------------------
	// ���C�g
	//--------------------
	// �萔�o�b�t�@(���C�g)
	struct cbLight
	{
		// ����
		KdVec3 AmbientLight = { 0.2f,0.2f,0.2f };
		// ���s��
		int	DL_Cnt = 1;		// ���s���̐�
		struct DLData
		{
			KdVec3 Dir = { 0,-1,0 };	// ����
			float tmp;
			KdVec3 Color = { 1,1,1 };	// �F
			float tmp2;
		};
		DLData  DL[3];	// ���s���̔z��	

		// �_��
		int PL_Cnt = 0;	// �_���̐�
		float tmp;
		float tmp2;
		float tmp3;

		struct PLData
		{
			KdVec3 Pos;
			float tmp;
			KdVec3 Color = { 1,1,1 };	// �F
			float Radius;
		};
//		PLData PL[100];		//	�_���̔z��
		std::array<PLData, 100>	PL;		// �_���̔z��

		// �|�C���g��

		// IBL
		float IBLIntensity = 0.6f;
		float tmp4[3];

		//-------------------------
		// �V���h�E�}�b�s���O�p(���s����0�Ԗڐ�p)
		//-------------------------
		KdMatrix	DirLightVP[4];		// ���C�g�J�����̃r���[*�ˉe�s��z��
		int			CascadeNowIdx = 0;
		int			CascadeNum = 4;		// ������
		float		tmp5[2];
		float		CascadeDist[4] = { 5,20,100,300 };		// 0�`3�̋���
	};

	KdConstantBuffer<cbLight> m_cb8_Light;







	//--------------------
	// Sampler State
	//--------------------
	ID3D11SamplerState*		m_ss0_AnisoWrap = nullptr;	// �ٕ����t�B���^ Wrap���[�h
	ID3D11SamplerState*		m_ss1_AnisoClamp = nullptr;	// �ٕ����t�B���^ Clamp���[�h

	ID3D11SamplerState*		m_ss2_LinearWrap = nullptr;	// ���`�t�B���^ Wrap���[�h
	ID3D11SamplerState*		m_ss3_LinearClamp = nullptr;	// ���`�t�B���^ Clamp���[�h

	ID3D11SamplerState*		m_ss4_PointWrap = nullptr;	// �t�B���^�Ȃ� Wrap���[�h
	ID3D11SamplerState*		m_ss5_PointClamp = nullptr;	// �t�B���^�Ȃ� Clamp���[�h

	ID3D11SamplerState*		m_ss10_Comparison = nullptr;	// ShadowMapping�p ��r�T���v��

	//--------------------
	// Blend State
	//--------------------
	ID3D11BlendState*		m_bsDisable = nullptr;		// �u�����h����
	ID3D11BlendState*		m_bsNoBlend = nullptr;		// �u�����h�����ɂ����̃R�s�[���s��
	ID3D11BlendState*		m_bsAlpha_AtoC = nullptr;	// ����������(Alpha To Coverage�t��)
	ID3D11BlendState*		m_bsAlpha = nullptr;		// ����������
	ID3D11BlendState*		m_bsAdd = nullptr;			// ���Z����

	//--------------------
	// DepthStencil State
	//--------------------
	ID3D11DepthStencilState*	m_ds_ZCompareON_ZWriteON = nullptr;		// Z����On Z��������On
	ID3D11DepthStencilState*	m_ds_ZCompareON_ZWriteOFF = nullptr;	// Z����On Z��������Off ��ɃG�t�F�N�g�`��p
	ID3D11DepthStencilState*	m_ds_ZCompareOFF_ZWriteON = nullptr;	// Z����Off Z��������On
	ID3D11DepthStencilState*	m_ds_ZCompareOFF_ZWriteOFF = nullptr;	// Z����Off Z��������Off ���2D�摜�`��p

	//--------------------
	// Rasterizer State
	//--------------------
	ID3D11RasterizerState*		m_rs_Default = nullptr;		// �\�ʕ`��
	ID3D11RasterizerState*		m_rs_Reverse = nullptr;		// ���ʕ`��
	ID3D11RasterizerState*		m_rs_Both = nullptr;		// ���ʕ`��
	ID3D11RasterizerState*		m_rs_Wireframe = nullptr;		// ���C���[�t���[���`��
	ID3D11RasterizerState*		m_rs_WireframeBoth = nullptr;	// ���C���[�t���[���`�� &  ���ʕ`��

	ID3D11RasterizerState*		m_rs_Default_ZClipOff = nullptr;		// ���ʕ`�� Z�N���b�vOff

	//--------------------
	// �f�o�b�O�p�萔�o�b�t�@
	//--------------------
	struct cbDebug {
		int DisableMaterial = 0;	// �}�e���A���F�𖳎�
		int ShowCascadeRange = 0;	// �J�X�P�[�h�V���h�E�}�b�v�͈̔͂�\��

		float tmp[2];
	};
	KdConstantBuffer<cbDebug>	m_cb13_Debug;

	//--------------------
	// �����ݒ�
	//--------------------
	void Init();

	//--------------------
	// ���
	//--------------------
	void Release();


//=======================================
// �V���O���g��
//=======================================
private:
	ShaderManager() {}

public:
	static ShaderManager& GetInstance() {
		static ShaderManager instance;
		return instance;
	}
};

#define ShMgr ShaderManager::GetInstance()
