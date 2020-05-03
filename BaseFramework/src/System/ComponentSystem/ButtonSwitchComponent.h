#pragma once

#include "BaseComponent.h"

// ButtonSwitchComponent
// �Ή�����I�u�W�F�N�g(�h�A,�{�b�N�X���o���I�u�W�F�N�g��)��
// �����N���ē��삳���鎖���o����悤�ɂ���H
// �����N�����I�u�W�F�N�g�̎q�Ƃ��ē�����������������

class ButtonSwitchComponent : public BaseComponent
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
};