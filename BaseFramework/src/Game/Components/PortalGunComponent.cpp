#include "main.h"

#include "System/GameManager.h"

#include "PortalGunComponent.h"

void PortalGunComponent::Start()
{
	// ������擾
	auto owner = GetOwner();

	// �e�擾
	auto parent = owner->GetParent();
	if (parent == nullptr)return;
	// �J�����擾
	auto cam = parent->FindObject("Cam");
	if (cam == nullptr)return;

	m_mCam = cam->GetMatrix();
}

void PortalGunComponent::Update()
{
	// ������擾
	auto owner = GetOwner();

	// ���f���R���|�[�l���g�擾
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return;

	// �A�j���[�^��i�߂�
	if (modelComp)
	{
		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// �e�擾
	auto parent = owner->GetParent();
	if (parent == nullptr)return;
	// �J�����擾
	auto cam = parent->FindObject("Cam");
	if (cam == nullptr)return;

	// �J�����̍s��ƘA��������
	KdMatrix m = owner->GetMatrix();
	KdMatrix mCam = cam->GetMatrix();
	KdMatrix mCamInv;
	m_mCam.Inverse(mCamInv);
	KdMatrix mDelta = mCamInv * mCam;
	m *= mDelta;
	owner->SetMatrix(m, false);

	// �J�����̍s���ۑ�
	m_mCam = mCam;
}

void PortalGunComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void PortalGunComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void PortalGunComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);

}
