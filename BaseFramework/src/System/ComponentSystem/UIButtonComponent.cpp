#include "main.h"

#include "../GameManager.h"

#include "UIButtonComponent.h"

void UIButtonComponent::Start()
{
	m_as = std::make_shared<OFF>();
}

void UIButtonComponent::Update()
{
	// �����̎��͉������Ȃ�
	if (m_enable == false) return;

	// �s������
	if (m_as)
	{
		// ������̃L�����̃A�h���X�����Ă���
		m_as->m_pComp = this;
		// �s���������s
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}
}

void UIButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// �摜�t�@�C��
	if (ImGuiResourceButton(u8"�I�𒆂̉摜�t�@�C��", m_selectedTexFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" }))
	{
		m_buttonSelectedTex = KDResFactory.GetTexture(m_selectedTexFilename);
	}

	// �摜�t�@�C��
	if (ImGuiResourceButton(u8"��I�𒆂̉摜�t�@�C��", m_TexFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" }))
	{
		m_buttonTex = KDResFactory.GetTexture(m_TexFilename);
	}
}

void UIButtonComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_selectedTexFilename = jsonObj["SelectedTexFilename"].string_value();
	m_TexFilename = jsonObj["TexFilename"].string_value();
}

void UIButtonComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["SelectedTexFilename"] = m_selectedTexFilename;
	outJsonObj["TexFilename"] = m_TexFilename;
}

SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::OFF::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// �X�v���C�g�R���|�[�l���g�擾
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// �e�I�u�W�F�N�g�̓��̓R���|�[�l���g���擾
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// �}�E�X�J�[�\���̈ʒu�擾
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	HWND hwnd = nullptr;
	hwnd = APP.m_Window.GetHWnd();
	ScreenToClient(hwnd, &cursorPos);

	// �{�^���̏�Ԃ�OFF�֕ύX
	m_pComp->m_buttons |= UIButton::OFF;

	// �X�v���C�g�̏�Ƀ}�E�X�J�[�\�������邩
	if (spriteComp->IsOnTexture(KdVec2(cursorPos.x, cursorPos.y)))
	{
		return std::make_shared<ONBUTTON>();
	}

	return nullptr;
}

SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::ONBUTTON::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// �X�v���C�g�R���|�[�l���g�擾
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// �^�C�g���I�u�W�F�N�g�̓��̓R���|�[�l���g���擾
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// �}�E�X�J�[�\���̈ʒu�擾
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	HWND hwnd = nullptr;
	hwnd = APP.m_Window.GetHWnd();
	ScreenToClient(hwnd, &cursorPos);

	// �{�^���̏�Ԃ��摜�̏�Ƀ}�E�X�J�[�\���������ԂɕύX
	m_pComp->m_buttons |= UIButton::ONBUTTON;

	// �X�v���C�g�̏�Ƀ}�E�X�J�[�\�������邩
	if (spriteComp->IsOnTexture(KdVec2(cursorPos.x, cursorPos.y)) == false)
	{
		spriteComp->ChangeTex(m_pComp->m_buttonTex);
		return std::make_shared<OFF>();
	}

	// �}�E�X�̃N���b�N����
	if (input->m_keys[VK_LBUTTON])
	{
		return std::make_shared<Press>();
	}

	return nullptr;
}


SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::Press::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// �X�v���C�g�R���|�[�l���g�擾
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// �^�C�g���I�u�W�F�N�g�̓��̓R���|�[�l���g���擾
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// �{�^���̏�Ԃ��v���X�ɕύX
	m_pComp->m_buttons |= UIButton::PRESS;

	// �{�^���I�����̃e�N�X�`�����Ȃ��ꍇ�͕ʂ̏���������
	if (m_pComp->m_buttonSelectedTex == nullptr)
	{

	}
	else
	{
		spriteComp->ChangeTex(m_pComp->m_buttonSelectedTex);
	}

	return nullptr;
}

SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::Drop::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// �X�v���C�g�R���|�[�l���g�擾
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// �^�C�g���I�u�W�F�N�g�̓��̓R���|�[�l���g���擾
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	return nullptr;
}

