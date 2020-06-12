#pragma once

#include "BaseComponent.h"

// 
// �Q�[���Ŏg�p����͈ړ���C���邩��
// �Q�[�����̏d�͂��ꊇ�ŕύX�ł���Ƃ�������
// 
class ForceComponent : public BaseComponent
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
	// �ړ������
	KdVec3 m_vForce;

	// �؋󎞊ԃt���[����
	int m_SkyFrame;

};