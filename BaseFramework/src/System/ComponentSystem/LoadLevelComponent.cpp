#include "main.h"

#include "../GameManager.h"

#include "LoadLevelComponent.h"

void LoadLevelComponent::Start()
{
	m_as = std::make_shared<Generic>();
}

void LoadLevelComponent::Update()
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

void LoadLevelComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// レベルファイル
	ImGuiResourceButton("Level File", m_levelFilename, { ".json" });
}

void LoadLevelComponent::LoadLevel(const std::string & levelfilename)
{
	m_levelFilename = levelfilename;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	// フェードオブジェクトを探す
	auto fadeobj = GameMgr.m_level->GetRoot()->FindObject("Fade");
	if (fadeobj == nullptr)return;
	// フェードコンポーネントを使用してフェードインフェードアウトさせる
	auto fade = fadeobj->GetComponent<FadeComponent>();
	if (fade == nullptr)return;
	fade->FadeOut();

	m_as = std::make_shared<LoadLevelAction>();
}

void LoadLevelComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void LoadLevelComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<LoadLevelComponent::BaseLoadLevelAction> LoadLevelComponent::Generic::Update()
{
	return nullptr;
}

SPtr<LoadLevelComponent::BaseLoadLevelAction> LoadLevelComponent::LoadLevelAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// フェードアウトが終了したら指定したレベルをロードする
	if (GameMgr.m_fade)
	{
		GameMgr.ChangeLevel(m_pComp->m_levelFilename);
	}

	return nullptr;
}
