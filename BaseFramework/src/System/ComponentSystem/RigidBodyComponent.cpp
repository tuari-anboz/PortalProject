#include "main.h"

#include "../GameManager.h"

#include "RigidBodyComponent.h"

void RigidBodyComponent::Start()
{
	// 
	UpdateRigidBody();
}

void RigidBodyComponent::Update()
{
	// ���̂ƃQ�[�����̍s��𓯊�������
	if (GetOwner())
	{
		if (m_body)
		{
			m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			// �L�l�}�e�B�b�N�ȃI�u�W�F�N�g�̓v���O�������œ�������
			if (m_isKinematic)
			{
				m_body->SetMatrix(GetOwner()->GetMatrix());
			}
			else
			{
				KdMatrix m;
				m_body->GetMatrix(m);
				GetOwner()->SetMatrix(m, false);
			}
		}
	}
}

void RigidBodyComponent::ImGuiUpdate()
{
	// �L��/����
	if (ImGui::Checkbox("Enable", &m_enable))
	{
		// ���͖̂����̎��������[���h����폜����
		SetEnable(m_enable);
	}
	//
	if (ImGui::Button(u8"�X�V"))
	{
		UpdateRigidBody();
	}
	//
	ImGui::Checkbox(u8"Is Kinematic", &m_isKinematic);
	// ����
	ImGui::DragFloat(u8"����", &m_mass, 0.01f, 0, 100);
	// ���C��
	ImGui::DragFloat(u8"���C��", &m_friction, 0.01f, 0, 100);
	// ������
	ImGui::DragFloat(u8"������", &m_restitution, 0.01f, 0, 100);
	// ����O���[�v
	ImGui::InputInt(u8"����O���[�v", &m_group);
	// ����Ȃ��I�u�W�F�N�g(���炭���̃I�u�W�F�N�g�������ς�����Ƃ߂�����d���Ȃ�܂�)
	ImGui::Checkbox(u8"Is NonSleep", &m_NonSleep);
}

void RigidBodyComponent::UpdateRigidBody()
{
	if (m_body)
	{
		m_body->RemoveFromWorld();
	}

	// ������̃R���C�_�[����`����擾���Ă���
	auto boxcolcomp = GetOwner()->GetComponent<BoxColliderComponent>();
	if (boxcolcomp)
	{
		// �����G���W���̌`��ɃR���C�_�[���g�p���邩
		if (boxcolcomp->UsePhysics())
		{
			KdMatrix m;
			m = boxcolcomp->GetRotaMat();
			m.SetScale(boxcolcomp->GetOwner()->GetMatrix().GetScale());
			m.SetPos(boxcolcomp->GetBox().Center);

			// ���`����쐬
			SPtr<KdPhysicsShape_Box> boxShape = std::make_shared<KdPhysicsShape_Box>();
			boxShape->Create(boxcolcomp->GetLocalSize(), boxcolcomp->GetLocalSize());

			// ���̌`��̍��̂𐶐�
			m_body = std::make_shared<KdPhysicsRigidBody>();
			m_body->Create(boxShape, m, m_mass);	// ���̂̍쐬 ���ʂ�0���傫���l���w�肷��ƕ����������s����
			m_body->SetFriction(m_friction);		// ���C��
			m_body->SetRestitution(m_restitution);	// ������
			m_body->SetLinearDamping(0.0f);
			m_body->SetAngularDamping(0.050f);

			// ���̂��X���[�v��Ԃɂ����Ȃ�
			if(m_NonSleep)m_body->SetNonSleep();

			// �R���|�[�l���g���L���Ȏ��̂ݍ��̂𕨗����[���h�֒ǉ�����
			if (m_enable)
			{
				// �������[���h�֍��̂�ǉ�����
				m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			}
		}
	}

	// ������̃R���C�_�[����`����擾���Ă���
	auto meshcolcomp = GetOwner()->GetComponent<MeshColliderComponent>();
	if (meshcolcomp)
	{
		// �����G���W���̌`��ɃR���C�_�[���g�p���邩
		if (meshcolcomp->UsePhysics())
		{
			// �s��擾
			KdMatrix m;
			m = meshcolcomp->GetMatrix();

			auto models = meshcolcomp->GetModel()->GetModels();
			// ��ԍŏ��̃��b�V���݂̂𕨗��`��Ƃ��ė��p����
			auto mesh = models[0]->GetMesh();

			// ���b�V���`��𐶐�
			SPtr<KdPhysicsShape_Mesh> meshShape = std::make_shared<KdPhysicsShape_Mesh>();
			meshShape->CreateFromMesh(*mesh);

			// ���̌`��̍��̂𐶐�
			m_body = std::make_shared<KdPhysicsRigidBody>();
			m_body->Create(meshShape, m, 0.0f);		// Mesh�`��́A����0�łȂ���΂Ȃ�Ȃ�(���������͂ł��Ȃ��Œ蕨�ɂȂ�)
			m_body->SetKinematic(m_isKinematic);		// �����œ������悤�ȍ��̂̓L�l�}�e�B�b�N��(BOXGEN�̃h�A�Ȃ�)

			m_body->SetFriction(m_friction);		// ���C��
			m_body->SetRestitution(m_restitution);		// ������

			// ���̂��X���[�v��Ԃɂ����Ȃ�
			if (m_NonSleep)m_body->SetNonSleep();

			// �R���|�[�l���g���L���Ȏ��̂ݍ��̂𕨗����[���h�֒ǉ�����
			if (m_enable)
			{
				// �������[���h�֍��̂�ǉ�����
				m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			}
		}
	}
}

void RigidBodyComponent::SetCollisionGroup(const int group, bool update)
{
	m_group = group;

	if (update)
	{
		m_body->RemoveFromWorld();

		// �R���|�[�l���g���L���Ȃ獄�̂𕨗����[���h�֒ǉ�����
		if (m_enable)
		{
			m_body->AddToWorld(GameMgr.m_physicsWorld, group, 1);
		}
	}
}

void RigidBodyComponent::SetEnable(bool enable)
{
	if (enable)
	{
		m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
	}
	else
	{
		m_body->RemoveFromWorld();
	}

	m_enable = enable;
}

void RigidBodyComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_isKinematic = jsonObj["IsKinematic"].bool_value();
	m_mass = (float)jsonObj["Mass"].number_value();
	m_friction = (float)jsonObj["Friction"].number_value();
	m_restitution = (float)jsonObj["Restitution"].number_value();
	m_group = jsonObj["CollisionGroup"].int_value();
}

void RigidBodyComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["IsKinematic"] = m_isKinematic;
	outJsonObj["Mass"] = m_mass;
	outJsonObj["Friction"] = m_friction;
	outJsonObj["Restitution"] = m_restitution;
	outJsonObj["CollisionGroup"] = m_group;
}
