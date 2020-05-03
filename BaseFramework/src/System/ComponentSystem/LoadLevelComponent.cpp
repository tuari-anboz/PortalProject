#include "main.h"

#include "../GameManager.h"

#include "LoadLevelComponent.h"

void LoadLevelComponent::Start()
{
	m_as = std::make_shared<Generic>();
}

void LoadLevelComponent::Update()
{
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

void LoadLevelComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// ���x���t�@�C��
	ImGuiResourceButton("Level File", m_levelFilename, { ".json" });
}

void LoadLevelComponent::LoadLevel(const std::string & levelfilename)
{
	m_levelFilename = levelfilename;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �t�F�[�h�I�u�W�F�N�g��T��
	auto fadeobj = GameMgr.m_level->GetRoot()->FindObject("Fade");
	if (fadeobj == nullptr)return;
	// �t�F�[�h�R���|�[�l���g���g�p���ăt�F�[�h�C���t�F�[�h�A�E�g������
	auto fade = fadeobj->GetComponent<FadeComponent>();
	if (fade == nullptr)return;
	fade->FadeOut();

	m_as = std::make_shared<LoadLevelAction>();
}

void LoadLevelComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void LoadLevelComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<LoadLevelComponent::BaseLoadLevelAction> LoadLevelComponent::Generic::Update()
{
	return nullptr;
}

SPtr<LoadLevelComponent::BaseLoadLevelAction> LoadLevelComponent::LoadLevelAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// �t�F�[�h�A�E�g���I��������w�肵�����x�������[�h����
	if (GameMgr.m_fade)
	{
		GameMgr.ChangeLevel(m_pComp->m_levelFilename);
	}

	return nullptr;
}
