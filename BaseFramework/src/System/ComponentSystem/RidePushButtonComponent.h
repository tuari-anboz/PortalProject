#pragma once

#include "BaseComponent.h"

// RidePushButtonComponent
// 乗ると反応するボタン
// 対応するオブジェクト(ドア,ボックスを出すオブジェクト等)と
// リンクして動作させる事を出来るようにする？
// リンクしたオブジェクトの子として動いた方がいいかも
// アニメーションが初期状態に戻るまで反応状態にする
// 

class RidePushButtonComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	// 
	void OnPush();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:

	//===========================
	// アクションステートクラス
	//===========================
	class BaseRidePushButtonAction : public std::enable_shared_from_this<BaseRidePushButtonAction> {
	public:
		virtual ~BaseRidePushButtonAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseRidePushButtonAction> Update() = 0;
		// キャラへのアドレス
		RidePushButtonComponent*	m_pComp = nullptr;
	};
	// 「基本」行動
	class OFF : public BaseRidePushButtonAction {
	public:
		virtual SPtr<BaseRidePushButtonAction> Update() override;
	};


	// 現在の行動
	SPtr<BaseRidePushButtonAction>	m_as = nullptr;
};