#include "main.h"

#include "../Gamemanager.h"

#include "ShotComponent.h"

void ShotComponent::Update()
{
	if (m_enable == false)return;
	auto owner = GetOwner();

	// 判定
	{
		// コライダーコンポーネントを「全て」取得する
		auto colliders = owner->GetComponents<RayColliderComponent>();
		for (auto&& coll : colliders)
		{
			// 射撃系の判定
			if (coll->GetTag() == m_tag)
			{
				// ヒット時の実行される関数を登録
				coll->m_onHitStay = [this,owner](BaseColliderComponent* collider)
				{
					// ヒットした奴ら(Collider)全員
					for (auto&& res : collider->GetResults())
					{
						m_hitPos = res.Pos;
					}
				};
			}
		}
	}
}

void ShotComponent::Draw()
{
}

void ShotComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	ImGuiInputString("Tag", m_tag);
}

void ShotComponent::EditorDraw()
{
	if (m_enable == false)return;


}

void ShotComponent::Deserialize(const json11::Json & jsonObj)
{
	{
		// 継承元のDeserializeを実行
		BaseComponent::Deserialize(jsonObj);

		m_range = (float)jsonObj["Range"].number_value();
		m_dir.Deserialize(jsonObj["Dir"]);
		m_hitPos.Deserialize(jsonObj["HitPos"]);
		m_shotPos.Deserialize(jsonObj["ShotPos"]);
	}
}

void ShotComponent::Serialize(json11::Json::object & outJson)
{
		// 継承元のSerializeを実行
		BaseComponent::Serialize(outJson);

		outJson["Range"] = m_range;
		m_dir.Serialize();
		m_hitPos.Serialize();
		m_shotPos.Serialize();
}



