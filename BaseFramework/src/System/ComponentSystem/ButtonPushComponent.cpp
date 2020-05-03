#include "main.h"

#include "../Gamemanager.h"

#include "ButtonPushComponent.h"

void ButtonPushComponent::Update()
{
	// ������擾
	auto owner = GetOwner();

	// �΃{�^���X�C�b�`
	{
		// ���C����R���C�_�[�R���|�[�l���g���擾
		auto collider = owner->GetComponent<RayColliderComponent>();
		
		if (collider)
		{
			// �q�b�g���̎��s�����֐���o�^
			collider->m_onHitStay;
		}
	}
	
}

void ButtonPushComponent::ImGuiUpdate()
{
}

void ButtonPushComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void ButtonPushComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);
}
