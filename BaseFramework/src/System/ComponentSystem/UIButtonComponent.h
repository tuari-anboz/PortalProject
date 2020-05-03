#pragma once

#include "BaseComponent.h"

//===========================================
// UIButton�R���|�[�l���g
// �E�摜�̃N���b�N�𔻒�
// �E�N���b�N��������A�N���b�N���A�N���b�N���Ȃ��Ȃ�������𔻒�ł���悤�ɂ���
// �E

namespace UIButton
{
	enum
	{
		OFF = 0x000000001,
		ONBUTTON = 0x00000002,
		PRESS = 0x00000004,
	};
}

class UIButtonComponent : public BaseComponent
{
public:
	virtual void Start() override;

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

	// �Q�b�^�[
	int GetButtons() {return m_buttons;}

private:
	// �{�^���I�����̉摜�t�@�C����
	std::string m_selectedTexFilename = "";
	// �{�^���I�����̉摜
	SPtr<KdTexture> m_buttonSelectedTex;

	// �{�^���I�����̉摜�t�@�C����
	std::string m_TexFilename = "";
	// �{�^���������ĂȂ��ꍇ�̃e�N�X�`��
	SPtr<KdTexture> m_buttonTex;

	// �{�^���̏��
	int m_buttons = 0;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseUIButtonAction : public std::enable_shared_from_this<BaseUIButtonAction> {
	public:
		virtual ~BaseUIButtonAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseUIButtonAction> Update() = 0;
		// �L�����ւ̃A�h���X
		UIButtonComponent*	m_pComp = nullptr;
	};
	// �u�I�t�v�s��
	class OFF : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// �u�I���v�s��
	class ONBUTTON : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// �u�v���X�v�s��
	class Press : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// �u�h���b�v�v�s��
	class Drop : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};


	// ���݂̍s��
	SPtr<BaseUIButtonAction>	m_as = nullptr;

};