#include "main.h"

#include "System/GameManager.h"

#include "PortalGunComponent.h"

void PortalGunComponent::Start()
{
	// 持ち主取得
	auto owner = GetOwner();

	// 親取得
	auto parent = owner->GetParent();
	if (parent == nullptr)return;
	// カメラ取得
	auto cam = parent->FindObject("Cam");
	if (cam == nullptr)return;

	m_mCam = cam->GetMatrix();
}

void PortalGunComponent::Update()
{
	// 持ち主取得
	auto owner = GetOwner();

	// モデルコンポーネント取得
	auto modelComp = owner->GetComponent<ModelComponent>();
	if (modelComp == nullptr)return;

	// アニメータを進める
	if (modelComp)
	{
		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// 親取得
	auto parent = owner->GetParent();
	if (parent == nullptr)return;
	// カメラ取得
	auto cam = parent->FindObject("Cam");
	if (cam == nullptr)return;

	// カメラの行列と連動させる
	KdMatrix m = owner->GetMatrix();
	KdMatrix mCam = cam->GetMatrix();
	KdMatrix mCamInv;
	m_mCam.Inverse(mCamInv);
	KdMatrix mDelta = mCamInv * mCam;
	m *= mDelta;
	owner->SetMatrix(m, false);

	// カメラの行列を保存
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
