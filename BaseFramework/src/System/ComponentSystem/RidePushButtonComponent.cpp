#include "main.h"

#include "../GameManager.h"

#include "RidePushButtonComponent.h"

void RidePushButtonComponent::Start()
{
	m_as = std::make_shared<OFF>();

	
}

void RidePushButtonComponent::Update()
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

void RidePushButtonComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void RidePushButtonComponent::OnPush()
{
	// �A�j���[�V������PUSH�ɕύX


	// ��Ԃ�������Ă����ԂɕύX

}

void RidePushButtonComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void RidePushButtonComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<RidePushButtonComponent::BaseRidePushButtonAction> RidePushButtonComponent::OFF::Update()
{

	return nullptr;
}
