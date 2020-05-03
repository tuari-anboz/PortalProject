#include "main.h"

#include "../GameManager.h"

#include "CameraComponent.h"

//==============================================
//
// カメラコンポーネント
//
//==============================================

void CameraComponent::PreDraw()
{
	if (m_enable == false)return;
	// 非アクティブカメラはシェーダへセットしない
	if (m_active == false)return;
	// Editor用
	if (GameMgr.m_Editor_UseCameraComponent == false)return;

	//============================
	// 射影行列
	//============================
	ShMgr.m_mProj.CreatePerspectiveFovLH(m_fov, m_aspect, m_nearFarDist.x, m_nearFarDist.y);
	

	//============================
	// ビュー行列
	//============================
	ShMgr.m_mView = GetOwner()->GetMatrix();
	ShMgr.m_mView.NormalizeScale();
	ShMgr.m_mView.Inverse();

	// カメラ情報(ビュー行列、射影行列)を、各シェーダの定数バッファへセット
	ShMgr.UpdateCamera();
}

void CameraComponent::EditorDraw()
{
	if (m_enable == false)return;


	// 射影行列
	KdMatrix mProj;
	mProj.CreatePerspectiveFovLH(m_fov, m_aspect, m_nearFarDist.x, m_nearFarDist.y);
	// ビュー行列
	KdMatrix mView;
	mView = GetOwner()->GetMatrix();
	mView.NormalizeScale();
	mView.Inverse();

	// ビューと射影の逆行列から、視錐台の各頂点を求める
	KdMatrix mInvVP = mView * mProj;
	mInvVP.Inverse();

	KdVec3 points[8] = {
		{-1, -1, 0},
		{-1,  1, 0},
		{ 1,  1, 0},
		{ 1, -1, 0},

		{-1, -1, 1},
		{-1,  1, 1},
		{ 1,  1, 1},
		{ 1, -1, 1},
	};
	for (auto&& v : points) {
		v.Transform(mInvVP);
	}

	// 
	ShMgr.m_KdPrimSh.DrawLine(points[0], points[4], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[1], points[5], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[2], points[6], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[3], points[7], { 1, 1, 0, 1 });

	// 最近接距離
	ShMgr.m_KdPrimSh.DrawLine(points[0], points[1], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[1], points[2], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[2], points[3], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[3], points[0], { 1, 1, 0, 1 });

	// 最遠方距離
	ShMgr.m_KdPrimSh.DrawLine(points[4], points[5], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[5], points[6], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[6], points[7], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[7], points[4], { 1, 1, 0, 1 });
}

void CameraComponent::Deserialize(const json11::Json & jsonObj)
{
	// 継承元のデータもデシリアライズ
	BaseComponent::Deserialize(jsonObj);

	m_fov = (float)jsonObj["FOV"].number_value();
	m_nearFarDist.Deserialize(jsonObj["NearFarDist"]);
	m_attachToParent = jsonObj["AttachToParent"].bool_value();
	m_attachDistance = (float)jsonObj["AttachDistance"].number_value();
	m_attachOffset.Deserialize(jsonObj["AttachOffset"]);
	m_active = jsonObj["Active"].bool_value();
}

void CameraComponent::Serialize(json11::Json::object & outJsonObj)
{
	// 継承元のデータもシリアライズ
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["FOV"] = m_fov;
	outJsonObj["NearFarDist"] = m_nearFarDist.Serialize();
	outJsonObj["AttachToParent"] = m_attachToParent;
	outJsonObj["AttachDistance"] = m_attachDistance;
	outJsonObj["AttachOffset"] = m_attachOffset.Serialize();
	outJsonObj["Active"] = m_active;
}

void CameraComponent::ImGuiUpdate()
{
	// 継承元のImGui処理
	BaseComponent::ImGuiUpdate();

	// アクティブ状態
	ImGui::Checkbox(u8"描画で使用する", &m_active);

	// 視野角
	ImGui::DragFloat(u8"FOV", &m_fov, 1, 1, 179);

	// 視野の距離
	ImGui::DragFloat2("Near and Far", m_nearFarDist, 0.01f, 0.001f, 10000);
	if (m_nearFarDist.x < 0.0001f) {
		m_nearFarDist.x = 0.0001f;
	}

	// 親に取り付け
	ImGui::Indent();
	if (ImGui::CollapsingHeader(u8"親Object取り付け"))
	{
		ImGui::Checkbox(u8"親に取り付け", &m_attachToParent);
		ImGui::DragFloat(u8"距離", &m_attachDistance, 0.01f, 0, 100);
		ImGui::DragFloat3(u8"Offset", m_attachOffset, 0.01f);
	}
	ImGui::Unindent();
}
