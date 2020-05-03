#pragma once

#include "BaseComponent.h"

// RideButtonComponent
// ���Ɣ�������{�^��
// �Ή�����I�u�W�F�N�g(�h�A,�{�b�N�X���o���I�u�W�F�N�g��)��
// �����N���ē��삳���鎖���o����悤�ɂ���H
// �����N�����I�u�W�F�N�g�̎q�Ƃ��ē�����������������
// �A�j���[�V������������Ԃɖ߂�܂Ŕ�����Ԃɂ���
// 

class RideButtonComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	// �I���I�t�擾
	bool& GetOnPush() { return m_isOn; }

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// �I���I�t
	bool m_isOn = false;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseRidePushButtonAction : public std::enable_shared_from_this<BaseRidePushButtonAction> {
	public:
		virtual ~BaseRidePushButtonAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseRidePushButtonAction> Update() = 0;
		// �L�����ւ̃A�h���X
		RideButtonComponent*	m_pComp = nullptr;
	};
	// �u�}�p�v�s��
	class Generic : public BaseRidePushButtonAction {
	public:
		virtual SPtr<BaseRidePushButtonAction> Update() override;
	};
	// �u�I�t�v�s��
	class OFF : public BaseRidePushButtonAction {
	public:
		virtual SPtr<BaseRidePushButtonAction> Update() override;
	};
	// �u�I���v�s��
	class ON : public BaseRidePushButtonAction {
	public:
		virtual SPtr<BaseRidePushButtonAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseRidePushButtonAction>	m_as = nullptr;
};