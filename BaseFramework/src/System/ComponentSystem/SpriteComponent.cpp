#include "main.h"

#include "../GameManager.h"

#include "SpriteComponent.h"

//==============================================
//
// Sprite�R���|�[�l���g
//
//==============================================
void SpriteComponent::LoadTex(const std::string & texFilename)
{
	m_texFilename = texFilename;
	m_tex = KDResFactory.GetTexture(texFilename);
}

void SpriteComponent::DrawSprite()
{
	if (m_enable == false)return;

	if (m_tex) {
		ShMgr.m_KdSpriteSh.Draw2D(*m_tex, GetOwner()->GetMatrix(), &KdVec4(1,1,1,m_texAlpha), true);
	}
}

void SpriteComponent::Deserialize(const json11::Json & jsonObj)
{
	// �p�����̃f�[�^���f�V���A���C�Y
	BaseComponent::Deserialize(jsonObj);

	// 
	std::string texFileName = jsonObj["TexFilename"].string_value();

	LoadTex(texFileName);

	if (jsonObj["TexAlpha"].is_number())
	{
		m_texAlpha = (float)jsonObj["TexAlpha"].number_value();
	}
}

void SpriteComponent::Serialize(json11::Json::object & outJsonObj)
{
	// �p�����̃f�[�^���V���A���C�Y
	BaseComponent::Serialize(outJsonObj);

	// 
	outJsonObj["TexFilename"] = m_tex ? m_tex->GetFilepath().string() : "";
	outJsonObj["TexAlpha"] = m_texAlpha;
}

void SpriteComponent::ImGuiUpdate()
{
	//
	BaseComponent::ImGuiUpdate();

	// �摜�t�@�C��
	if (ImGuiResourceButton(u8"�摜�t�@�C��", m_texFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" })) {
		LoadTex(m_texFilename);
	}

	// �����x
	ImGui::SliderFloat(u8"�����x", &m_texAlpha, 0, 1);
}

bool SpriteComponent::IsOnTexture(const KdVec2 & pos)
{
	auto owner = GetOwner();
	if (owner == nullptr)return false;

	if (
		pos.x >= owner->GetMatrix().GetPos().x - GetTexture()->GetInfo().Width / 2
		&& pos.x <= (owner->GetMatrix().GetPos().x + GetTexture()->GetInfo().Width / 2)
		&& pos.y >= owner->GetMatrix().GetPos().y - GetTexture()->GetInfo().Height / 2
		&& pos.y <= (owner->GetMatrix().GetPos().y + GetTexture()->GetInfo().Height / 2)
		)
	{
		return true;
	}

	return false;
}
