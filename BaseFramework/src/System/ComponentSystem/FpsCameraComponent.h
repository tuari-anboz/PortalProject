#pragma once

#include "BaseComponent.h"

//==============================================
//
// FPS�J�����R���|�[�l���g�N���X
//
//==============================================
class FpsCameraComponent : public BaseComponent
{
public:
	// �X�V����
	virtual void Update()override;

	// (Editor)GUI����
	virtual void ImGuiUpdate() override;
	// (Editor�p)�`��
	virtual void EditorDraw() override;

	float& GetAngY() { return m_angY; }
	float& GetAngX() { return m_angX; }

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;


private:
	// ���[�J���ȍ���
	float m_height = 0.0f;

	// �e�ƃ����N���Đe�̕������J�����ɒǏ]�����邩
	bool m_linkToParent = false;

	// �J�����̉�]�p�x
	float m_angX = 0.0f;
	float m_angY = 0.0f;
};