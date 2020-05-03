#include "main.h"

#include "../GameManager.h"

#include "BoxGeneratorComponent.h"

void BoxGeneratorComponent::Start()
{
	// 
	m_as = std::make_shared<ON>();

	m_isGenerating = true;
	// 持ち主取得
	auto owner = GetOwner();
	if (owner == nullptr)return;
	// 子のドアマネージャーオブジェクトを取得
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return;
	// ドアコンポーネントを取得
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return;
	// ドアを開ける
	doorComp->Open();

	// 最初にボックスを一つ生成する
	auto box = GameMgr.Instantiate(KDResFactory.GetJson("Box.pref"));
	if (box == nullptr)return;
	box->SetMatrix(GetOwner()->GetMatrix(), false);
	box->SetParent(GetOwner()->shared_from_this());
}

void BoxGeneratorComponent::Update()
{
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
	// 持ち主取得
	auto owner = m_pComp->GetOwner();

	// 子のボタンオブジェクトを取得
	auto button = owner->FindObject("Button");
	if (button)
	{

		auto ridebutton = button->GetComponent<RideButtonComponent>();
		if (ridebutton)
		{
			// ボタンがオフなら生成中を解除
			if (ridebutton->GetOnPush() == false)
			{
				m_pComp->m_isGenerating = false;
			}
			// 生成中じゃない時にボタンがオンになったら生成開始
			if (m_pComp->m_isGenerating == false)
			{
				if (ridebutton->GetOnPush())
				{
					m_pComp->m_isGenerating = true;
					// 持ち主取得
					auto owner = m_pComp->GetOwner();
					if (owner == nullptr)return std::make_shared<Generic>();
					// 子のドアマネージャーオブジェクトを取得
					auto doorManager = owner->FindObject("doormanager");
					if (doorManager == nullptr)return std::make_shared<Generic>();
					// ドアコンポーネントを取得
					auto doorComp = doorManager->GetComponent<DoorComponent>();
					if (doorComp == nullptr)return std::make_shared<Generic>();
					// ドアを開ける
					doorComp->Open();
					// 古いボックスを削除する
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
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return std::make_shared<Generic>();
	// 子のドアマネージャーオブジェクトを取得
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return std::make_shared<Generic>();
	// ドアコンポーネントを取得
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return std::make_shared<Generic>();

	// ドアが開ききったら閉じる
	if (doorComp->GetState() == DOOR::OPEN)
	{
		doorComp->Close();
		return std::make_shared<OFF>();
	}

	return nullptr;
}

SPtr<BoxGeneratorComponent::BaseBoxGeneratorAction> BoxGeneratorComponent::OFF::Update()
{
	// 持ち主取得
	auto owner = m_pComp->GetOwner();
	if (owner == nullptr)return std::make_shared<Generic>();
	// 子のドアマネージャーオブジェクトを取得
	auto doorManager = owner->FindObject("doormanager");
	if (doorManager == nullptr)return std::make_shared<Generic>();
	// ドアコンポーネントを取得
	auto doorComp = doorManager->GetComponent<DoorComponent>();
	if (doorComp == nullptr)return std::make_shared<Generic>();
	if (doorComp->GetState() == DOOR::CLOSE)
	{
		// ボックスを生成
		auto box = GameMgr.Instantiate(KDResFactory.GetJson("Box.pref"));
		if (box == nullptr)return std::make_shared<Generic>();
		box->SetMatrix(owner->GetMatrix(), false);
		box->SetParent(owner->shared_from_this());

		return std::make_shared<Generic>();
	}

	return nullptr;
}


