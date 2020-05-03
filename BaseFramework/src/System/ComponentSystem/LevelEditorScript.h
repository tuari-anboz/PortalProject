#pragma once

#include "BaseComponent.h"

class LevelEditorScript : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
	
private:
	// 生成するプレハブ名
	std::string m_generationPrefabName = "";
	// 生成するプレイヤー名
	std::string m_playerPrefabName = "";
	// 選択したオブジェクトをコピーするか
	bool m_isCopy = true;
	// 選択したオブジェクトセット
	WPtr<Object> m_selectedObjectSet;
	// 選択したオブジェクト
	WPtr<Object> m_selectedObject;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseLevelEditorAction : public std::enable_shared_from_this<BaseLevelEditorAction> {
	public:
		virtual ~BaseLevelEditorAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseLevelEditorAction> Update() = 0;
		// キャラへのアドレス
		LevelEditorScript*	m_pObject = nullptr;
	};
	// 「基本」行動
	class Basic : public BaseLevelEditorAction {
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「選択中」行動
	class Selected : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「作成」行動
	class Create : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「編集」行動
	class Edit : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「置換」行動
	class Replace : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 現在の行動
	SPtr<BaseLevelEditorAction>	m_as = nullptr;
};