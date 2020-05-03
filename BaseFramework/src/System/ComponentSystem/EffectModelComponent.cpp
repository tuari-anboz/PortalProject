#include "main.h"
#include "../GameManager.h"
#include "EffectModelComponent.h"


void EffectModelComponent::DrawEffect()
{
	if (m_enable == false)return;

	if (m_model) {
		// �s��
		KdMatrix m = GetOwner()->GetMatrix();

		// �r���{�[�h
		if (m_billboardType == 1) {
			KdMatrix mCam;
			ShMgr.m_mView.Inverse(mCam);

			m.SetRotation(mCam);
			m.Scale_Local(GetOwner()->GetMatrix().GetScale());
		}
		// Y���Œ�r���{�[�h
		else if (m_billboardType == 2) {
			// �J�����ւ̕���
			KdMatrix mCam;
			ShMgr.m_mView.Inverse(mCam);

			KdVec3 vCam = mCam.GetPos() - m.GetPos();
			KdVec3 vY = m.GetYAxis();
			KdVec3 vX = vCam.Cross(vY);
			KdVec3 vZ = vY.Cross(vX);
			vX.Normalize();
			vY.Normalize();
			vZ.Normalize();
			m._11 = vX.x;
			m._12 = vX.y;
			m._13 = vX.z;
			m._21 = vY.x;
			m._22 = vY.y;
			m._23 = vY.z;
			m._31 = vZ.x;
			m._32 = vZ.y;
			m._33 = vZ.z;
			m.Scale_Local(GetOwner()->GetMatrix().GetScale());
		}

		// ����������
		if (m_blendType == 0) {
		}
		else if (m_blendType == 1) {
			float bf[4] = { 0,0,0,0 };
			KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAdd, bf, 0xFFFFFFFF);
		}

		// �`��
		ShMgr.m_KdPrimSh.SetTexture(m_tex.get());
		ShMgr.m_KdPrimSh.DrawMesh(*m_model->GetModels()[0]->GetMesh(), m_color, &m);
		ShMgr.m_KdPrimSh.SetTexture(nullptr);

		// �����������ɂ��ǂ��Ă���
		float bf[4] = { 0,0,0,0 };
		KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAlpha, bf, 0xFFFFFFFF);
	}
}

void EffectModelComponent::ImGuiUpdate()
{
	//
	BaseComponent::ImGuiUpdate();

	// �r���{�[�h���@
	std::vector<char*> bbTypes = { u8"�Ȃ�", u8"�ʏ�r���{�[�h", u8"Y���Œ�r���{�[�h" };
	ImGui::Combo("Billboard Type", &m_billboardType, &bbTypes[0], bbTypes.size());

	// �u�����h���@
	std::vector<char*> blendtypes = { u8"����������", u8"���Z����" };
	ImGui::Combo("Blend Type", &m_blendType, &blendtypes[0], blendtypes.size());

	// �F
	ImGui::ColorEdit4(u8"�F", m_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);

	bool bChanged = false;

	// �`��Model�t�@�C��
	if (ImGuiResourceButton(u8"���f��", m_modelFilename, { ".xed" })) {
		bChanged = true;
	}
	ImGuiShowHelp(u8"�`��p��3D���f����I��", false);

	// �`��Model�t�@�C��
	if (ImGuiResourceButton(u8"�e�N�X�`��", m_texFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" })) {
		bChanged = true;
	}
	ImGuiShowHelp(u8"����t����摜�t�@�C����I��", false);

	// �Đݒ�
	if (bChanged) {
		LoadModel(m_modelFilename, m_texFilename);
	}
}
