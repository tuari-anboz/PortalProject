#include "main.h"
#include "System/GameManager.h"
#include "SimpleCalculatorComponent.h"

void SimpleCalculatorComponent::Start()
{
}

void SimpleCalculatorComponent::Update()
{
	auto owner = GetOwner();
	if (owner == nullptr)return;

	m_LifeTime++;

	// 寿命フレーム数を超えたらオブジェクトを削除
	if (m_LifeTime > m_LifeSpan)
	{
	//	owner->Delete();
	}

	// 拡縮・回転・移動を行う
	KdMatrix m = owner->GetMatrix();

	KdVec3 scale = m.GetScale();
	scale += m_vScale;
	m.SetScale(scale);

	m.RotateX_Local(m_vRot.x);
	m.RotateY_Local(m_vRot.y);
	m.RotateZ_Local(m_vRot.z);
	m.Move_Local(m_vTrans);

	owner->SetMatrix(m, false);
}

void SimpleCalculatorComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	ImGui::DragInt(u8"生存フレーム数", &m_LifeSpan, 1);
	ImGui::DragFloat3(u8"移動", m_vTrans, 1.0f);
	ImGui::DragFloat3(u8"回転", m_vRot, 1.0f);
	ImGui::DragFloat3(u8"拡大縮小", m_vScale, 0.001f);
}

void SimpleCalculatorComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_LifeSpan = jsonObj["LifeSpan"].int_value();
	m_vTrans.Deserialize(jsonObj["Transform"]);
	m_vRot.Deserialize(jsonObj["Rotation"]);
	m_vScale.Deserialize(jsonObj["Scale"]);

}

void SimpleCalculatorComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["LifeSpan"] = m_LifeSpan;
	outJsonObj["Transform"] = m_vTrans.Serialize();
	outJsonObj["Rotation"] = m_vRot.Serialize();
	outJsonObj["Scale"] = m_vScale.Serialize();
}
