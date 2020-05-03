#pragma once

#include "BaseComponent.h"

// LoadLevelComponent
// レベルのロードを制御するコンポーネント
// レベル切り替え時の動作を担当します
// FadeComponentを使用してレベル切り替え時に適切な動作をさせる

// 使い方
// このコンポーネントに切り替えたいレベルのファイル名を渡す
// フェードインフェードアウトやロード画面を出すか選択する
class LoadLevelComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	// レベルのロード
	void LoadLevel(const std::string& levelfilename);

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// 切り替え先のレベルファイル名
	std::string m_levelFilename = "";

	//===========================
	// アクションステートクラス
	//===========================
	class BaseLoadLevelAction : public std::enable_shared_from_this<BaseLoadLevelAction> {
	public:
		virtual ~BaseLoadLevelAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseLoadLevelAction> Update() = 0;
		// キャラへのアドレス
		LoadLevelComponent*	m_pComp = nullptr;
	};
	// 「基本」行動
	class Generic: public BaseLoadLevelAction {
	public:
		virtual SPtr<BaseLoadLevelAction> Update() override;
	};
	//「レベルロード」行動
	class LoadLevelAction : public BaseLoadLevelAction {
	public:
		virtual SPtr<BaseLoadLevelAction> Update() override;
	};

	// 現在の行動
	SPtr<BaseLoadLevelAction>	m_as = nullptr;
};