#pragma once

#include "BaseComponent.h"

// LoadLevelComponent
// ���x���̃��[�h�𐧌䂷��R���|�[�l���g
// ���x���؂�ւ����̓����S�����܂�
// FadeComponent���g�p���ă��x���؂�ւ����ɓK�؂ȓ����������

// �g����
// ���̃R���|�[�l���g�ɐ؂�ւ��������x���̃t�@�C������n��
// �t�F�[�h�C���t�F�[�h�A�E�g�⃍�[�h��ʂ��o�����I������
class LoadLevelComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	// ���x���̃��[�h
	void LoadLevel(const std::string& levelfilename);

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// �؂�ւ���̃��x���t�@�C����
	std::string m_levelFilename = "";

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseLoadLevelAction : public std::enable_shared_from_this<BaseLoadLevelAction> {
	public:
		virtual ~BaseLoadLevelAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseLoadLevelAction> Update() = 0;
		// �L�����ւ̃A�h���X
		LoadLevelComponent*	m_pComp = nullptr;
	};
	// �u��{�v�s��
	class Generic: public BaseLoadLevelAction {
	public:
		virtual SPtr<BaseLoadLevelAction> Update() override;
	};
	//�u���x�����[�h�v�s��
	class LoadLevelAction : public BaseLoadLevelAction {
	public:
		virtual SPtr<BaseLoadLevelAction> Update() override;
	};

	// ���݂̍s��
	SPtr<BaseLoadLevelAction>	m_as = nullptr;
};