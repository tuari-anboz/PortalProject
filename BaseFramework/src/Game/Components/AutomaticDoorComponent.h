#pragma once

#include "System/ComponentSystem/BaseComponent.h"

// �����h�A�R���|�[�l���g
// �����蔻��̐���⋓����S������
namespace AUTODOOR
{
	enum
	{
		CLOSE = 0,
		OPEN = 1
	};
};


class AutomaticDoorComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	// �h�A�̊J��
	void Open();
	void Close();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// �h�A�̏��
	int m_state = AUTODOOR::CLOSE;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseAutomaticDoorAction : public std::enable_shared_from_this<BaseAutomaticDoorAction> {
	public:
		virtual ~BaseAutomaticDoorAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseAutomaticDoorAction> Update() = 0;
		// �L�����ւ̃A�h���X
		AutomaticDoorComponent*	m_pComp = nullptr;
	};
	// �u�}�p�v�s��
	class Generic : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};
	// �u�J���v�s��
	class OpenAction : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};
	// �u�܂�v�s��
	class CloseAction : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseAutomaticDoorAction>	m_as = nullptr;
};