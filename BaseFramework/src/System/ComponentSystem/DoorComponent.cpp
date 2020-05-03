#include "main.h"

#include "../GameManager.h"

#include "DoorComponent.h"

void DoorComponent::Start()
{
	//m_as = std::make_shared<Generic>();
}

void DoorComponent::Update()
{
	// 持ち主取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// 子のドアオブジェクトを全て取得
	auto doors = owner->FindObjects("door");

	// 子のドアオブジェクトのアニメーションを全て進める
	for (auto&& door : doors)
	{
		// モデルコンポーネントを取得
		auto modelComp = door->GetComponent<ModelComponent>();
		if (modelComp == nullptr)continue;

		modelComp->GetAnimator().GetTrack()[0]->m_Enable = true;
	}

	// 行動処理
	if (m_as) {
		while (1)
		{
			// 持ち主のキャラのアドレスを入れておく
			m_as->m_pComp = this;
			// 行動処理実行
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

	// 持ち主取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// 子のドアオブジェクトを全て取得
	auto doors = owner->FindObjects("door");

	for (auto&& door : doors)
	{
		// モデルコンポーネントを取得
		auto modelComp = door->GetComponent<ModelComponent>();

		modelComp->GetAnimator().ChangeAnimeSmooth("Open", false, 1.0f);
	}

	// 状態をONに変更
	m_as = std::make_shared<ON>();
}

void DoorComponent::Close()
{
	// 持ち主取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// 子のドアオブジェクトを全て取得
	auto doors = owner->FindObjects("door");

	for (auto&& door : doors)
	{
		// モデルコンポーネントを取得
		auto modelComp = door->GetComponent<ModelComponent>();
		if (modelComp == nullptr)continue;

		modelComp->GetAnimator().ChangeAnimeSmooth("Close", false, 1.0f);
	}

	// 状態をOFFに変更
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

	// 子のドアオブジェクトを全て取得
	auto doors = owner->FindObjects("door");

	// 全てのドアオブジェクトのアニメーションが終了しているか判定
	{
		int doorCnt = doors.size();
		int endCnt = 0;
		for (auto&& door : doors)
		{
			// モデルコンポーネントを取得
			auto modelComp = door->GetComponent<ModelComponent>();
			if (modelComp == nullptr)continue;

			if (modelComp->GetAnimator().IsAnimationEnd())
			{
				endCnt++;
			}
		}
		// 終了していたらステータスをOPENに
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

	// 子のドアオブジェクトを全て取得
	auto doors = owner->FindObjects("door");

	// 全てのドアオブジェクトのアニメーションが終了しているか判定
	{
		int doorCnt = doors.size();
		int endCnt = 0;
		for (auto&& door : doors)
		{
			// モデルコンポーネントを取得
			auto modelComp = door->GetComponent<ModelComponent>();
			if (modelComp == nullptr)continue;

			if (modelComp->GetAnimator().IsAnimationEnd())
			{
				endCnt++;
			}
		}
		// 終了していたらステータスをCLOSEに
		if (endCnt == doorCnt)
		{
			m_pComp->m_state = DOOR::CLOSE;
			return std::make_shared<Generic>();
		}
	}


	return nullptr;
}