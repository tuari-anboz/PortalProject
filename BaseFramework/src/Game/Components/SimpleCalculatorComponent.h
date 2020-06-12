#pragma once

#include "System/ComponentSystem/BaseComponent.h"

//
//
//
//
class SimpleCalculatorComponent : public BaseComponent
{
public:
	// �����ݒ�
	virtual void Start() override;

	// ����
	virtual void Update() override;

	// ImGui����
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;

private:
	// �I�u�W�F�N�g����������Ă���̃t���[����
	int m_LifeTime = 0;
	// �I�u�W�F�N�g�̎����t���[����
	int	m_LifeSpan = 0;
	// ���t���[���̕ω���
	KdVec3 m_vScale = { 1.0f,1.0f,1.0f };
	KdVec3 m_vRot, m_vTrans;

};