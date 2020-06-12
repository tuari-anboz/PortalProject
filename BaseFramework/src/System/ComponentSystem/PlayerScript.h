#pragma once

#include "BaseComponent.h"

// �L����AI����
class CharaAIInputComponent : public InputComponent {
public:
	virtual void Update() override {
		// �����̏ꍇ�͉������Ȃ�
		if (m_enable == false) {
			// �{�^�������Z�b�g�����s��
			m_axis = { 0,0 };
			m_buttons = 0;
			return;
		}

		// �K���ɃL�[������(0�`100�̗���)
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			if (KdMTRand::s_Rnd.GetInt(0, 1) == 0)m_axis.y = 1;
			else m_axis.y = -1;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			m_axis.y = 0;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			if (KdMTRand::s_Rnd.GetInt(0, 1) == 0)m_axis.x = 1;
			else m_axis.x = -1;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			m_axis.x = 0;
		}
	}

	// ImGui����
	virtual void ImGuiUpdate() override {
		// �p�����̊֐������s
		InputComponent::ImGuiUpdate();
		// �����̓��e
		ImGui::InputInt("AI Level", &m_aiLevel);
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// �p�����̊֐������s����
		InputComponent::Deserialize(jsonObj);

		m_aiLevel = jsonObj["AILevel"].int_value();
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJson) override {
		// �p�����̊֐������s����
		InputComponent::Serialize(outJson);

		outJson["AILevel"] = m_aiLevel;
	}


private:
	int		m_aiLevel = 1;	// AI�̎v�l���x��
};

// �l�ԃL����
class PlayerScript : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// Hp�Z�b�g
	void SetHp(int hp) { m_hp = hp; }

	// �ړ��x�N�g���Q�b�g
	KdVec3& GetForce() { return m_vForce; }

	// �|�[�^���Փ˃Z�b�g
	bool& SetIsPortalCollide() { return m_isPortalCollide; }

	// �X�V����
	virtual void Update() override;

	// 
	virtual void DrawEffect() override;

	// �_���[�W��H������Ƃ�
	//virtual void OnDamage(const DamageArg& dmg, DamageReply& rep) override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	//===============================
	// Serialize / Deserialize
	//===============================
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// �p�����̂ق���Deserialize�֐������s����
		BaseComponent::Deserialize(jsonObj);

		// ���̃L�����ŗL�̃f�[�^�ݒ�

		// Hp
		m_hp = jsonObj["Hp"].int_value();
		// �ړ����x
		m_ms = (float)jsonObj["MoveSpeed"].number_value();
		// �v���n�u�t�@�C���l�[��
		m_editorPrefabName = jsonObj["PrefabName"].string_value();
	}
	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJson) override {
		// �p������Serialize�����s����
		BaseComponent::Serialize(outJson);
		// �����̃f�[�^��Serialize
		outJson["Hp"] = m_hp;
		outJson["MoveSpeed"] = m_ms;
		outJson["PrefabName"] = m_editorPrefabName;
	}

	// ��ԊǗ�
	enum State
	{
		STAND_STATE,
		WALK_STATE,
		SHOT_STATE,
	};

private:

	// Hp
	int					m_hp = 0;

	// �v���C���[�̈ړ����x
	float				m_ms = 1.0f;

	// �ړ��x�N�g��(�L�����ɂ������Ă���̓x�N�g��)
	KdVec3				m_vForce;

	// �q�b�g�X�g�b�v����(�J�E���^�[)
	int					m_hitStop = 0;

	// �󒆁H
	bool				m_isSky = false;

	// Test
	KdLaser				m_trail;

	// �|�[�^���ɐG��Ă��邩
	bool				 m_isPortalCollide = false;

	// �G�f�B�^�[�v���n�u�t�@�C���l�[��
	std::string			m_editorPrefabName = "";

	//===========================
	// �A�N�V�����X�e�[�g�N���X
	//===========================
	class BaseAction : public std::enable_shared_from_this<BaseAction> {
	public:
		virtual ~BaseAction() {}
		// �s������
		// �߂�l�F���̍s���N���X
		virtual SPtr<BaseAction> Update() = 0;
		// �L�����ւ̃A�h���X
		PlayerScript*	m_pChara = nullptr;
	};
	// �u�����v�s��
	class Stand : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// �u�����v�s��
	class Walk : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// �u�ˌ��s���v
	class Shot : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	class Shot2 : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	class GetBox : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// �ėp�s��
	class Generic : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// ���S�s��
	class Die : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;

		int m_cnt = 0;
	};

	// �W�����v�J�n
	class JumpStart : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;

		KdVec3	m_vForce;
	};

	// �W�����v�㏸
	class JumpUp : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// �W�����v���~
	class JumpDown : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// ���n
	class Landing : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// ���݂̍s��
	SPtr<BaseAction>	m_as;

	// ��������s��
	SPtr<BaseAction>	m_asExt;
	KdAnimator			m_animExt;
};