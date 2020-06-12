#pragma once

#include "BaseComponent.h"

// 
// ゲームで使用する力移動を任せるかも
// ゲーム中の重力を一括で変更できるといいかも
// 
class ForceComponent : public BaseComponent
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
	// 移動する力
	KdVec3 m_vForce;

	// 滞空時間フレーム数
	int m_SkyFrame;

};