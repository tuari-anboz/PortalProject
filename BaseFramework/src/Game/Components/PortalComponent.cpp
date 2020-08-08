#include "main.h"

#include "System/GameManager.h"

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
				const CollisionResult* result;
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() == "RePosPortal")
					{
						hitCounter++;
						result = &res;
					}
				}

				if (hitCounter == 1)
				{
					KdMatrix m = GetOwner()->GetMatrix();
					m.Move(KdVec3(0, result->Dist - 1.001f, 0));
					
					GetOwner()->SetMatrix(m, false);
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
	BaseComponent::ImGuiUpdate();
}

void PortalComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void PortalComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);
}
