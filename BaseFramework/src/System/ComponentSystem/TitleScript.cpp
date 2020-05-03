#include "main.h"

#include "../GameManager.h"

#include "TitleScript.h"

void TitleScript::Start()
{
	m_as = std::make_shared<Basic>();
}

void TitleScript::Update()
{
	// �I�[�i�[�擾
	auto owner = GetOwner();

	// ���̓R���|�[�l���g�擾
	auto input = GameMgr.m_level->GetRoot()->GetComponent<BasicInputComponent>();
	if (input) {
		// A�{�^��(Z�L�[)
		if (input->m_keys['Z'])
		{
			auto loadlevel = owner->GetComponent<LoadLevelComponent>();
			// ���x���ύX
			loadlevel->LoadLevel("data/StageDataLevelEditor.json");
			GameMgr.m_Editor_Log.AddLog("OEK");
		}

	}

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

SPtr<TitleScript::BaseTitleAction> TitleScript::Basic::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return nullptr;

	// �q�̃{�^���I�u�W�F�N�g����������
	// �{�^���I�u�W�F�N�g���̋@�\����������

	return nullptr;
}

SPtr<TitleScript::BaseTitleAction> TitleScript::StageSelect::Update()
{

	return nullptr;
}