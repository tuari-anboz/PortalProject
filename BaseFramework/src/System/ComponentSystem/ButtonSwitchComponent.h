#pragma once

#include "BaseComponent.h"

// ButtonSwitchComponent
// 対応するオブジェクト(ドア,ボックスを出すオブジェクト等)と
// リンクして動作させる事を出来るようにする？
// リンクしたオブジェクトの子として動いた方がいいかも

class ButtonSwitchComponent : public BaseComponent
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
};