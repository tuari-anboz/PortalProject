#pragma once

#include "BaseComponent.h"


class BoxGeneratorComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// ������
	bool m_isGenerating = false;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseBoxGeneratorAction : public std::enable_shared_from_this<BaseBoxGeneratorAction> {
	public:
		virtual ~BaseBoxGeneratorAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseBoxGeneratorAction> Update() = 0;
		// �L�����ւ̃A�h���X
		BoxGeneratorComponent*	m_pComp = nullptr;
	};
	// �u�}�p�v�s��
	class Generic : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};
	// �u�I���v�s��
	class ON : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};
	// �u�I�t�v�s��
	class OFF : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseBoxGeneratorAction>	m_as = nullptr;
};
