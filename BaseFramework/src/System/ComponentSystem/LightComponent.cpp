#include "main.h"

#include "../GameManager.h"

#include "LightComponent.h"

//==============================================
//
// Lightコンポーネント
//
//==============================================

void LightComponent::Deserialize(const json11::Json & jsonObj)
{
	// 継承元のデータもデシリアライズ
	BaseComponent::Deserialize(jsonObj);

	// 
	m_lightType = (LightTypes)jsonObj["LightType"].int_value();
	m_color.Deserialize(jsonObj["Color"]);
	m_range = (float)jsonObj["Range"].number_value();
	m_intensity = (float)jsonObj["Intensity"].number_value();

	m_minAngle = (float)jsonObj["MinAngle"].number_value();
	m_maxAngle = (float)jsonObj["MaxAngle"].number_value();
}

void LightComponent::Serialize(json11::Json::object & outJsonObj)
{
	// 継承元のデータもシリアライズ
	BaseComponent::Serialize(outJsonObj);

	// 
	outJsonObj["LightType"] = (int)m_lightType;
	outJsonObj["Color"] = m_color.Serialize();
	outJsonObj["Range"] = m_range;
	outJsonObj["Intensity"] = m_intensity;

	outJsonObj["MinAngle"] = m_minAngle;
	outJsonObj["MaxAngle"] = m_maxAngle;
}

void LightComponent::PreDraw()
{
	if (m_enable == false)return;

	// 平行ライト
	if (m_lightType == LightTypes::Directional) {
		//
		ShMgr.m_KdModelSh.SetDirectionalLight(GetOwner()->GetMatrix().GetZAxis(), m_color * m_intensity);
		
		// ライト情報を登録
		int idx = ShMgr.m_cb8_Light.GetWork_Read().DL_Cnt;
		if (idx < 3)
		{
			auto& work = ShMgr.m_cb8_Light.GetWork();
			work.DL[idx].Dir = GetOwner()->GetMatrix().GetZAxis();
			work.DL[idx].Color = m_color.ToVec3() * m_intensity;
			// 数を増やす
			work.DL_Cnt++;
		}
	}
	// ポイントライト
	else if (m_lightType == LightTypes::Point) {
		// 
		ShMgr.m_KdModelSh.AddPointLight(GetOwner()->GetMatrix().GetPos(), m_range, m_color * m_intensity);
		// ライト情報を登録
		int idx = ShMgr.m_cb8_Light.GetWork_Read().PL_Cnt;
		if (idx < 100)
		{
			auto& work = ShMgr.m_cb8_Light.GetWork();
			work.PL[idx].Pos = GetOwner()->GetMatrix().GetPos();
			work.PL[idx].Color = m_color.ToVec3() * m_intensity;
			work.PL[idx].Radius = m_range;
			// 数を増やす
			work.PL_Cnt++;
		}
	}
	// スポットライト
	else if (m_lightType == LightTypes::Spot) {
		// 
		ShMgr.m_KdModelSh.AddSpotLight(GetOwner()->GetMatrix().GetPos(), GetOwner()->GetMatrix().GetZAxis(), m_range, m_minAngle, m_maxAngle, m_color * m_intensity);
	}

}

void LightComponent::ImGuiUpdate()
{
	//
	BaseComponent::ImGuiUpdate();

	static const char* lightTypes[] = {"Directional", "Point", "Spot"};

	ImGui::Combo("Type", (int*)&m_lightType, lightTypes, _countof(lightTypes));

	// 
	ImGui::ColorEdit3("Color", m_color, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

	//
	ImGui::DragFloat("Intensity", &m_intensity, 0.01f, 0, 100);

	// ポイントライト
	if (m_lightType == LightTypes::Point) {
		ImGui::DragFloat("Range", &m_range, 0.1f);
	}
	// スポットライト
	else if (m_lightType == LightTypes::Spot) {
		ImGui::DragFloat("Range", &m_range, 0.1f);

		ImGui::DragFloat(u8"最小角度", &m_minAngle, 0.1f, 0, 180);
		ImGui::DragFloat(u8"最大角度", &m_maxAngle, 0.1f, 0, 180);
	}
}

void LightComponent::EditorDraw()
{
	if (m_enable == false)return;

	// 平行ライトとスポットライト
	if (m_lightType == LightTypes::Directional || m_lightType == LightTypes::Spot) {
		KdVec3 vZ = GetOwner()->GetMatrix().GetZAxis();
		vZ.Normalize();

		ShMgr.m_KdPrimSh.DrawLine(GetOwner()->GetMatrix().GetPos(), GetOwner()->GetMatrix().GetPos() + vZ * 2, { 1, 1, 0, 1 });
	}
	// ポイントライト
	if (m_lightType == LightTypes::Point) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(GetOwner()->GetMatrix().GetPos(), m_range, { 1, 1, 0, 1 });
	}
}
