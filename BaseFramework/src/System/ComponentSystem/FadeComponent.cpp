#include "main.h"

#include "System\GameManager.h"

#include "FadeComponent.h"

void FadeComponent::Start()
{
	// スプライトコンポーネント取得
	auto sprite = GetOwner()->GetComponent<SpriteComponent>();
	if (sprite == nullptr)
	{
		return;
	}

	if (GameMgr.m_fade)
	{
		m_fadeAlpha = 1.0f;
		sprite->SetTexAlpha(m_fadeAlpha);
		m_as = std::make_shared<FadeInAction>();
	}
	else
	{
		m_fadeAlpha = 0.0f;
		sprite->SetTexAlpha(m_fadeAlpha);
		m_as = std::make_shared<Basic>();
	}
}

void FadeComponent::Update()
{
	// 行動処理
	if (m_as)
	{
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pComp = this;
		// 行動処理実行
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}
}

void FadeComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	if (ImGui::Button(u8"FadeIn"))
	{
		FadeIn();
	}
	if (ImGui::Button(u8"FadeOut"))
	{
		FadeOut();
	}

}

void FadeComponent::FadeOut()
{
	//
	m_as = std::make_shared<FadeOutAction>();
}

void FadeComponent::FadeIn()
{
	//
	m_as = std::make_shared<FadeInAction>();
}

void FadeComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void FadeComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);
}

SPtr<FadeComponent::BaseFadeAction> FadeComponent::Basic::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;
	
	return nullptr;
}

SPtr<FadeComponent::BaseFadeAction> FadeComponent::FadeInAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// スプライトコンポーネント取得
	auto sprite = owner->GetComponent<SpriteComponent>();
	if (sprite == nullptr)return nullptr;

	m_pComp->m_fadeAlpha -= 0.01f;

	if (m_pComp->m_fadeAlpha <= 0)
	{
		m_pComp->m_fadeAlpha = 0;
		// 透明度変更
		sprite->SetTexAlpha(m_pComp->m_fadeAlpha);
		GameMgr.m_fade = false;

		return std::make_shared<Basic>();
	}

	// 透明度変更
	sprite->SetTexAlpha(m_pComp->m_fadeAlpha);

	return nullptr;
}

SPtr<FadeComponent::BaseFadeAction> FadeComponent::FadeOutAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// スプライトコンポーネント取得
	auto sprite = owner->GetComponent<SpriteComponent>();
	if (sprite == nullptr)return nullptr;

	m_pComp->m_fadeAlpha += 0.01f;

	if (m_pComp->m_fadeAlpha >= 1.0f)
	{
		m_pComp->m_fadeAlpha = 1.0f;
		// 透明度変更
		sprite->SetTexAlpha(m_pComp->m_fadeAlpha);
		GameMgr.m_fade = true;

		return std::make_shared<Basic>();
	}

	// 透明度変更
	sprite->SetTexAlpha(m_pComp->m_fadeAlpha);

	return nullptr;
}
