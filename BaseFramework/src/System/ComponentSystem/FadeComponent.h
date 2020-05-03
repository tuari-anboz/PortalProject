#pragma once

#include "BaseComponent.h"

// フェードイン/フェードアウトさせるコンポーネント
// GameManagerのフラグで現在のフェード状況を判断して
// ステージ開始時などには適切にフェードイン処理をする
// 別のコンポーネントで呼び出して任意のタイミングでフェードアウトさせる

// 使い方
// ボタン等を押した後stateを変更してフェードアウトさせた後シーンを切り替える

// 子のスプライトコンポーネントオブジェクト

class FadeComponent : public BaseComponent
{
public:
	virtual void Start() override;

	virtual void Update() override;

	virtual void ImGuiUpdate() override;

	// フェードイン
	void FadeIn();
	// フェードアウト
	void FadeOut();

	virtual void Deserialize(const json11::Json& jsonObj)override;

	virtual void Serialize(json11::Json::object& outJson)override;
private:
	// フェードインアウト時の透明度
	float m_fadeAlpha = 1.0f;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseFadeAction : public std::enable_shared_from_this<BaseFadeAction> {
	public:
		virtual ~BaseFadeAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseFadeAction> Update() = 0;
		// キャラへのアドレス
		FadeComponent*	m_pComp = nullptr;
	};
	// 「基本」行動
	class Basic : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};
	//「フェードイン」行動
	class FadeInAction : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};
	//「フェードアウト」行動
	class FadeOutAction : public BaseFadeAction {
	public:
		virtual SPtr<BaseFadeAction> Update() override;
	};

	// 現在の行動
	SPtr<BaseFadeAction>	m_as = nullptr;

};