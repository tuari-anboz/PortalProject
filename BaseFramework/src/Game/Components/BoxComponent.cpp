#include "main.h"

#include "System/GameManager.h"

#include "BoxComponent.h"

void BoxComponent::Start()
{
}

void BoxComponent::Update()
{
	// ������擾
	auto owner = GetOwner();

	auto rigidbody = owner->GetComponent<RigidBodyComponent>();

	// ���R���C�_�[�擾
	auto coll = owner->GetComponent<SphereColliderComponent>();
	if (coll)
	{
		// 
		//coll->m_onHitStay = [this,owner](BaseColliderComponent* collider)
		//{
		//	auto results = collider->GetResults();
		//	for (auto&& res : results)
		//	{
		//		if (res.Collider->GetTag() == "Collision")
		//		{
		//			KdMatrix m = owner->GetMatrix();
		//			m.Move(res.Push);	// �����߂�
		//			owner->SetMatrix(m, false);
		//		}
		//	}
		//};
		// �|�[�^���ɐG��Ă����畨������O���[�v���|�[�^���݂̂֓�����悤�ɕύX
		coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
		{
			auto results = collider->GetResults();
			for (auto&& res : results)
			{
				if (res.Collider->GetTag() == "EnterPortal")
				{
					// 2�̃|�[�^���̑��݃`�F�b�N
					auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
					if (!portal1)return;
					auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");
					if (!portal2)return;

					auto rigidbody = collider->GetOwner()->GetComponent<RigidBodyComponent>();
					if (rigidbody)
					{
						rigidbody->SetCollisionGroup(2, true);
					}
				}
			}
		};
		// �|�[�^�����痣�ꂽ�畨������O���[�v���|�[�^���ȊO�̕��ɓ�����悤�ɕύX
		coll->m_onHitExit = [this, owner](BaseColliderComponent* collider)
		{
			auto rigidbody = collider->GetOwner()->GetComponent<RigidBodyComponent>();
			if (rigidbody)
			{
				rigidbody->SetCollisionGroup(1, true);
			}

		};
	}
}

void BoxComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void BoxComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void BoxComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}