#pragma once

// 継承元クラスのヘッダーをインクルード
#include "BaseComponent.h"

//=============================
// 射撃コンポーネント
//=============================
// ***************************
// 必要な事
// ・対応するタグをImGui上で決められること
// ・射程距離等をImGui上で決められること
// ・当たったときに実行する関数はメンバに持っておく？
// ・撃つ方向をImGuiとソース内で決められること

class ShotComponent : public BaseComponent
{
public:

	//=============================
	// 処理
	//=============================

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw() override;

	// (Editor)GUI処理
	virtual void ImGuiUpdate() override;

	// (Editor用)描画
	virtual void EditorDraw() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override;

	//=============================
	// アクセサ
	//=============================
	void SetDir(const KdVec3& dir) { m_dir = dir; }
	void AddFunction();

private:
	// 射程
	float m_range;

	// 方向
	KdVec3 m_dir;
	// 当たった位置
	KdVec3 m_hitPos;
	// 発射した位置
	KdVec3 m_shotPos;
	// タグ
	std::string m_tag;
	// 実行関数
	std::function <void(void)> m_function;
};