#pragma once

#include "System/ComponentSystem/BaseComponent.h"

//=============================
// ポータルコンポーネント
//=============================
class PortalComponent : public BaseComponent
{
	//=============================
	// 処理
	//=============================

	// 更新処理
	virtual void Update() override;

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
};