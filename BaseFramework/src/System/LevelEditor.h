#pragma once

class LevelEditor
{
public:
	LevelEditor(){}

	void Init();

	void Update();

	// 
	void ImGuiUpdate();

	void Deserialize(const json11::Json& jsonObj);

	void Serialize(json11::Json::object& outJsonObj);


private:

	// �I�u�W�F�N�g���R�s�[���邩�ǂ���
	bool m_isCopy = true;

	// �����A�u������v���n�u�t�@�C����
	std::string m_generationPrefabName = "";

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
		LevelEditor*	m_pObject = nullptr;
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
	// �u�u���v�s��
	class Replace : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};

	// ���݂̍s��
	SPtr<BaseLevelEditorAction>	m_as = nullptr;
};