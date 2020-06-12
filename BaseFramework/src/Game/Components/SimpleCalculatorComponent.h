#pragma once

#include "System/ComponentSystem/BaseComponent.h"

//
//
//
//
class SimpleCalculatorComponent : public BaseComponent
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
	// オブジェクトが生成されてからのフレーム数
	int m_LifeTime = 0;
	// オブジェクトの寿命フレーム数
	int	m_LifeSpan = 0;
	// 毎フレームの変化量
	KdVec3 m_vScale = { 1.0f,1.0f,1.0f };
	KdVec3 m_vRot, m_vTrans;

};