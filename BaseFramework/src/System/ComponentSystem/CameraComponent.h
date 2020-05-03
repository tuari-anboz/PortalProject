#pragma once

#include "BaseComponent.h"


//==============================================
//
// �J�����R���|�[�l���g�N���X
//
//==============================================
class CameraComponent : public BaseComponent {
public:

	//----------------------------
	// �ݒ�E�擾
	//----------------------------
	// �ˉe�s��ݒ�
	void SetProjection(float fov, float fNear, float fFar) {
		m_fov = fov;
		m_nearFarDist.Set(fNear, fFar);
	}

	// Aspect��̐ݒ�
	void SetAspect(float _aspectratio) { m_aspect = _aspectratio; }


	// �`��O����
	virtual void PreDraw() override;

	// (Editor)Editor�`��
	virtual void EditorDraw() override;

	// (Editor)ImGui����
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// �A�N�e�B�u��ԁA�V�F�[�_�[�փJ�������Z�b�g����
	bool		m_active = true;
	// �ˉe�s��ݒ�
	float		m_fov = 60;						// ����p
	float		m_aspect = KD3D.GetBackBuffer()->GetAspect();		// �A�X�y�N�g��
	KdVec2		m_nearFarDist = {0.1f, 1000};	// �ŋߐڋ����A�ŉ�������

	// �eObject�Ɏ��t����
	bool		m_attachToParent = false;		// �e�ɒǏ]����H
	float		m_attachDistance = 5;			// ����
	KdVec3		m_attachOffset;					// �I�t�Z�b�g���W
};
