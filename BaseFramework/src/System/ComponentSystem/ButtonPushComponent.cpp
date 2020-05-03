#include "main.h"

#include "../Gamemanager.h"

#include "ButtonPushComponent.h"

void ButtonPushComponent::Update()
{
	// 持ち主取得
	auto owner = GetOwner();

	// 対ボタンスイッチ
	{
		// レイ判定コライダーコンポーネントを取得
		auto collider = owner->GetComponent<RayColliderComponent>();
		
		if (collider)
		{
			// ヒット時の実行される関数を登録
			collider->m_onHitStay;
		}
	}
	
}

void ButtonPushComponent::ImGuiUpdate()
{
}

void ButtonPushComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void ButtonPushComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);
}
