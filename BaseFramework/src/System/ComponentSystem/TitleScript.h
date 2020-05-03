#pragma once

#include "BaseComponent.h"

// �^�C�g���I�u�W�F�N�g�p�X�N���v�g
// �^�C�g����ʂł̑�����X�e�[�g�p�^�[����p���ĕ\������
class TitleScript : public BaseComponent {
public:
	virtual void Start() override;

	virtual void Update() override;

private:
	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseTitleAction : public std::enable_shared_from_this<BaseTitleAction> {
	public:
		virtual ~BaseTitleAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseTitleAction> Update() = 0;
		// �L�����ւ̃A�h���X
		TitleScript*	m_pComp = nullptr;
	};
	// �u��{�v�s��
	class Basic : public BaseTitleAction {
	public:
		virtual SPtr<BaseTitleAction> Update() override;
	};
	// (��) �u�X�e�[�W�Z���N�g�v�s��
	class StageSelect : public BaseTitleAction{
	public:
		virtual SPtr<BaseTitleAction> Update() override;
	};

	// ���݂̍s��
	SPtr<BaseTitleAction>	m_as = nullptr;

};
