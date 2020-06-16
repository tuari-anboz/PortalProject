#pragma once

#include "System/ComponentSystem/BaseComponent.h"

//=============================
// �|�[�^���R���|�[�l���g
//=============================
class PortalComponent : public BaseComponent
{
	//=============================
	// ����
	//=============================

	// �X�V����
	virtual void Update() override;

	// (Editor)GUI����
	virtual void ImGuiUpdate() override;
	// (Editor�p)�`��
	virtual void EditorDraw() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;
};