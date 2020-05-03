#include "main.h"

#include "System/GameManager.h"

#include "GettingBoxComponent.h"

void GettingBoxComponent::Start()
{
	m_as = std::make_shared<Basic>();
}

void GettingBoxComponent::Update()
{
	// 持ち主取得
	auto owner = GetOwner();

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

void GettingBoxComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();
}

void GettingBoxComponent::GetBox()
{
	auto owner = GetOwner();

	// ボックスを取得していたらリリースしまだ取得していないなら取得する
	if (m_GettingBox)
	{
		// ボックスを取得していない状態にする
		m_GettingBox = false;
	}
	else
	{
		auto colls = owner->GetComponents<RayColliderComponent>();
		for (auto&& coll : colls)
		{
			if (coll->GetTag() == "GettingBox")
			{
				coll->m_onHitStay = [this, coll](BaseColliderComponent* collider)
				{
					for (auto&& res : collider->GetResults())
					{
						if (res.Collider->GetTag() == "GettingBox")
						{
							auto owner = GetOwner();

							// ボックスのWPを格納
							m_pBox = res.Collider->GetOwner()->weak_from_this();

							// ボックスの剛体をオフに
							auto rigidbody = collider->GetOwner()->GetComponent<RigidBodyComponent>();
							if (rigidbody)
							{
								rigidbody->SetEnable(false);
							}
							// ボックスを取得した状態にする
							m_GettingBox = true;

							// 対象を非アクティブ化する
							res.Collider->GetOwner()->SetActive(false);
						}
					}
				};
			}
		}
	}
}

void GettingBoxComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void GettingBoxComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);
}

SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Basic::Update()
{
	// ボックスを取得時に取得時行動に切り替える
	if (m_pComp->m_GettingBox)
	{
		return std::make_shared<Get>();
	}

	return nullptr;
}

SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Get::Update()
{
	auto owner = m_pComp->GetOwner();

	// ボックスが削除されていたら基本行動に戻る
	if (m_pComp->m_pBox.expired())
	{
		m_pComp->m_GettingBox = false;
		return std::make_shared<Basic>();
	}
	// ボックスを離した時行動を切り替える
	if (m_pComp->m_GettingBox == false)
	{
		return std::make_shared<Release>();
	}

	// ボックスの行列を持ち主と同じ行列にする
	m_pComp->m_pBox.lock()->SetMatrix(owner->GetMatrix(), false);

	return nullptr;
}

// 
SPtr<GettingBoxComponent::BaseGettingBoxAction> GettingBoxComponent::Release::Update()
{
	auto owner = m_pComp->GetOwner();

	// ボックスが削除されていたら基本行動に戻る
	if (m_pComp->m_pBox.expired())
	{
		m_pComp->m_GettingBox = false;
		return std::make_shared<Basic>();
	}

	// ボックスをアクティブ状態にする
	auto box = m_pComp->m_pBox.lock();
	if (box)
	{
		box->SetActive(true);
	}
	// 剛体をオンにして視点の方向へ投げる
	auto rigidbody = box->GetComponent<RigidBodyComponent>();
	if (rigidbody)
	{
		rigidbody->SetEnable(true);
		rigidbody->UpdateRigidBody();
		rigidbody->GetRigidBody()->SetMatrix(owner->GetMatrix());
		KdVec3 throwVec = owner->GetMatrix().GetZAxis();
		throwVec.Normalize();
		rigidbody->SetLinearVelocity(throwVec * 2.0f);
	}

	m_pComp->m_GettingBox = false;
	m_pComp->m_pBox.reset();

	return std::make_shared<Basic>();
}
