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

	// アニメーションを進める
	model->GetAnimator().GetTrack()[0]->m_Enable = true;

	// 行動処理
	if (m_as)
	{
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pComp = this;
		// 行動処理実行
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
	// 既に開いていたら何もしない
	if(m_state == AUTODOOR::OPEN)return;
	m_state = AUTODOOR::OPEN;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// アニメーションを変更する
	model->GetAnimator().ChangeAnimeSmooth("Open", false, 1);

	m_as = std::make_shared<OpenAction>();
	
}

void AutomaticDoorComponent::Close()
{
	// 既に閉まっていたら何もしない
	if(m_state == AUTODOOR::CLOSE)return;
	m_state = AUTODOOR::CLOSE;

	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// アニメーションを変更する
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

	// 子のボタンが押されたかどうかを判定して開ける
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

	// 自動ドアの場合タグ付けされたボックスコライダーが
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

	// アニメーションが終了したら当たり判定を消して凡用行動に戻る
	if (model->GetAnimator().IsAnimationEnd())
	{
		// ドアが開いたら当たり判定を消す
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

	// アニメーションが終了したら当たり判定を復活して凡用行動に戻る
	if (model->GetAnimator().IsAnimationEnd())
	{
		// ドアが開いたら当たり判定を戻す
		for (auto&& boxcol : boxcolcomps)
		{
			boxcol->SetEnable(true);
		}
		//rigidbodycomp->SetEnable(true);

		return std::make_shared<Generic>();
	}


	return nullptr;
}
