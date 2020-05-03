#include "main.h"

#include "../GameManager.h"

#include "BoxGeneratorComponent.h"

void BoxGeneratorComponent::Start()
{
	// 
	m_as = std::make_shared<ON>();

	m_isGenerating = true;
	// ������擾
	auto owner = GetOwner();
	if (owner == nullptr)return;
	// �q�̃h�A�}�l�[�W���[�I�u�W�F�N�g���擾
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return;
	// �h�A�R���|�[�l���g���擾
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return;
	// �h�A���J����
	doorComp->Open();

	// �ŏ��Ƀ{�b�N�X�����������
	auto box = GameMgr.Instantiate(KDResFactory.GetJson("Box.pref"));
	if (box == nullptr)return;
	box->SetMatrix(GetOwner()->GetMatrix(), false);
	box->SetParent(GetOwner()->shared_from_this());
}

void BoxGeneratorComponent::Update()
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

void BoxGeneratorComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void BoxGeneratorComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void BoxGeneratorComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<BoxGeneratorComponent::BaseBoxGeneratorAction> BoxGeneratorComponent::Generic::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();

	// �q�̃{�^���I�u�W�F�N�g���擾
	auto button = owner->FindObject("Button");
	if (button)
	{

		auto ridebutton = button->GetComponent<RideButtonComponent>();
		if (ridebutton)
		{
			// �{�^�����I�t�Ȃ琶����������
			if (ridebutton->GetOnPush() == false)
			{
				m_pComp->m_isGenerating = false;
			}
			// ����������Ȃ����Ƀ{�^�����I���ɂȂ����琶���J�n
			if (m_pComp->m_isGenerating == false)
			{
				if (ridebutton->GetOnPush())
				{
					m_pComp->m_isGenerating = true;
					// ������擾
					auto owner = m_pComp->GetOwner();
					if (owner == nullptr)return std::make_shared<Generic>();
					// �q�̃h�A�}�l�[�W���[�I�u�W�F�N�g���擾
					auto doorManager = owner->FindObject("doormanager");
					if (doorManager == nullptr)return std::make_shared<Generic>();
					// �h�A�R���|�[�l���g���擾
					auto doorComp = doorManager->GetComponent<DoorComponent>();
					if (doorComp == nullptr)return std::make_shared<Generic>();
					// �h�A���J����
					doorComp->Open();
					// �Â��{�b�N�X���폜����
					auto box = owner->FindObject("Box");
					if (box == nullptr)return std::make_shared<Generic>();
					box->Delete();

					return std::make_shared<ON>();
				}
			}
		}
	}

	return nullptr;
}

SPtr<BoxGeneratorComponent::BaseBoxGeneratorAction> BoxGeneratorComponent::ON::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return std::make_shared<Generic>();
	// �q�̃h�A�}�l�[�W���[�I�u�W�F�N�g���擾
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return std::make_shared<Generic>();
	// �h�A�R���|�[�l���g���擾
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return std::make_shared<Generic>();

	// �h�A���J�������������
	if (doorComp->GetState() == DOOR::OPEN)
	{
		doorComp->Close();
		return std::make_shared<OFF>();
	}

	return nullptr;
}

SPtr<BoxGeneratorComponent::BaseBoxGeneratorAction> BoxGeneratorComponent::OFF::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return std::make_shared<Generic>();
	// �q�̃h�A�}�l�[�W���[�I�u�W�F�N�g���擾
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return std::make_shared<Generic>();
	// �h�A�R���|�[�l���g���擾
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return std::make_shared<Generic>();
	if (doorComp->GetState() == DOOR::CLOSE)
	{
		// �{�b�N�X�𐶐�
		auto box = GameMgr.Instantiate(KDResFactory.GetJson("Box.pref"));
		if (box == nullptr)return std::make_shared<Generic>();
		box->SetMatrix(owner->GetMatrix(), false);
		box->SetParent(owner->shared_from_this());

		return std::make_shared<Generic>();
	}

	return nullptr;
}


