#pragma once

#include "BaseComponent.h"

// DoorComponent
// �h�A���J����R���|�[�l���g
// �q�̃h�A�I�u�W�F�N�g�̃A�j���[�V�����Ǘ��Ȃǂ�����

namespace DOOR
{
	enum
	{
		CLOSE = 0,
		OPEN = 1
	};
};

class DoorComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	//
	void Open();

	//
	void Close();

	// ��Ԃ��擾
	const int GetState() { return m_state; }

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// 
	int m_state = DOOR::CLOSE;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseDoorAction : public std::enable_shared_from_this<BaseDoorAction> {
	public:
		virtual ~BaseDoorAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseDoorAction> Update() = 0;
		// �L�����ւ̃A�h���X
		DoorComponent*	m_pComp = nullptr;
	};
	// �u�}�p�v�s��
	class Generic : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};
	// �u�I���v�s��
	class ON : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};
	// �u�I�t�v�s��
	class OFF : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseDoorAction>	m_as = nullptr;
};