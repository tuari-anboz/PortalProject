#include "main.h"

#include "../GameManager.h"

#include "InputComponent.h"

void PlayerInputComponent::Start()
{
	// �}�E�X
	POINT nowPos;
	GetCursorPos(&nowPos);
	m_PrevMousePos = nowPos;
}

void PlayerInputComponent::Update()
{
	// �{�^�����Z�b�g
	m_axis = { 0, 0 };
	m_buttons = 0;
	m_buttonsNotRepeat = 0;
	m_deltaX = 0;
	m_deltaY = 0;

	// �����̏ꍇ�͉������Ȃ�
	if (m_enable == false) {
		return;
	}

	// ��
	if (KdIsKeyDown(VK_LEFT, true))	m_axis.x = -1.0f;
	if (KdIsKeyDown(VK_RIGHT, true))m_axis.x = 1.0f;
	if (KdIsKeyDown(VK_UP, true))	m_axis.y = 1.0f;
	if (KdIsKeyDown(VK_DOWN, true))	m_axis.y = -1.0f;
	// �{�^��
	if (KdIsKeyDown('Z', true))	m_buttons |= GameButtons::Z;
	if (KdIsKeyDown('X', true))	m_buttons |= GameButtons::X;
	if (KdIsKeyDown('C', true))	m_buttons |= GameButtons::C; 
	if (KdIsKeyDown('V', true))	m_buttons |= GameButtons::V; 
	if (KdIsKeyDown('W', true)) m_buttons |= GameButtons::W;
	if (KdIsKeyDown('A', true))	m_buttons |= GameButtons::A;
	if (KdIsKeyDown('S', true))	m_buttons |= GameButtons::S;
	if (KdIsKeyDown('D', true))	m_buttons |= GameButtons::D;
	if (KdIsKeyDown('F', true))	m_buttons |= GameButtons::F;
	if (KdIsKeyDown(VK_RETURN, true))m_buttons |= GameButtons::RETURN;
	if (KdIsKeyDown(VK_SPACE, true))m_buttons |= GameButtons::SPACE;
	if (KdIsKeyDown(VK_ESCAPE, true))m_buttons |= GameButtons::ESCAPE;
	if (KdIsKeyDown(VK_RBUTTON, true))m_buttons |= GameButtons::MR;
	if (KdIsKeyDown(VK_LBUTTON, true))m_buttons |= GameButtons::ML;
	if (KdIsKeyDown(VK_MBUTTON, true))m_buttons |= GameButtons::MM;
	// �}�E�X
	POINT nowPos;
	GetCursorPos(&nowPos);

	// �}�E�X�̑O�񂩂�̈ړ��ʂ��v�Z
	float deltaX = 0;
	if (nowPos.x - m_PrevMousePos.x > 1 || nowPos.x - m_PrevMousePos.x < -1)
	{
		deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.5f;
	}
	float deltaY = 0;
	if (nowPos.y - m_PrevMousePos.y > 1 || nowPos.y - m_PrevMousePos.y < -1)
	{

		deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.5f;
	}

	m_deltaX = deltaX;
	m_deltaY = deltaY;

	// �}�E�X�J�[�\���𒆐S�ɌŒ肷��
	if (m_FixedMouseCursor)
	{
		RECT windowSize = { 0 };
		HWND hwnd = nullptr;
		hwnd = APP.m_Window.GetHWnd();
		GetClientRect(hwnd, &windowSize);
		POINT FixedPos = { windowSize.left + (windowSize.right / 2), windowSize.top + (windowSize.bottom / 2) };
		// �N���C�A���g�̐^�񒆂̍��W���f�B�X�v���C�̂ǂ̈ʒu�ɂ��邩�ɕϊ�����
		ClientToScreen(hwnd, &FixedPos);
		// 
		SetCursorPos(FixedPos.x,FixedPos.y);

		m_PrevMousePos = FixedPos;
	}
	else
	{
		m_PrevMousePos = nowPos;
	}

	if (m_buttons & GameButtons::ESCAPE)
	{
		m_FixedMouseCursor = false;
	}
}

void PlayerInputComponent::ImGuiUpdate()
{
	// �p������ImGui����
	BaseComponent::ImGuiUpdate();

	ImGui::Checkbox(u8"�}�E�X�J�[�\���𒆐S�ɌŒ肷��", &m_FixedMouseCursor);
}

void PlayerInputComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_FixedMouseCursor = jsonObj["FixedMouseCursor"].bool_value();
}

void PlayerInputComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);

	outJson["FixedMouseCursor"] = m_FixedMouseCursor;
}

void BasicInputComponent::Start()
{
	// �}�E�X
	POINT nowPos;
	GetCursorPos(&nowPos);
	m_PrevMousePos = nowPos;
}

void BasicInputComponent::Update()
{
	// ���Z�b�g
	m_deltaX = 0;
	m_deltaY = 0;

	// �����̏ꍇ�͉������Ȃ�
	if (m_enable == false) {
		return;
	}

	for (int keycode = 0; keycode < 256; keycode++)
	{
		if (KdIsKeyDown(keycode, true))
		{
			// ����
			if (m_keysRepeat[keycode] == 0)
			{
				m_keys[keycode] = 1;
				m_keysRepeat[keycode] = 1;
			}
			else if (m_keysRepeat[keycode])
			{
				m_keys[keycode] = 0;
			}
			
 		}
		else
		{
			m_keys[keycode] = 0;
			m_keysRepeat[keycode] = 0;
		}
	}

	// �}�E�X
	POINT nowPos;
	GetCursorPos(&nowPos);

	// �}�E�X�̑O�񂩂�̈ړ��ʂ��v�Z
	float deltaX = 0;
	if (nowPos.x - m_PrevMousePos.x > 1 || nowPos.x - m_PrevMousePos.x < -1)
	{
		deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.5f;
	}
	float deltaY = 0;
	if (nowPos.y - m_PrevMousePos.y > 1 || nowPos.y - m_PrevMousePos.y < -1)
	{

		deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.5f;
	}

	m_deltaX = deltaX;
	m_deltaY = deltaY;

	// �}�E�X�J�[�\���𒆐S�ɌŒ肷��
	if (m_fixedMouseCursor)
	{
		RECT windowSize = { 0 };
		HWND hwnd = nullptr;
		hwnd = APP.m_Window.GetHWnd();
		GetClientRect(hwnd, &windowSize);
		POINT FixedPos = { windowSize.left + (windowSize.right / 2), windowSize.top + (windowSize.bottom / 2) };
		// �N���C�A���g�̐^�񒆂̍��W���f�B�X�v���C�̂ǂ̈ʒu�ɂ��邩�ɕϊ�����
		ClientToScreen(hwnd, &FixedPos);
		// 
		SetCursorPos(FixedPos.x, FixedPos.y);

		m_PrevMousePos = FixedPos;
	}
	else
	{
		m_PrevMousePos = nowPos;
	}

	if (m_keys[VK_F6])
	{
		m_fixedMouseCursor = false;
	}
}

void BasicInputComponent::ImGuiUpdate()
{
	// �p������ImGui����
	BaseComponent::ImGuiUpdate();

	ImGui::Checkbox(u8"�}�E�X�J�[�\���𒆐S�ɌŒ肷��", &m_fixedMouseCursor);
}

void BasicInputComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_fixedMouseCursor = jsonObj["FixedMouseCursor"].bool_value();
}

void BasicInputComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);

	outJson["FixedMouseCursor"] = m_fixedMouseCursor;
}
