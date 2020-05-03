#pragma once

#include "BaseComponent.h"

//==============================================
//
// Lightコンポーネントクラス
//
//==============================================
class LightComponent : public BaseComponent {
public:

	//----------------------------
	// 設定・取得
	//----------------------------

	// ディレクショナルライトとして設定
	void SetDirectionalLight(const KdVec4& color, float intensity) {
		m_lightType = LightTypes::Directional;
		m_color = color;
		m_range = 0;
		m_intensity = intensity;
	}

	// ポイントライトとして設定
	void SetPointLight(const KdVec4& color, float radius, float intensity) {
		m_lightType = LightTypes::Point;
		m_color = color;
		m_range = radius;
		m_intensity = intensity;
	}

	// 描画前処理
	virtual void PreDraw() override;

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() override;

	// (Editor)Editor描画
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:

	// ライトの種類定数
	enum class LightTypes {
		Directional	= 0,
		Point		= 1,
		Spot		= 2,
	};
	// ライトの種類　0:平行ライト 1:ポイントライト 2:スポットライト
	LightTypes	m_lightType = LightTypes::Point;

	//
	KdVec4	m_color = {1, 1, 1, 0};		// 色
	float	m_range = 10;				// 点光の範囲
	float	m_intensity = 1.0f;			// 強度

	float	m_minAngle = 10;			// スポットライトの最小半径
	float	m_maxAngle = 15;			// スポットライトの最大半径

};

