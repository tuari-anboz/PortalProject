#include "main.h"

#include "../GameManager.h"

#include "LightComponent.h"

//==============================================
//
// Light�R���|�[�l���g
//
//==============================================

void LightComponent::Deserialize(const json11::Json & jsonObj)
{
	// �p�����̃f�[�^���f�V���A���C�Y
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
	// �p�����̃f�[�^���V���A���C�Y
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

	// ���s���C�g
	if (m_lightType == LightTypes::Directional) {
		//
		ShMgr.m_KdModelSh.SetDirectionalLight(GetOwner()->GetMatrix().GetZAxis(), m_color * m_intensity);
		
		// ���C�g����o�^
		int idx = ShMgr.m_cb8_Light.GetWork_Read().DL_Cnt;
		if (idx < 3)
		{
			auto& work = ShMgr.m_cb8_Light.GetWork();
			work.DL[idx].Dir = GetOwner()->GetMatrix().GetZAxis();
			work.DL[idx].Color = m_color.ToVec3() * m_intensity;
			// ���𑝂₷
			work.DL_Cnt++;
		}
	}
	// �|�C���g���C�g
	else if (m_lightType == LightTypes::Point) {
		// 
		ShMgr.m_KdModelSh.AddPointLight(GetOwner()->GetMatrix().GetPos(), m_range, m_color * m_intensity);
		// ���C�g����o�^
		int idx = ShMgr.m_cb8_Light.GetWork_Read().PL_Cnt;
		if (idx < 100)
		{
			auto& work = ShMgr.m_cb8_Light.GetWork();
			work.PL[idx].Pos = GetOwner()->GetMatrix().GetPos();
			work.PL[idx].Color = m_color.ToVec3() * m_intensity;
			work.PL[idx].Radius = m_range;
			// ���𑝂₷
			work.PL_Cnt++;
		}
	}
	// �X�|�b�g���C�g
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

	// �|�C���g���C�g
	if (m_lightType == LightTypes::Point) {
		ImGui::DragFloat("Range", &m_range, 0.1f);
	}
	// �X�|�b�g���C�g
	else if (m_lightType == LightTypes::Spot) {
		ImGui::DragFloat("Range", &m_range, 0.1f);

		ImGui::DragFloat(u8"�ŏ��p�x", &m_minAngle, 0.1f, 0, 180);
		ImGui::DragFloat(u8"�ő�p�x", &m_maxAngle, 0.1f, 0, 180);
	}
}

void LightComponent::EditorDraw()
{
	if (m_enable == false)return;

	// ���s���C�g�ƃX�|�b�g���C�g
	if (m_lightType == LightTypes::Directional || m_lightType == LightTypes::Spot) {
		KdVec3 vZ = GetOwner()->GetMatrix().GetZAxis();
		vZ.Normalize();

		ShMgr.m_KdPrimSh.DrawLine(GetOwner()->GetMatrix().GetPos(), GetOwner()->GetMatrix().GetPos() + vZ * 2, { 1, 1, 0, 1 });
	}
	// �|�C���g���C�g
	if (m_lightType == LightTypes::Point) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(GetOwner()->GetMatrix().GetPos(), m_range, { 1, 1, 0, 1 });
	}
}
