#pragma once

#include "BaseComponent.h"

namespace PhiysicsCollide
{
}

// RigidBodyComponent
// ���������̍��̂𐶐������肷��
// 
// 
class RigidBodyComponent : public BaseComponent
{
public:
	virtual void Start()override;
	virtual void Update()override;
	// ImGui����
	virtual void ImGuiUpdate() override;

	//
	void UpdateRigidBody();

	// rigidbody�擾
	SPtr<KdPhysicsRigidBody>& GetRigidBody() { return m_body; }

	// ���̂ɗ͂�������
	void SetLinearVelocity(const KdVec3& vec)
	{
		m_body->SetLinearVelocity(vec);
	}

	// ���̂̔���O���[�v��ύX����
	// update : �ύX�����㍄�̂��ēx�������[���h�ɒǉ�����t���O
	void SetCollisionGroup(const int group, bool update = false);

	// ���̂̓R���|�[�l���g�̗L��/������؂�ւ������ɕ������[���h����폜����
	void SetEnable(bool enable)override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// ����
	SPtr<KdPhysicsRigidBody> m_body = nullptr;

	// �L�l�}�e�B�b�N����On/Off
	bool m_isKinematic = false;
	// ����
	float m_mass = 0;
	// ���C��
	float m_friction = 0;
	// ������
	float m_restitution = 0;
	// ����O���[�v
	int m_group = 1;
	// ����}�X�N
	int m_mask = 0xFFFFFFFF;
	// �m���X���[�v
	bool m_NonSleep = false;
};