#include "main.h"

#include "../GameManager.h"

#include "DoorComponent.h"

void DoorComponent::Start()
{
	//m_as = std::make_shared<Generic>();
}

void DoorComponent::Update()
{
	// ������擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �q�̃h�A�I�u�W�F�N�g��S�Ď擾
	auto doors = owner->FindObjects("door");

	// �q�̃h�A�I�u�W�F�N�g�̃A�j���[�V������S�Đi�߂�
	for (auto&& door : doors)
	{
		// ���f���R���|�[�l���g���擾
		auto modelComp = door->GetComponent<ModelComponent>();
		if (modelComp == nullptr)continue;

		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// �s������
	if (m_as) {
		while (1)
		{
			// ������̃L�����̃A�h���X�����Ă���
			m_as->m_pComp = this;
			// �s���������s
			auto p = m_as->Update();
			if (p) {
				m_as = p;
			}
			else
			{
				break;
			}
		}
	}
}

void DoorComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void DoorComponent::Open()
{

	// ������擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �q�̃h�A�I�u�W�F�N�g��S�Ď擾
	auto doors = owner->FindObjects("door");

	for (auto&& door : doors)
	{
		// ���f���R���|�[�l���g���擾
		auto modelComp = door->GetComponent<ModelComponent>();

		modelComp->GetAnimator().ChangeAnimeSmooth("Open", false, 1.0f);
	}

	// ��Ԃ�ON�ɕύX
	m_as = std::make_shared<ON>();
}

void DoorComponent::Close()
{
	// ������擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �q�̃h�A�I�u�W�F�N�g��S�Ď擾
	auto doors = owner->FindObjects("door");

	for (auto&& door : doors)
	{
		// ���f���R���|�[�l���g���擾
		auto modelComp = door->GetComponent<ModelComponent>();
		if (modelComp == nullptr)continue;

		modelComp->GetAnimator().ChangeAnimeSmooth("Close", false, 1.0f);
	}

	// ��Ԃ�OFF�ɕύX
	m_as = std::make_shared<OFF>();
}

void DoorComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void DoorComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<DoorComponent::BaseDoorAction> DoorComponent::Generic::Update()
{

	return nullptr;
}

SPtr<DoorComponent::BaseDoorAction> DoorComponent::ON::Update()
{
	// 
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return std::make_shared<Generic>();

	// �q�̃h�A�I�u�W�F�N�g��S�Ď擾
	auto doors = owner->FindObjects("door");

	// �S�Ẵh�A�I�u�W�F�N�g�̃A�j���[�V�������I�����Ă��邩����
	{
		int doorCnt = doors.size();
		int endCnt = 0;
		for (auto&& door : doors)
		{
			// ���f���R���|�[�l���g���擾
			auto modelComp = door->GetComponent<ModelComponent>();
			if (modelComp == nullptr)continue;

			if (modelComp->GetAnimator().IsAnimationEnd())
			{
				endCnt++;
			}
		}
		// �I�����Ă�����X�e�[�^�X��OPEN��
		if (endCnt == doorCnt)
		{
			m_pComp->m_state = DOOR::OPEN;
			return std::make_shared<Generic>();
		}
	}

	return nullptr;
}
SPtr<DoorComponent::BaseDoorAction> DoorComponent::OFF::Update()
{
	// 
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr) return std::make_shared<Generic>();

	// �q�̃h�A�I�u�W�F�N�g��S�Ď擾
	auto doors = owner->FindObjects("door");

	// �S�Ẵh�A�I�u�W�F�N�g�̃A�j���[�V�������I�����Ă��邩����
	{
		int doorCnt = doors.size();
		int endCnt = 0;
		for (auto&& door : doors)
		{
			// ���f���R���|�[�l���g���擾
			auto modelComp = door->GetComponent<ModelComponent>();
			if (modelComp == nullptr)continue;

			if (modelComp->GetAnimator().IsAnimationEnd())
			{
				endCnt++;
			}
		}
		// �I�����Ă�����X�e�[�^�X��CLOSE��
		if (endCnt == doorCnt)
		{
			m_pComp->m_state = DOOR::CLOSE;
			return std::make_shared<Generic>();
		}
	}


	return nullptr;
}