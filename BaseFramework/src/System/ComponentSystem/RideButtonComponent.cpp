#include "main.h"

#include "../GameManager.h"

#include "RideButtonComponent.h"

void RideButtonComponent::Start()
{
	m_as = std::make_shared<Generic>();
}

void RideButtonComponent::Update()
{
	// ������擾
	auto owner = GetOwner();
	if (owner == nullptr)return;
	// ���f���R���|�[�l���g�擾
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return;

	// �A�j���[�^��i�߂�
	if (modelComp)
	{
		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// �s������
	if (m_as)
	{
		// ������̃L�����̃A�h���X�����Ă���
		m_as->m_pComp = this;
		// �s���������s
		auto p = m_as->Update();
		if (p) 
		{
			m_as = p;
		}
	}
}

void RideButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void RideButtonComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void RideButtonComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::Generic::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;
	// ���f���R���|�[�l���g�擾
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return nullptr;

	if (m_pComp->m_isOn)
	{
		return std::make_shared<ON>();
	}
	else
	{
		return std::make_shared<OFF>();
	}


	return nullptr;
}


SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::OFF::Update()
{
	// ������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;
	// ���f���R���|�[�l���g�擾
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return nullptr;

	// ������Ă����ԂȂ�s����ύX
	if (m_pComp->m_isOn)
	{
		return std::make_shared<ON>();
	}

	// �R���C�_�[�R���|�[�l���g�擾
	auto coll = owner->GetComponent<MeshColliderComponent>();
	if (coll)
	{
		// �q�b�g�����ŏ��̈��̂ݎ��s�����֐���o�^
		coll->m_onHitStay = [this](BaseColliderComponent* collider)
		{
			// ������Ă��Ȃ�������
			if (m_pComp->m_isOn == false)
			{
				auto modelComp = m_pComp->GetOwner()->GetComponent<ModelComponent>();
				if (modelComp == nullptr)return;
				// ���݂̃A�j���[�V�������I�����Ă�����
				if (modelComp->GetAnimator().IsAnimationEnd())
				{
					// �A�j���[�V������PUSH�ɕύX
					modelComp->GetAnimator().ChangeAnime("Push", false);

					// ��Ԃ��ύX
					m_pComp->m_isOn = true;
				}
			}
		};
	}

	return nullptr;
}

SPtr<RideButtonComponent::BaseRidePushButtonAction> RideButtonComponent::ON::Update()
{
	// ��������擾
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return nullptr;

	// ������Ă����ԂȂ�s����ύX
	if (m_pComp->m_isOn == false)
	{
		return std::make_shared<OFF>();
	}

	// �R���C�_�[�R���|�[�l���g�擾
	auto coll = owner->GetComponent<MeshColliderComponent>();
	if (coll)
	{
		// �N���Ԃ����ĂȂ��ꍇ���������s����
		if (coll->GetResults().size() <= 0)
		{
			// ������Ă�����
			if (m_pComp->m_isOn)
			{
				auto modelComp = m_pComp->GetOwner()->GetComponent<ModelComponent>();
				if (modelComp == nullptr)return nullptr;
				// ���݂̃A�j���[�V�������I�����Ă�����
				if (modelComp->GetAnimator().IsAnimationEnd())
				{
					// �A�j���[�V������PUSH�ɕύX
					modelComp->GetAnimator().ChangeAnime("Off", false);

					// ��Ԃ��ύX
					m_pComp->m_isOn = false;
				}
			}
		}

	}

	return nullptr;
}
