#pragma once

// �p�����N���X�̃w�b�_�[���C���N���[�h
#include "BaseComponent.h"

//=============================
// �Q�[���L�[�萔
//=============================
namespace GameButtons {
	enum {
		Z = 0x00000001,
		X = 0x00000002,
		C = 0x00000004,
		V = 0x00000008,
		A = 0x00000010,
		S = 0x00000020,
		D = 0x00000040,
		F = 0x00000080,
		RETURN = 0x00000100,
		MR = 0x00000200,
		ML = 0x00000400,
		MM = 0x00000800,
		W = 0x00001000,
		SPACE = 0x00002000,
		ESCAPE = 0x00004000
	};
}

//=============================
// �L�[���͏�Ԓ萔
//=============================
namespace KeyState
{
	enum
	{
		FREE = 0x00000001,
		HOLD = 0x00000002,
		PRESS = 0x00000004,
		RELEASE = 0x00000008
	};
}

//=============================
// ���̓R���|�[�l���g��{�N���X
//=============================
class InputComponent : public BaseComponent {
public:
	// �L�[����
	std::array<int, 256> m_keys;
	std::array<int, 256> m_keysRepeat;
	// �{�^��
	int		m_buttons = 0;
	int		m_buttonsNotRepeat = 0;
	// ��
	KdVec2	m_axis;
	// �}�E�X�|�C���^�̑O�񂩂�̈ړ���
	float m_deltaX = 0;
	float m_deltaY = 0;
};

//=============================
// �v���C���[���̓R���|�[�l���g
//=============================
// �v���C���[���̓N���X
class PlayerInputComponent : public InputComponent {
public:
	// ����N��������
	virtual void Start() override;

	virtual void Update() override;

	// (Editor)GUI����
	virtual void ImGuiUpdate() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;
private:
	POINT m_PrevMousePos{ 0,0 };

	// �}�E�X�J�[�\���𒆐S�ɌŒ肷�邩�ǂ���
	bool m_FixedMouseCursor = false;
};

//==============================
// �ėp���̓R���|�[�l���g
//==============================
// �ėp���̓N���X
class BasicInputComponent : public InputComponent
{
public:
	// ����N����
	virtual void Start() override;

	virtual void Update() override;

	// (Editor)GUI����
	virtual void ImGuiUpdate() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;
private:
	POINT m_PrevMousePos{ 0,0 };

	// �}�E�X�J�[�\���𒆐S�ɌŒ肷�邩�ǂ���
	bool m_fixedMouseCursor = false;
};
