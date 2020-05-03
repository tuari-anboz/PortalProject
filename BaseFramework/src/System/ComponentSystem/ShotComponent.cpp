#include "main.h"

#include "../Gamemanager.h"

#include "ShotComponent.h"

void ShotComponent::Update()
{
	if (m_enable == false)return;
	auto owner = GetOwner();

	// ����
	{
		// �R���C�_�[�R���|�[�l���g���u�S�āv�擾����
		auto colliders = owner->GetComponents<RayColliderComponent>();
		for (auto&& coll : colliders)
		{
			// �ˌ��n�̔���
			if (coll->GetTag() == m_tag)
			{
				// �q�b�g���̎��s�����֐���o�^
				coll->m_onHitStay = [this,owner](BaseColliderComponent* collider)
				{
					// �q�b�g�����z��(Collider)�S��
					for (auto&& res : collider->GetResults())
					{
						m_hitPos = res.Pos;
					}
				};
			}
		}
	}
}

void ShotComponent::Draw()
{
}

void ShotComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	ImGuiInputString("Tag", m_tag);
}

void ShotComponent::EditorDraw()
{
	if (m_enable == false)return;


}

void ShotComponent::Deserialize(const json11::Json & jsonObj)
{
	{
		// �p������Deserialize�����s
		BaseComponent::Deserialize(jsonObj);

		m_range = (float)jsonObj["Range"].number_value();
		m_dir.Deserialize(jsonObj["Dir"]);
		m_hitPos.Deserialize(jsonObj["HitPos"]);
		m_shotPos.Deserialize(jsonObj["ShotPos"]);
	}
}

void ShotComponent::Serialize(json11::Json::object & outJson)
{
		// �p������Serialize�����s
		BaseComponent::Serialize(outJson);

		outJson["Range"] = m_range;
		m_dir.Serialize();
		m_hitPos.Serialize();
		m_shotPos.Serialize();
}



