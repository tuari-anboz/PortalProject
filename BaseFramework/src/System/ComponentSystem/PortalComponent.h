#pragma once

// 継承元クラスのヘッダーをインクルード
#include "BaseComponent.h"

// 必要な事
// ・メッシュの表面にポータルを生成
// ・ポータルに当たり判定を設置
// ・当たり判定がメッシュの法線と平行になるように回転させる

//=============================
// ポータルコンポーネント
//=============================
class PortalComponent : public BaseComponent
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

	// ポータルへの情報セット
	virtual void Set(const KdVec3& _raypos,const KdVec3& _pos, const KdVec3& _normal,const int _no);

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override;


private:
	// ポータルのサイズ
	KdVec3 m_size;

	// ポータルカメラのデフォルト行列
	KdMatrix m_Portal1DefaultCamMat;
	KdMatrix m_Portal2DefaultCamMat;

	//test
	KdMatrix cm;
	KdVec3 cv;

};