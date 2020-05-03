#include "main.h"

#include "System/GameManager.h"

#include "GettingBoxComponent.h"

void GettingBoxComponent::Start()
{
	m_as = std::make_shared<Basic>();
}

void GettingBoxComponent::Update()
{
	// ������擾
	auto owner = GetOwner();

	// �s������
	if (m_as)
	{
		// ������̃L�����̃A�h���X�����Ă���
		m_as->m_pComp = this;
		// �s���������s
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}

}

void GettingBoxComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void GettingBoxComponent::GetBox()
{
	auto owner = GetOwner();

	// �{�b�N�X���擾���Ă����烊���[�X���܂��擾���Ă��Ȃ��Ȃ�擾����
	if (m_GettingBox)
	{
		// �{�b�N�X���擾���Ă��Ȃ���Ԃɂ���
		m_GettingBox = false;
	}
	else
	{
		auto colls = owner->GetComponents<RayColliderComponent>();
		for (auto&& coll : colls)
		{
			if (coll->GetTag() == "GettingBox")
			{
				coll->m_onHitStay = [this, coll](BaseColliderComponent* collider)
				{
					for (auto&& res : collider->GetResults())
					{
						if (res.Collider->GetTag() == "GettingBox")
						{
							auto owner = GetOwner();

							// �{�b�N�X��WP���i�[
							m_pBox = res.Collider->GetOwner()->weak_from_this();

							// �{�b�N�X�̍��̂��I�t��
							auto rigidbody = collider->GetOwner()->GetComponent<RigidBodyComponent>();
							if (rigidbody)
							{
								rigidbody->SetEnable(false);
							}
							// �{�b�N�X���擾������Ԃɂ���
							m_GettingBox = true;

							// �Ώۂ��A�N�e�B�u������
							res.Collider->GetOwner()->SetActive(false);
						}
					}
				};
			}
		}
	}
}

void GettingBoxComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void GettingBoxComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Basic::Update()
{
	// �{�b�N�X���擾���Ɏ擾���s���ɐ؂�ւ���
	if (m_pComp->m_GettingBox)
	{
		return std::make_shared<Get>();
	}

	return nullptr;
}

SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Get::Update()
{
	auto owner = m_pComp->GetOwner();

	// �{�b�N�X���폜����Ă������{�s���ɖ߂�
	if (m_pComp->m_pBox.expired())
	{
		m_pComp->m_GettingBox = false;
		return std::make_shared<Basic>();
	}
	// �{�b�N�X�𗣂������s����؂�ւ���
	if (m_pComp->m_GettingBox == false)
	{
		return std::make_shared<Release>();
	}

	// �{�b�N�X�̍s���������Ɠ����s��ɂ���
	m_pComp->m_pBox.lock()->SetMatrix(owner->GetMatrix(), false);

	return nullptr;
}

// 
SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Release::Update()
{
	auto owner = m_pComp->GetOwner();

	// �{�b�N�X���폜����Ă������{�s���ɖ߂�
	if (m_pComp->m_pBox.expired())
	{
		m_pComp->m_GettingBox = false;
		return std::make_shared<Basic>();
	}

	// �{�b�N�X���A�N�e�B�u��Ԃɂ���
	auto box = m_pComp->m_pBox.lock();
	if (box)
	{
		box->SetActive(true);
	}
	// ���̂��I���ɂ��Ď��_�̕����֓�����
	auto rigidbody = box->GetComponent<RigidBodyComponent>();
	if (rigidbody)
	{
		rigidbody->SetEnable(true);
		rigidbody->UpdateRigidBody();
		rigidbody->GetRigidBody()->SetMatrix(owner->GetMatrix());
		KdVec3 throwVec = owner->GetMatrix().GetZAxis();
		throwVec.Normalize();
		rigidbody->SetLinearVelocity(throwVec * 2.0f);
	}

	m_pComp->m_GettingBox = false;
	m_pComp->m_pBox.reset();

	return std::make_shared<Basic>();
}
