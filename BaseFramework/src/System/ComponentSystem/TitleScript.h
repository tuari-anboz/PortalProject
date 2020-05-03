#pragma once

#include "BaseComponent.h"

// タイトルオブジェクト用スクリプト
// タイトル画面での操作をステートパターンを用いて表現する
class TitleScript : public BaseComponent {
public:
	virtual void Start() override;

	virtual void Update() override;

private:
	//===========================
	// アクションステートクラス
	//===========================
	class BaseTitleAction : public std::enable_shared_from_this<BaseTitleAction> {
	public:
		virtual ~BaseTitleAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseTitleAction> Update() = 0;
		// キャラへのアドレス
		TitleScript*	m_pComp = nullptr;
	};
	// 「基本」行動
	class Basic : public BaseTitleAction {
	public:
		virtual SPtr<BaseTitleAction> Update() override;
	};
	// (仮) 「ステージセレクト」行動
	class StageSelect : public BaseTitleAction{
	public:
		virtual SPtr<BaseTitleAction> Update() override;
	};

	// 現在の行動
	SPtr<BaseTitleAction>	m_as = nullptr;

};
