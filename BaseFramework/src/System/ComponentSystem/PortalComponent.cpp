#include "main.h"

#include "../GameManager.h"

#include "PortalComponent.h"

void PortalComponent::Update()
{
	// オーナー取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// ポータルのレイコライダーをすべて取得
	auto colliders = owner->GetComponents<RayColliderComponent>();
	for (auto&& coll : colliders)
	{
		if (coll->GetTag() == "Top")
		{
			coll->m_onHitEnter = [this](BaseColliderComponent* collider)
			{
				// ヒット数カウンター
				int hitCounter = 0;
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() == "RePosPortal")
					{
						hitCounter++;
					}
				}

				// ヒット回数が一回しか無かった場合ポータルを正しい位置に直す
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
