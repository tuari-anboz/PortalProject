#pragma once

#include "System/ComponentSystem/BaseComponent.h"

// �d�͔��R���|�[�l���g
// �����蔻��̐���⋓����S������

class GettingBoxComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	// 
	void GetBox();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// �{�b�N�X���擾���Ă��邩�ǂ���
	bool m_GettingBox = false;

	// �擾���Ă���Box
	WPtr<Object> m_pBox;

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseGettingBoxAction : public std::enable_shared_from_this<BaseGettingBoxAction> {
	public:
		virtual ~BaseGettingBoxAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseGettingBoxAction> Update() = 0;
		// �L�����ւ̃A�h���X
		GettingBoxComponent*	m_pComp = nullptr;
	};
	// �u��{�v�s��
	class Basic : public BaseGettingBoxAction 
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};
	// �u�擾�v�s��
	class Get : public BaseGettingBoxAction
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};
	// �u�����[�X�v�s��
	class Release : public BaseGettingBoxAction
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};

	SPtr<BaseGettingBoxAction> m_as = nullptr;
};