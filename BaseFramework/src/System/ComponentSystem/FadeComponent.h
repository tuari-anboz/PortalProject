#pragma once

#include "BaseComponent.h"

// �t�F�[�h�C��/�t�F�[�h�A�E�g������R���|�[�l���g
// GameManager�̃t���O�Ō��݂̃t�F�[�h�󋵂𔻒f����
// �X�e�[�W�J�n���Ȃǂɂ͓K�؂Ƀt�F�[�h�C������������
// �ʂ̃R���|�[�l���g�ŌĂяo���ĔC�ӂ̃^�C�~���O�Ńt�F�[�h�A�E�g������

// �g����
// �{�^��������������state��ύX���ăt�F�[�h�A�E�g��������V�[����؂�ւ���

// �q�̃X�v���C�g�R���|�[�l���g�I�u�W�F�N�g

class FadeComponent : public BaseComponent
{
public:
	virtual void Start() override;

	virtual void Update() override;

	virtual void ImGuiUpdate() override;

	// �t�F�[�h�C��
	void FadeIn();
	// �t�F�[�h�A�E�g
	void FadeOut();

	virtual void Deserialize(const json11::Json& jsonObj)override;

	virtual void Serialize(json11::Json::object& outJson)override;
private:
	// �t�F�[�h�C���A�E�g���̓����x
	float m_fadeAlpha = 1.0f;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseFadeAction : public std::enable_shared_from_this<BaseFadeAction> {
	public:
		virtual ~BaseFadeAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseFadeAction> Update() = 0;
		// �L�����ւ̃A�h���X
		FadeComponent*	m_pComp = nullptr;
	};
	// �u��{�v�s��
	class Basic : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};
	//�u�t�F�[�h�C���v�s��
	class FadeInAction : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};
	//�u�t�F�[�h�A�E�g�v�s��
	class FadeOutAction : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};

	// ���݂̍s��
	SPtr<BaseFadeAction>	m_as = nullptr;

};