#include "main.h"

#include "../GameManager.h"

#include "RideButtonComponent.h"

void RideButtonComponent::Start()
{
	m_as = std::make_shared<Generic>();
}

void RideButtonComponent::Update()
{
	// 持ち主取得
	auto owner = GetOwner();
	if (owner == nullptr)return;
	// モデルコンポーネント取得
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return;

	// アニメータを進める
	if (modelComp)
	{
		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// 行動処理
	if (m_as)
	{
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pComp = this;
		// 行動処理実行
		auto p = m_as->Update();
		if (p) 
		{
			m_as = p;
		}
	}
}

void RideButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void RideButtonComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void RideButtonComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::Generic::Update()
{
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;
	// モデルコンポーネント取得
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return nullptr;

	if (m_pComp->m_isOn)
	{
		return std::make_shared<ON>();
	}
	else
	{
		return std::make_shared<OFF>();
	}


	return nullptr;
}


SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::OFF::Update()
{
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;
	// モデルコンポーネント取得
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return nullptr;

	// 押されている状態なら行動を変更
	if (m_pComp->m_isOn)
	{
		return std::make_shared<ON>();
	}

	// コライダーコンポーネント取得
	auto coll = owner->GetComponent<MeshColliderComponent>();
	if (coll)
	{
		// ヒットした最初の一回のみ実行される関数を登録
		coll->m_onHitStay = [this](BaseColliderComponent* collider)
		{
			// 押されていなかったら
			if (m_pComp->m_isOn == false)
			{
				auto modelComp = m_pComp->GetOwner()->GetComponent<ModelComponent>();
				if (modelComp == nullptr)return;
				// 現在のアニメーションが終了していたら
				if (modelComp->GetAnimator().IsAnimationEnd())
				{
					// アニメーションをPUSHに変更
					modelComp->GetAnimator().ChangeAnime("Push", false);

					// 状態も変更
					m_pComp->m_isOn = true;
				}
			}
		};
	}

	return nullptr;
}

SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::ON::Update()
{
	// 持ち主を取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// 押されている状態なら行動を変更
	if (m_pComp->m_isOn == false)
	{
		return std::make_shared<OFF>();
	}

	// コライダーコンポーネント取得
	auto coll = owner->GetComponent<MeshColliderComponent>();
	if (coll)
	{
		// 誰もぶつかってない場合処理を実行する
		if (coll->GetResults().size() <= 0)
		{
			// 押されていたら
			if (m_pComp->m_isOn)
			{
				auto modelComp = m_pComp->GetOwner()->GetComponent<ModelComponent>();
				if (modelComp == nullptr)return nullptr;
				// 現在のアニメーションが終了していたら
				if (modelComp->GetAnimator().IsAnimationEnd())
				{
					// アニメーションをPUSHに変更
					modelComp->GetAnimator().ChangeAnime("Off", false);

					// 状態も変更
					m_pComp->m_isOn = false;
				}
			}
		}

	}

	return nullptr;
}
