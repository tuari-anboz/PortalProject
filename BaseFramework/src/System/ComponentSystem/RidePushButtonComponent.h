#pragma once

#include "BaseComponent.h"

// RidePushButtonComponent
// ���Ɣ�������{�^��
// �Ή�����I�u�W�F�N�g(�h�A,�{�b�N�X���o���I�u�W�F�N�g��)��
// �����N���ē��삳���鎖���o����悤�ɂ���H
// �����N�����I�u�W�F�N�g�̎q�Ƃ��ē�����������������
// �A�j���[�V������������Ԃɖ߂�܂Ŕ�����Ԃɂ���
// 

class RidePushButtonComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	// 
	void OnPush();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:

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
		RidePushButtonComponent*	m_pComp = nullptr;
	};
	// �u��{�v�s��
	class OFF : public BaseRidePushButtonAction {
	public:
		virtual SPtr<BaseRidePushButtonAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseRidePushButtonAction>	m_as = nullptr;
};