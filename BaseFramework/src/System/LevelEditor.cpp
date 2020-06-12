#include "main.h"
#include "GameManager.h"
#include "LevelEditor.h"

void LevelEditor::Init()
{
	// 初期行動
	m_as = std::make_shared<Basic>();
}

void LevelEditor::Update()
{
	// 行動処理
	if (m_as) 
	{
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pObject = this;
		// 行動処理実行
		auto p = m_as->Update();
		if (p)
		{
			m_as = p;
		}
	}
}

void LevelEditor::ImGuiUpdate()
{
	// モデルファイル
	ImGuiResourceButton(u8"生成するファイル名", m_generationPrefabName, { ".pref" });
}

void LevelEditor::Deserialize(const json11::Json& jsonObj)
{

}

void LevelEditor::Serialize(json11::Json::object& outJsonObj)
{

}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Basic::Update()
{
	// 選択中のオブジェクトがある場合選択行動に
	if (GameMgr.GetSelectObj().lock())return std::make_shared<Selected>();



	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Selected::Update()
{
	// 選択中のオブジェクトがない場合基本行動に
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// Cキーで作成モードへ
	if (KdIsKeyDown('C',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"作成モードへ移行");
		return std::make_shared<Create>();
	}

	// Tキーで置換モードへ
	if (KdIsKeyDown('T',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"置換モードへ移行");
		return std::make_shared<Replace>();
	}

	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Create::Update()
{
	// 選択中のオブジェクトセットがない場合基本行動に
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// 再度Cキーで選択モードへ
	if (KdIsKeyDown('C', false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"選択モードへ移行");
		return std::make_shared<Selected>();
	}

	//// 選択しているプレハブファイルがない場合
	//if (m_pObject->m_isCopy == false)
	//{
	//	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();
	//}

	// DELETEキーでオブジェクトを削除
	//if ()
	//{
	//	m_pObject->m_selectedObjectSet.lock()->Delete();
	//	m_pObject->m_selectedObjectSet.reset();
	//	GameMgr.m_Editor_Log.AddLog(u8"選択したオブジェクトセットを削除しました");
	//}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown(VK_RIGHT,false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		//else
		//{
		//	newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		//	auto parent = GameMgr.GetSelectObj().lock()->GetParent();
		//	newObj->SetParent(parent);

		//	auto obj = GameMgr.GetSelectObj().lock();
		//	KdMatrix mSelectedObj = obj->GetMatrix();
		//	KdMatrix mGeneratedObj = mSelectedObj;

		//	mGeneratedObj.Move(2.0f, 0, 0);

		//	newObj->SetMatrix(mGeneratedObj, true);
		//}
		GameMgr.SetSelectObj(newObj);
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown(VK_LEFT, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown('Q', false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown('Z', false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown(VK_UP, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (KdIsKeyDown(VK_DOWN, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Replace::Update()
{
	// 選択中のオブジェクトセットがない場合基本行動に
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// 選択しているプレハブファイルがない場合
	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();

	// Tキーで選択モードへ
	if (KdIsKeyDown('T', false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"選択モードへ移行");
		return std::make_shared<Selected>();
	}


	// Oキーのみなら選択中のオブジェクトのみ
	// SHIFTと一緒に押すと選択中のオブジェクトと同じ名前のオブジェクトも全て置換する
	if (KdIsKeyDown('O') && KdIsKeyDown(VK_LSHIFT,true))
	{
		auto objects = GameMgr.m_level->GetRoot()->FindObjects(GameMgr.GetSelectObj().lock()->GetName());
		for (auto && obj : objects)
		{
			GameMgr.m_Editor_Log.AddLog(u8"オブジェクトを置換しました");
			auto parent = obj->GetParent();
			KdMatrix mObj = obj->GetMatrix();
			auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			ReplaceObj->SetMatrix(mObj, true);
			obj->Delete();
			ReplaceObj->SetParent(parent);
		}
	}
	else if (KdIsKeyDown('P',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"オブジェクトを置換しました");
		auto selectedObj = GameMgr.GetSelectObj().lock();
		auto parent = selectedObj->GetParent();
		KdMatrix mObj = selectedObj->GetMatrix();
		auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		ReplaceObj->SetMatrix(mObj, true);
		selectedObj->Delete();
		ReplaceObj->SetParent(parent);
	}

	return nullptr;
}


