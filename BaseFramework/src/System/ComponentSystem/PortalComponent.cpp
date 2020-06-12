#include "main.h"

#include "../GameManager.h"

#include "PortalComponent.h"

void PortalComponent::Update()
{
	// �I�[�i�[�擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �|�[�^���̃��C�R���C�_�[�����ׂĎ擾
	auto colliders = owner->GetComponents<RayColliderComponent>();
	for (auto&& coll : colliders)
	{
		if (coll->GetTag() == "Top")
		{
			coll->m_onHitEnter = [this](BaseColliderComponent* collider)
			{
				// �q�b�g���J�E���^�[
				int hitCounter = 0;
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() == "RePosPortal")
					{
						hitCounter++;
					}
				}

				// �q�b�g�񐔂���񂵂����������ꍇ�|�[�^���𐳂����ʒu�ɒ���
				if (hitCounter == 1)
				{

				}
			};
		}

		if (coll->GetTag() == "Bottom")
		{

		}

		if (coll->GetTag() == "Right")
		{

		}

		if (coll->GetTag() == "Left")
		{

		}
	}
}

void PortalComponent::ImGuiUpdate()
{
}

void PortalComponent::Deserialize(const json11::Json & jsonObj)
{
}

void PortalComponent::Serialize(json11::Json::object & outJson)
{
}
