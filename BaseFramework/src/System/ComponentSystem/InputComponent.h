#pragma once

// 継承元クラスのヘッダーをインクルード
#include "BaseComponent.h"

//=============================
// ゲームキー定数
//=============================
namespace GameButtons {
	enum {
		Z = 0x00000001,
		X = 0x00000002,
		C = 0x00000004,
		V = 0x00000008,
		A = 0x00000010,
		S = 0x00000020,
		D = 0x00000040,
		F = 0x00000080,
		RETURN = 0x00000100,
		MR = 0x00000200,
		ML = 0x00000400,
		MM = 0x00000800,
		W = 0x00001000,
		SPACE = 0x00002000,
		ESCAPE = 0x00004000
	};
}

//=============================
// キー入力状態定数
//=============================
namespace KeyState
{
	enum
	{
		FREE = 0x00000001,
		HOLD = 0x00000002,
		PRESS = 0x00000004,
		RELEASE = 0x00000008
	};
}

//=============================
// 入力コンポーネント基本クラス
//=============================
class InputComponent : public BaseComponent {
public:
	// キー入力
	std::array<int, 256> m_keys;
	std::array<int, 256> m_keysRepeat;
	// ボタン
	int		m_buttons = 0;
	int		m_buttonsNotRepeat = 0;
	// 軸
	KdVec2	m_axis;
	// マウスポインタの前回からの移動量
	float m_deltaX = 0;
	float m_deltaY = 0;
};

//=============================
// プレイヤー入力コンポーネント
//=============================
// プレイヤー入力クラス
class PlayerInputComponent : public InputComponent {
public:
	// 初回起動時処理
	virtual void Start() override;

	virtual void Update() override;

	// (Editor)GUI処理
	virtual void ImGuiUpdate() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override;
private:
	POINT m_PrevMousePos{ 0,0 };

	// マウスカーソルを中心に固定するかどうか
	bool m_FixedMouseCursor = false;
};

//==============================
// 汎用入力コンポーネント
//==============================
// 汎用入力クラス
class BasicInputComponent : public InputComponent
{
public:
	// 初回起動時
	virtual void Start() override;

	virtual void Update() override;

	// (Editor)GUI処理
	virtual void ImGuiUpdate() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override;
private:
	POINT m_PrevMousePos{ 0,0 };

	// マウスカーソルを中心に固定するかどうか
	bool m_fixedMouseCursor = false;
};
