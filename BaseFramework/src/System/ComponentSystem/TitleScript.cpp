#include "main.h"

#include "../GameManager.h"

#include "TitleScript.h"

void TitleScript::Start()
{
	m_as = std::make_shared<Basic>();
}

void TitleScript::Update()
{
	// オーナー取得
	auto owner = GetOwner();

	// 入力コンポーネント取得
	auto input = GameMgr.m_level->GetRoot()->GetComponent<BasicInputComponent>();
	if (input) {
		// Aボタン(Zキー)
		if (input->m_keys['Z'])
		{
			auto loadlevel = owner->GetComponent<LoadLevelComponent>();
			// レベル変更
			loadlevel->LoadLevel("data/StageDataLevelEditor.json");
			GameMgr.m_Editor_Log.AddLog("OEK");
		}

	}

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

SPtr<TitleScript::BaseTitleAction> TitleScript::Basic::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// 子のボタンオブジェクトを検索して
	// ボタンオブジェクト毎の機能を持たせる

	return nullptr;
}

SPtr<TitleScript::BaseTitleAction> TitleScript::StageSelect::Update()
{

	return nullptr;
}