#include "main.h"

#include "../GameManager.h"

#include "UIButtonComponent.h"

void UIButtonComponent::Start()
{
	m_as = std::make_shared<OFF>();
}

void UIButtonComponent::Update()
{
	// 無効の時は何もしない
	if (m_enable == false) return;

	// 行動処理
	if (m_as)
	{
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pComp = this;
		// 行動処理実行
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}
}

void UIButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// 画像ファイル
	if (ImGuiResourceButton(u8"選択中の画像ファイル", m_selectedTexFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" }))
	{
		m_buttonSelectedTex = KDResFactory.GetTexture(m_selectedTexFilename);
	}

	// 画像ファイル
	if (ImGuiResourceButton(u8"非選択中の画像ファイル", m_TexFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" }))
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
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// スプライトコンポーネント取得
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// 親オブジェクトの入力コンポーネントを取得
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// マウスカーソルの位置取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	HWND hwnd = nullptr;
	hwnd = APP.m_Window.GetHWnd();
	ScreenToClient(hwnd, &cursorPos);

	// ボタンの状態をOFFへ変更
	m_pComp->m_buttons |= UIButton::OFF;

	// スプライトの上にマウスカーソルがあるか
	if (spriteComp->IsOnTexture(KdVec2(cursorPos.x, cursorPos.y)))
	{
		return std::make_shared<ONBUTTON>();
	}

	return nullptr;
}

SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::ONBUTTON::Update()
{
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// スプライトコンポーネント取得
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// タイトルオブジェクトの入力コンポーネントを取得
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// マウスカーソルの位置取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	HWND hwnd = nullptr;
	hwnd = APP.m_Window.GetHWnd();
	ScreenToClient(hwnd, &cursorPos);

	// ボタンの状態を画像の上にマウスカーソルがある状態に変更
	m_pComp->m_buttons |= UIButton::ONBUTTON;

	// スプライトの上にマウスカーソルがあるか
	if (spriteComp->IsOnTexture(KdVec2(cursorPos.x, cursorPos.y)) == false)
	{
		spriteComp->ChangeTex(m_pComp->m_buttonTex);
		return std::make_shared<OFF>();
	}

	// マウスのクリック判定
	if (input->m_keys[VK_LBUTTON])
	{
		return std::make_shared<Press>();
	}

	return nullptr;
}


SPtr<UIButtonComponent::BaseUIButtonAction> UIButtonComponent::Press::Update()
{
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// スプライトコンポーネント取得
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// タイトルオブジェクトの入力コンポーネントを取得
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	// ボタンの状態をプレスに変更
	m_pComp->m_buttons |= UIButton::PRESS;

	// ボタン選択時のテクスチャがない場合は別の処理をする
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
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// スプライトコンポーネント取得
	auto spriteComp = owner->GetComponent<SpriteComponent>();
	if (spriteComp == nullptr) return nullptr;

	// タイトルオブジェクトの入力コンポーネントを取得
	auto input = owner->GetParent()->GetComponent <InputComponent>();

	return nullptr;
}

