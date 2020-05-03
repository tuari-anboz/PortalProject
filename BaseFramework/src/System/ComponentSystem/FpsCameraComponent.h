#pragma once

#include "BaseComponent.h"

//==============================================
//
// FPSカメラコンポーネントクラス
//
//==============================================
class FpsCameraComponent : public BaseComponent
{
public:
	// 更新処理
	virtual void Update()override;

	// (Editor)GUI処理
	virtual void ImGuiUpdate() override;
	// (Editor用)描画
	virtual void EditorDraw() override;

	float& GetAngY() { return m_angY; }
	float& GetAngX() { return m_angX; }

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override;


private:
	// ローカルな高さ
	float m_height = 0.0f;

	// 親とリンクして親の方向をカメラに追従させるか
	bool m_linkToParent = false;

	// カメラの回転角度
	float m_angX = 0.0f;
	float m_angY = 0.0f;
};