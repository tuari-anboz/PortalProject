#include "main.h"

#include "../GameManager.h"

#include "RidePushButtonComponent.h"

void RidePushButtonComponent::Start()
{
	m_as = std::make_shared<OFF>();

	
}

void RidePushButtonComponent::Update()
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
	if (m_as) {
		while (1)
		{
			// 持ち主のキャラのアドレスを入れておく
			m_as->m_pComp = this;
			// 行動処理実行
			auto p = m_as->Update();
			if (p) {
				m_as = p;
			}
			else
			{
				break;
			}
		}
	}
}

void RidePushButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void RidePushButtonComponent::OnPush()
{
	// アニメーションをPUSHに変更


	// 状態も押されている状態に変更

}

void RidePushButtonComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void RidePushButtonComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<RidePushButtonComponent::BaseRidePushButtonAction> RidePushButtonComponent::OFF::Update()
{

	return nullptr;
}
