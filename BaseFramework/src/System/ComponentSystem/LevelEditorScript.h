#pragma once

#include "BaseComponent.h"

class LevelEditorScript : public BaseComponent
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
	// ��������v���n�u��
	std::string m_generationPrefabName = "";
	// ��������v���C���[��
	std::string m_playerPrefabName = "";
	// �I�������I�u�W�F�N�g���R�s�[���邩
	bool m_isCopy = true;
	// �I�������I�u�W�F�N�g�Z�b�g
	WPtr<Object> m_selectedObjectSet;
	// �I�������I�u�W�F�N�g
	WPtr<Object> m_selectedObject;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseLevelEditorAction : public std::enable_shared_from_this<BaseLevelEditorAction> {
	public:
		virtual ~BaseLevelEditorAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseLevelEditorAction> Update() = 0;
		// �L�����ւ̃A�h���X
		LevelEditorScript*	m_pObject = nullptr;
	};
	// �u��{�v�s��
	class Basic : public BaseLevelEditorAction {
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// �u�I�𒆁v�s��
	class Selected : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// �u�쐬�v�s��
	class Create : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// �u�ҏW�v�s��
	class Edit : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// �u�u���v�s��
	class Replace : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// ���݂̍s��
	SPtr<BaseLevelEditorAction>	m_as = nullptr;
};