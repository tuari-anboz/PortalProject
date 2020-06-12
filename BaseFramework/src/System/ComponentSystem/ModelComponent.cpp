#include "main.h"
#include "../GameManager.h"
#include "ModelComponent.h"

void ModelComponent::SetModel(const std::string & filename)
{
	m_modelFilename = filename;

	// �܂����
	m_bc.Release();			// �{�[���R���g���[�����
	m_anim.ResetTrack();	// �A�j����~

	// �V�������f����ǂݍ���
	m_model = KDResFactory.GetGameModel(filename);
	// ���̑��ݒ�
	if (m_model) {
		// m_model�p�̃{�[���ɏ���������
		m_bc.Init(*m_model);

		// BoneController�ƃA�j���[�^��ڑ�����
		m_bc.BindAnimator(m_anim);

		// �A�j���[�V������o�^
		m_anim.SetAnimationList(m_model->GetAnimeList());
		// �A�j���[�V�����Đ�
		m_anim.ChangeAnime(m_playAnimename, m_loopAnime);

		// �S�{�[�����Čv�Z
		m_bc.CalcBoneMatrix();
		// �S�{�[���̍s���萔�o�b�t�@�ɏ�������
		m_bc.WriteBoneConstantBuffer();
	}
}

void ModelComponent::ChangeEmissiveMap()
{
}

void ModelComponent::Update()
{
	// �������͂Ȃɂ����Ȃ�
	if (m_enable == false)return;

	// �A�j���[�V������i�s���AmDelta��Root�{�[����
	// �ω��ʂ̍s�񂪓����Ă���
	KdMatrix mDelta;
	if (m_onScriptProc) {
		m_anim.Animation(1.0, &mDelta, m_onScriptProc);
	}
	else {
		m_anim.Animation(1.0, &mDelta);
	}
	// �ω��ʍs�����������
	KdMatrix m = mDelta * GetOwner()->GetMatrix();
	GetOwner()->SetMatrix(m, false);


	// �S�{�[�����Čv�Z
	m_bc.CalcBoneMatrix();
	// �S�{�[���̍s���萔�o�b�t�@�ɏ�������
	m_bc.WriteBoneConstantBuffer();
}

void ModelComponent::Draw()
{
	// ���ܖ����̎��̂�
	if (m_enableRefract == true)return;

	// �������͂Ȃɂ����Ȃ�
	if (m_enable == false)return;
	// ���f�����Ȃ��Ƃ��͕`�悵�Ȃ�
	if (m_model == nullptr)return;

	// ���ʕ\��
	if (m_enableWater)
	{
		// �g�@���}�b�v���Z�b�g
		ShMgr.m_ModelSh.SetForceUseNormalMap(GameMgr.GetWaveTex().m_rtNormal);
	}
	// ���ܕ\��On/Off
	ShMgr.m_ModelSh.SetRefractEnable(false);
	// �����F�Z�b�g
	ShMgr.m_ModelSh.SetMulColor(m_mulColor);
	// �t�H�O��On/Off
	ShMgr.m_ModelSh.SetFogEnable(m_enableFog);
	// ���C�g��On/Off
	ShMgr.m_ModelSh.SetLightEnable(m_lightEnable);
	// �e��
	ShMgr.m_ModelSh.SetRoughness(m_roughness);
	// �����x
	ShMgr.m_ModelSh.SetMetallic(m_metallic);

	// �V�F�[�_�ɍs����Z�b�g
	ShMgr.m_ModelSh.SetWorld(GetOwner()->GetMatrix());
	// �V�F�[�_�ŕ`��
	//ShMgr.m_KdModelSh.DrawGameModel(*m_model, &m_bc);
	ShMgr.m_ModelSh.DrawGameModel(*m_model, &m_bc);

	if (m_enableWater)
	{
		// �g�@���}�b�v������
		ShMgr.m_ModelSh.SetForceUseNormalMap(nullptr);
	}
	// �����F
	ShMgr.m_ModelSh.SetMulColor(KdVec4{ 1,1,1,1 });
}

void ModelComponent::DrawTransparency()
{
	// ���܂���̎��̂�
	if (m_enableRefract == false)return;

	// �������͂Ȃɂ����Ȃ�
	if (m_enable == false)return;
	// ���f�����Ȃ��Ƃ��͕`�悵�Ȃ�
	if (m_model == nullptr)return;

	// ���ܕ\��On/Off
	ShMgr.m_ModelSh.SetRefractEnable(true);
	// ���ʕ\��
	if (m_enableWater)
	{
		// �g�@���}�b�v���Z�b�g
		ShMgr.m_ModelSh.SetForceUseNormalMap(GameMgr.GetWaveTex().m_rtNormal);
	}
	// �����F�Z�b�g
	ShMgr.m_ModelSh.SetMulColor(m_mulColor);
	// �t�H�O��On/Off
	ShMgr.m_ModelSh.SetFogEnable(m_enableFog);
	// ���C�g��On/Off
	ShMgr.m_ModelSh.SetLightEnable(m_lightEnable);
	// �e��
	ShMgr.m_ModelSh.SetRoughness(m_roughness);
	// �����x
	ShMgr.m_ModelSh.SetMetallic(m_metallic);

	// �V�F�[�_�ɍs����Z�b�g
	ShMgr.m_ModelSh.SetWorld(GetOwner()->GetMatrix());
	// �V�F�[�_�ŕ`��
	//ShMgr.m_KdModelSh.DrawGameModel(*m_model, &m_bc);
	ShMgr.m_ModelSh.DrawGameModel(*m_model, &m_bc);

	if (m_enableWater)
	{
		// �g�@���}�b�v������
		ShMgr.m_ModelSh.SetForceUseNormalMap(nullptr);
	}
	// �����F
	ShMgr.m_ModelSh.SetMulColor(KdVec4{ 1,1,1,1 });
	ShMgr.m_ModelSh.SetRefractEnable(false);
}

void ModelComponent::DrawShadowMap()
{
	// �������͂Ȃɂ����Ȃ�
	if (m_enable == false)return;
	// ���f�����Ȃ��Ƃ��͕`�悵�Ȃ�
	if (m_model == nullptr)return;

	// �V�F�[�_�ɍs����Z�b�g
	ShMgr.m_genShadowMapSh.SetWorld(GetOwner()->GetMatrix());
	// �V�F�[�_�ŕ`��
	ShMgr.m_genShadowMapSh.DrawGameModelDepth(*m_model);
}

void ModelComponent::ImGuiUpdate()
{
	// �p�����N���X��ImGuiUpdate�����s
	BaseComponent::ImGuiUpdate();

	// ���f���t�@�C��
	if (ImGuiResourceButton("Model File", m_modelFilename, { ".xed" })) {
		SetModel(m_modelFilename);
	}

	//---------------------
	// �Đ��A�j����
	//---------------------
	if (ImGui::ListBoxHeader("Play Anime", ImVec2(0, 100)))
	{
		if (ImGui::Selectable(u8"-- �Đ����Ȃ� --")) {
			m_playAnimename = "";
			m_anim.ResetTrack();
		}
		// �S�A�j����
		for (auto&& ani : m_anim.GetAnimeList()) {

			// �I�𒆁H
			bool bSelected = false;
			if (m_playAnimename == ani->m_AnimeName) {
				bSelected = true;
			}

			if (ImGui::Selectable(filesystem::path(ani->m_AnimeName).u8string().c_str(), bSelected)) {
				// �I�����ꂽ�A�j�������L��
				m_playAnimename = ani->m_AnimeName;
				// �A�j���ύX
				m_anim.ChangeAnime(m_playAnimename, m_loopAnime);
			}
		}

		ImGui::ListBoxFooter();
	}

	// ���[�v�Đ�
	if (ImGui::Checkbox("Loop Anime", &m_loopAnime)) {
		m_anim.GetTrack()[0]->m_Loop = m_loopAnime;
	}

	//========================================
	// �O���t�B�b�N�X�ݒ�
	//========================================
	// ���C�g��On/Off
	ImGui::Checkbox(u8"���C�g��On/Off", &m_lightEnable);
	// 
	ImGui::SliderFloat(u8"�e��", &m_roughness, 0, 1);
	// �����x
	ImGui::SliderFloat(u8"�����x", &m_metallic, 0, 1);
	// ���ʕ\��
	ImGui::Checkbox(u8"���ʕ\����On/Off", &m_enableWater);
	// ���ܕ\��
	ImGui::Checkbox(u8"���ܕ\����On/Off", &m_enableRefract);
	// �����F�w��
	ImGui::ColorEdit4(u8"�F", m_mulColor);
	// �t�H�OOn/Off
	ImGui::Checkbox(u8"�t�H�O", &m_enableFog);

	// ��filesystem::path(������).u8string()�@�͕������utf8�̕����R�[�h�֕ϊ����Ă��鏈���ł��B
}

void ModelComponent::EditorDraw()
{
	if (m_enable == false)return;
	if (m_model == nullptr)return;

	// ���݂̕`��X�e�[�g��ۑ����A
	// ���̊֐����I���Ƃ��ɕ��������
	KdStateSaver stateSaver;
	stateSaver.SaveDS();
	stateSaver.SaveRS();

	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareOFF_ZWriteOFF, 0);
	// ���C���[�t���[�����[�h�ɂ���
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_WireframeBoth);

	ShMgr.m_KdPrimSh.SetTexture(nullptr);

	for (auto&& model : m_model->GetModels()) {
		ShMgr.m_KdPrimSh.DrawMesh(*model->GetMesh(), { 1, 1, 1, 1 }, &GetOwner()->GetMatrix());
	}
}
