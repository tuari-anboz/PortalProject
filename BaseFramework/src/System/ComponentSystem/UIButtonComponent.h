#pragma once

#include "BaseComponent.h"

//===========================================
// UIButtonコンポーネント
// ・画像のクリックを判定
// ・クリックした直後、クリック中、クリックしなくなった直後を判定できるようにする
// ・

namespace UIButton
{
	enum
	{
		OFF = 0x000000001,
		ONBUTTON = 0x00000002,
		PRESS = 0x00000004,
	};
}

class UIButtonComponent : public BaseComponent
{
public:
	virtual void Start() override;

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

	// ゲッター
	int GetButtons() {return m_buttons;}

private:
	// ボタン選択時の画像ファイル名
	std::string m_selectedTexFilename = "";
	// ボタン選択時の画像
	SPtr<KdTexture> m_buttonSelectedTex;

	// ボタン選択時の画像ファイル名
	std::string m_TexFilename = "";
	// ボタンを押してない場合のテクスチャ
	SPtr<KdTexture> m_buttonTex;

	// ボタンの状態
	int m_buttons = 0;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseUIButtonAction : public std::enable_shared_from_this<BaseUIButtonAction> {
	public:
		virtual ~BaseUIButtonAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseUIButtonAction> Update() = 0;
		// キャラへのアドレス
		UIButtonComponent*	m_pComp = nullptr;
	};
	// 「オフ」行動
	class OFF : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// 「オン」行動
	class ONBUTTON : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// 「プレス」行動
	class Press : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};
	// 「ドロップ」行動
	class Drop : public BaseUIButtonAction
	{
	public:
		virtual SPtr<BaseUIButtonAction> Update() override;
	};


	// 現在の行動
	SPtr<BaseUIButtonAction>	m_as = nullptr;

};