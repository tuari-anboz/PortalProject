#pragma once

#include "BaseComponent.h"


//==============================================
//
// カメラコンポーネントクラス
//
//==============================================
class CameraComponent : public BaseComponent {
public:

	//----------------------------
	// 設定・取得
	//----------------------------
	// 射影行列設定
	void SetProjection(float fov, float fNear, float fFar) {
		m_fov = fov;
		m_nearFarDist.Set(fNear, fFar);
	}

	// Aspect比の設定
	void SetAspect(float _aspectratio) { m_aspect = _aspectratio; }


	// 描画前処理
	virtual void PreDraw() override;

	// (Editor)Editor描画
	virtual void EditorDraw() override;

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// アクティブ状態、シェーダーへカメラをセットする
	bool		m_active = true;
	// 射影行列設定
	float		m_fov = 60;						// 視野角
	float		m_aspect = KD3D.GetBackBuffer()->GetAspect();		// アスペクト比
	KdVec2		m_nearFarDist = {0.1f, 1000};	// 最近接距離、最遠方距離

	// 親Objectに取り付ける
	bool		m_attachToParent = false;		// 親に追従する？
	float		m_attachDistance = 5;			// 距離
	KdVec3		m_attachOffset;					// オフセット座標
};
