#include <main.h>

#include "System/GameManager.h"

#include "AutomaticDoorComponent.h"

void AutomaticDoorComponent::Start()
{
	auto owner = GetOwner();
	if (owner == nullptr)return;

	m_as = std::make_shared<Generic>();

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	model->GetAnimator().ChangeAnime("Save", false);
}

void AutomaticDoorComponent::Update()
{
	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// �A�j���[�V������i�߂�
	model->GetAnimator().GetTrack()[0]->m_Enable = true;

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

void AutomaticDoorComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void AutomaticDoorComponent::Open()
{
	// ���ɊJ���Ă����牽�����Ȃ�
	if(m_state == AUTODOOR::OPEN)return;
	m_state = AUTODOOR::OPEN;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// �A�j���[�V������ύX����
	model->GetAnimator().ChangeAnimeSmooth("Open", false, 1);

	m_as = std::make_shared<OpenAction>();
	
}

void AutomaticDoorComponent::Close()
{
	// ���ɕ܂��Ă����牽�����Ȃ�
	if(m_state == AUTODOOR::CLOSE)return;
	m_state = AUTODOOR::CLOSE;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// �A�j���[�V������ύX����
	model->GetAnimator().ChangeAnimeSmooth("Close", false, 1.0f);

	m_as = std::make_shared<CloseAction>();
}

void AutomaticDoorComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void AutomaticDoorComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<AutomaticDoorComponent::BaseAutomaticDoorAction> AutomaticDoorComponent::Generic::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	// �q�̃{�^���������ꂽ���ǂ����𔻒肵�ĊJ����
	auto button = owner->FindObject("Button");
	if (button == nullptr)return nullptr;
	auto ridebutton = button->GetComponent<RideButtonComponent>();
	if (ridebutton == nullptr) return nullptr;
	if (ridebutton->GetOnPush())
	{
		m_pComp->Open();
	}
	else
	{
		m_pComp->Close();
	}

	// �����h�A�̏ꍇ�^�O�t�����ꂽ�{�b�N�X�R���C�_�[��
	auto boxcolls = owner->GetComponents<BoxColliderComponent>();
	for (auto&& coll : boxcolls)
	{
		if (coll->GetTag() == "AutomaticDoor")
		{
		}
	}
	
	return nullptr;
}

SPtr<AutomaticDoorComponent::BaseAutomaticDoorAction> AutomaticDoorComponent::OpenAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	auto boxcolcomps = owner->GetComponents<BoxColliderComponent>();
	
	auto rigidbodycomp = owner->GetComponent <RigidBodyComponent>();
	if (rigidbodycomp == nullptr) return nullptr;

	// �A�j���[�V�������I�������瓖���蔻��������Ė}�p�s���ɖ߂�
	if (model->GetAnimator().IsAnimationEnd())
	{
		// �h�A���J�����瓖���蔻�������
		for (auto&& boxcol : boxcolcomps)
		{
			boxcol->SetEnable(false);
		}
		//rigidbodycomp->SetEnable(false);

		return std::make_shared<Generic>();
	}

	return nullptr;
}

SPtr<AutomaticDoorComponent::BaseAutomaticDoorAction> AutomaticDoorComponent::CloseAction::Update()
{
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	auto boxcolcomps = owner->GetComponents<BoxColliderComponent>();

	auto rigidbodycomp = owner->GetComponent<RigidBodyComponent>();
	if (rigidbodycomp == nullptr)return nullptr;

	// �A�j���[�V�������I�������瓖���蔻��𕜊����Ė}�p�s���ɖ߂�
	if (model->GetAnimator().IsAnimationEnd())
	{
		// �h�A���J�����瓖���蔻���߂�
		for (auto&& boxcol : boxcolcomps)
		{
			boxcol->SetEnable(true);
		}
		//rigidbodycomp->SetEnable(true);

		return std::make_shared<Generic>();
	}


	return nullptr;
}
