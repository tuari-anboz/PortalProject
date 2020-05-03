#include "main.h"

#include "GenShadowMapShader.h"

//====================================================================
// �J�����̏�񂩂�A�҂�������܂�r���[�s��Ɛ��ˉe�s������߂�
// mView	: �r���[�s��
// mProj	: �ˉe�s��
// lightDir	: ���C�g�̕���
// outView	: [�o��]�Z�o�����r���[�s��
// outProj	: [�o��]�Z�o�����ˉe�s��
//====================================================================
static void CreateLightViewProj(const KdMatrix& mView, const KdMatrix& mProj, const KdVec3& lightDir, KdMatrix& outView, KdMatrix& outProj)
{

	//-------------------------------------------
	// ���݂̎�����̊e�_(8���W)�����߂�
	// �ˉe���W�n���烏�[���h���W�n�֋t�ϊ�
	//-------------------------------------------
	KdMatrix camInvProj = mProj;
	camInvProj.Inverse();
	KdMatrix mCam = mView;
	mCam.Inverse();
	KdMatrix camInvPV = camInvProj * mCam;

	// -1�`1(Z��0�`1)�̎ˉe���W�n��BOX�f�[�^
	std::vector<KdVec3> PtTbl;
	PtTbl.push_back(KdVec3(1, 1, 0));
	PtTbl.push_back(KdVec3(-1, 1, 0));
	PtTbl.push_back(KdVec3(-1, -1, 0));
	PtTbl.push_back(KdVec3(1, -1, 0));
	PtTbl.push_back(KdVec3(1, 1, 1));
	PtTbl.push_back(KdVec3(-1, 1, 1));
	PtTbl.push_back(KdVec3(-1, -1, 1));
	PtTbl.push_back(KdVec3(1, -1, 1));
	// �ˉe���W�n�̎�����W���W�����[���h���W�n�֕ϊ�
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].Transform(camInvPV);
	}
	// ���̃r���[�s��쐬(���W�͂܂�����Ȃ̂ŕ����̂�)
	KdMatrix lightView;
	lightView.LookTo(lightDir, { 0, 1, 0 });
	lightView.Inverse();

	// ������̂W�_�̍��W�����̃r���[���W�n�֕ϊ�
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].Transform(lightView);
	}
	// �������AABB���Z�o
	KdVec3 vMin, vMax;
	vMin = PtTbl[0];
	vMax = PtTbl[0];
	for (UINT i = 1; i < PtTbl.size(); i++) {
		if (vMin.x > PtTbl[i].x)vMin.x = PtTbl[i].x;
		if (vMin.y > PtTbl[i].y)vMin.y = PtTbl[i].y;
		if (vMin.z > PtTbl[i].z)vMin.z = PtTbl[i].z;
		if (vMax.x < PtTbl[i].x)vMax.x = PtTbl[i].x;
		if (vMax.y < PtTbl[i].y)vMax.y = PtTbl[i].y;
		if (vMax.z < PtTbl[i].z)vMax.z = PtTbl[i].z;
	}
	// AABB����A�҂�������܂�傫���̐��ˉe�s��쐬
	outProj.CreateOrthoLH(vMax.x - vMin.x, vMax.y - vMin.y, 0, vMax.z - vMin.z);

	// AABB�̒��S(X��Y)�̃r���[�s����쐬(������Z=0�ɂ���)
	KdVec3 vCenter = vMin + (vMax - vMin)*0.5f;
	vCenter.z = vMin.z;
	lightView.Inverse();
	vCenter.Transform(lightView);

	outView.CreateMove(vCenter);
	outView.LookTo(lightDir, { 0, 1, 0 });
	outView.Inverse();
}

void GenShadowMapShader::DrawModelDepth(const KdModel & model)
{
	//-------------------
	// �V�F�[�_���Z�b�g
	//-------------------
		// �X�L�����b�V���̎�
	if (model.GetMesh()->IsSkinMesh())
	{
		// ���_�V�F�[�_���f�o�C�X�R���e�L�X�g�փZ�b�g
		KD3D.GetDevContext()->VSSetShader(m_SkinVS, 0, 0);
		// ���̓��C�A�E�g���Z�b�g
		KD3D.GetDevContext()->IASetInputLayout(m_SkinVLayout);
	}
	else
	{
		// ���_�V�F�[�_���f�o�C�X�R���e�L�X�g�փZ�b�g
		KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// ���̓��C�A�E�g���Z�b�g
		KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	}

	// �s�N�Z���V�F�[�_���Z�b�g
	KD3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	// �萔�o�b�t�@���Z�b�g
	m_cb0.GetBuffer().VSSetConstantBuffer(0);
	m_cb0.GetBuffer().PSSetConstantBuffer(0);
	// �萔�o�b�t�@��������
	m_cb0.Write();

	// ���b�V���̏��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�v���~�e�B�u�E�g�|���W�[��3��)���f�o�C�X�փZ�b�g
	model.GetMesh()->SetToDevice();

	//--------------------
	// �}�e���A�����[�v
	//--------------------
	for (UINT mi = 0; mi < model.GetMaterials().size(); mi++)
	{
		// �ʐ���0�Ȃ�X�L�b�v
		if (model.GetMesh()->GetSubset()[mi].FaceCount == 0)continue;

		// �}�e���A���f�[�^�̎Q��
		auto& mate = model.GetMaterials()[mi];

		// Diffuse�e�N�X�`��
		if (mate.texDiffuse)
		{
			mate.texDiffuse->PSSetShaderResource(0);
		}
		else
		{
			KD3D.GetWhiteTex()->PSSetShaderResource(0);
		}

		// �`��
		model.GetMesh()->DrawSubset(mi);
	}
}

void GenShadowMapShader::Generate(std::function<void()> drawProc)
{
	// ���s�����������Ȃ�A�������Ȃ�
	if (ShMgr.m_cb8_Light.GetWork_Read().DL_Cnt == 0)return;

	// �[�x�}�b�v������
	KdTexture::s_emptyTex.PSSetShaderResource(21);

	// RT���N���A
	m_depthMap->ClearRT({ 1,1,1,1 });
	m_zBuffer->ClearDepth();

	for(int ci = 0; ci < ShMgr.m_cb8_Light.GetWork_Read().CascadeNum; ci++)
	{
		// ���C�g�J�����̃r���[�E�ˉe�s��쐬
		KdMatrix mLV;
		KdMatrix mLP;

		// ���Ɍ��݂̎ˉe�s����쐬(������50m�ɐ������邽��)
		float ang;
		float aspect;
		float fNear, fFar;
		// ���݂̎ˉe�s��̃f�[�^���t�Z����
		ShMgr.m_mProj.ComputePerspectiveInfo(ang, aspect, fNear, fFar);
		// Far��50m�ɐ��������ˉe�s����쐬
		KdMatrix		mTmpProj;
		mTmpProj.CreatePerspectiveFovLH(ang, aspect,
			ci == 0 ? fNear : ShMgr.m_cb8_Light.GetWork().CascadeDist[ci-1],
			ShMgr.m_cb8_Light.GetWork().CascadeDist[ci]);
		// �J�����̃r���[�s��Ǝˉe�s�񂩂�A���C�g�J�������Z�o
		CreateLightViewProj(
			ShMgr.m_mView,		// �J�����̃r���[�s��
			mTmpProj,			// �J�����̎ˉe�s��
			ShMgr.m_cb8_Light.GetWork().DL[0].Dir,		// ���C�g�̕���
			mLV,				// ���C�g�̕���
			mLP					// ������͈̔͂ɂ҂�������܂����s�񂪓���
		);

		// �K���Ƀr���[�E�ˉe���쐬
		//mLV.LookTo(ShMgr.m_cb8_Light.GetWork().DL[0].Dir, { 0,1,0 });
		//mLV.Move_Local(0, 0, -50);
		//mLV.Inverse();

		//mLP.CreateOrthoLH(30, 30, 0.1f, 100);		// ���ˉe�s��(�|�C���g���C�g�̉e�Ȃ瓧�����e)

		ShMgr.m_cb8_Light.GetWork().DirLightVP[ci] = mLV * mLP;		// ���C�g�J�����̃r���[���ˉe
		ShMgr.m_cb8_Light.GetWork().CascadeNowIdx = ci;
		ShMgr.m_cb8_Light.Write();

		{
			// ���݂�RT��Z�o�b�t�@���L������
			KdRTSaver rtSaver;

			// ���X�^���C�U�X�e�[�g
			KdStateSaver sSaver;
			sSaver.SaveRS();
			KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Default_ZClipOff);

			{
				// RT��Z�o�b�t�@��ύX
				KdRTChanger rtc;
				rtc.RT(0, m_depthMap->GetRTV_Array(ci));
				rtc.Depth(m_zBuffer->GetDSV_Array(ci));
				rtc.SetToDevice();

				// �`��֐����s
				drawProc();
			}
		}

	}

	// �[�x�}�b�v���Z�b�g���Ă���
	m_zBuffer->PSSetShaderResource(21);
}

void GenShadowMapShader::Init()
{
	Release();

	//------------------
	// ���_�V�F�[�_
	//------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�փw�b�_�[�t�@�C�����C���N���[�h
#include "GenShadowMap_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(KD3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS)))
		{
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return;
		}

		// ���_�C���v�b�g���C�A�E�g�쐬
		auto VDecl = KdVertex_Pos_UV_TBN::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&VDecl.m_Layout[0],
			VDecl.m_Layout.size(),
			compiledBuffer,
			sizeof(compiledBuffer),
			&m_VLayout
		))
			) {
			assert(0 && "���_���C�A�E�g�쐬���s");
			Release();
			return;
		}
	}

	// SkinMesh���_�V�F�[�_�ǂݍ���
	{
#include "GenShadowMap_SkinVS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// �V�F�[�_�f�[�^�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			nullptr,
			&m_SkinVS))						// �쐬���ꂽ�V�F�[�_������
			) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
		}

		// ���_���̓��C�A�E�g�̍쐬
		auto verDecl = KdVertex_Pos_UV_TBN_Skin::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&verDecl.m_Layout[0],		// ���_���C�A�E�g�f�[�^�̐擪address
			verDecl.m_Layout.size(),			// ���C�A�E�g�̍��ڐ�
			compiledBuffer,				// ���_�V�F�[�_�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			&m_SkinVLayout))				// �����ɍ쐬���ꂽ���̂�����
			) {
			assert(0 && "���_���C�A�E�g�쐬���s");
		}
	}


	//---------------------------
	// �s�N�Z���V�F�[�_
	//---------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
#include "GenShadowMap_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS)))
		{
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return;
		}
	}

	//--------------------------------
	// �萔�o�b�t�@
	//--------------------------------
	m_cb0.Create();

	//--------------------------------
	//--------------------------------
	m_depthMap = KdMakeSPtr<KdTexture>();
	m_depthMap->CreateRT(1024, 1024, DXGI_FORMAT_R32_FLOAT, 4);
	m_zBuffer = KdMakeSPtr<KdTexture>();
	m_zBuffer->CreateDepth(1024, 1024, DXGI_FORMAT_R32_TYPELESS, 4);
}
