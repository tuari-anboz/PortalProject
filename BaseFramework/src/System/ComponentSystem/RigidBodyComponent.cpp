#include "main.h"

#include "../GameManager.h"

#include "RigidBodyComponent.h"

void RigidBodyComponent::Start()
{
	// 
	UpdateRigidBody();
}

void RigidBodyComponent::Update()
{
	// 剛体とゲーム内の行列を同期させる
	if (GetOwner())
	{
		if (m_body)
		{
			m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			// キネマティックなオブジェクトはプログラム側で動かせる
			if (m_isKinematic)
			{
				m_body->SetMatrix(GetOwner()->GetMatrix());
			}
			else
			{
				KdMatrix m;
				m_body->GetMatrix(m);
				GetOwner()->SetMatrix(m, false);
			}
		}
	}
}

void RigidBodyComponent::ImGuiUpdate()
{
	// 有効/無効
	if (ImGui::Checkbox("Enable", &m_enable))
	{
		// 剛体は無効の時物理ワールドから削除する
		SetEnable(m_enable);
	}
	//
	if (ImGui::Button(u8"更新"))
	{
		UpdateRigidBody();
	}
	//
	ImGui::Checkbox(u8"Is Kinematic", &m_isKinematic);
	// 質量
	ImGui::DragFloat(u8"質量", &m_mass, 0.01f, 0, 100);
	// 摩擦力
	ImGui::DragFloat(u8"摩擦力", &m_friction, 0.01f, 0, 100);
	// 反発力
	ImGui::DragFloat(u8"反発力", &m_restitution, 0.01f, 0, 100);
	// 判定グループ
	ImGui::InputInt(u8"判定グループ", &m_group);
	// 眠らないオブジェクト(恐らくこのオブジェクトがいっぱいあるとめっちゃ重くなります)
	ImGui::Checkbox(u8"Is NonSleep", &m_NonSleep);
}

void RigidBodyComponent::UpdateRigidBody()
{
	if (m_body)
	{
		m_body->RemoveFromWorld();
	}

	// 持ち主のコライダーから形状を取得してくる
	auto boxcolcomp = GetOwner()->GetComponent<BoxColliderComponent>();
	if (boxcolcomp)
	{
		// 物理エンジンの形状にコライダーを使用するか
		if (boxcolcomp->UsePhysics())
		{
			KdMatrix m;
			m = boxcolcomp->GetRotaMat();
			m.SetScale(boxcolcomp->GetOwner()->GetMatrix().GetScale());
			m.SetPos(boxcolcomp->GetBox().Center);

			// 箱形状を作成
			SPtr<KdPhysicsShape_Box> boxShape = std::make_shared<KdPhysicsShape_Box>();
			boxShape->Create(boxcolcomp->GetLocalSize(), boxcolcomp->GetLocalSize());

			// この形状の剛体を生成
			m_body = std::make_shared<KdPhysicsRigidBody>();
			m_body->Create(boxShape, m, m_mass);	// 剛体の作成 質量は0より大きい値を指定すると物理挙動が行われる
			m_body->SetFriction(m_friction);		// 摩擦力
			m_body->SetRestitution(m_restitution);	// 反発力
			m_body->SetLinearDamping(0.0f);
			m_body->SetAngularDamping(0.050f);

			// 剛体をスリープ状態にさせない
			if(m_NonSleep)m_body->SetNonSleep();

			// コンポーネントが有効な時のみ剛体を物理ワールドへ追加する
			if (m_enable)
			{
				// 物理ワールドへ剛体を追加する
				m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			}
		}
	}

	// 持ち主のコライダーから形状を取得してくる
	auto meshcolcomp = GetOwner()->GetComponent<MeshColliderComponent>();
	if (meshcolcomp)
	{
		// 物理エンジンの形状にコライダーを使用するか
		if (meshcolcomp->UsePhysics())
		{
			// 行列取得
			KdMatrix m;
			m = meshcolcomp->GetMatrix();

			auto models = meshcolcomp->GetModel()->GetModels();
			// 一番最初のメッシュのみを物理形状として利用する
			auto mesh = models[0]->GetMesh();

			// メッシュ形状を生成
			SPtr<KdPhysicsShape_Mesh> meshShape = std::make_shared<KdPhysicsShape_Mesh>();
			meshShape->CreateFromMesh(*mesh);

			// この形状の剛体を生成
			m_body = std::make_shared<KdPhysicsRigidBody>();
			m_body->Create(meshShape, m, 0.0f);		// Mesh形状は、質量0でなければならない(物理挙動はできない固定物になる)
			m_body->SetKinematic(m_isKinematic);		// 自分で動かすような剛体はキネマティックに(BOXGENのドアなど)

			m_body->SetFriction(m_friction);		// 摩擦力
			m_body->SetRestitution(m_restitution);		// 反発力

			// 剛体をスリープ状態にさせない
			if (m_NonSleep)m_body->SetNonSleep();

			// コンポーネントが有効な時のみ剛体を物理ワールドへ追加する
			if (m_enable)
			{
				// 物理ワールドへ剛体を追加する
				m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
			}
		}
	}
}

void RigidBodyComponent::SetCollisionGroup(const int group, bool update)
{
	m_group = group;

	if (update)
	{
		m_body->RemoveFromWorld();

		// コンポーネントが有効なら剛体を物理ワールドへ追加する
		if (m_enable)
		{
			m_body->AddToWorld(GameMgr.m_physicsWorld, group, 1);
		}
	}
}

void RigidBodyComponent::SetEnable(bool enable)
{
	if (enable)
	{
		m_body->AddToWorld(GameMgr.m_physicsWorld, m_group, 1);
	}
	else
	{
		m_body->RemoveFromWorld();
	}

	m_enable = enable;
}

void RigidBodyComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_isKinematic = jsonObj["IsKinematic"].bool_value();
	m_mass = (float)jsonObj["Mass"].number_value();
	m_friction = (float)jsonObj["Friction"].number_value();
	m_restitution = (float)jsonObj["Restitution"].number_value();
	m_group = jsonObj["CollisionGroup"].int_value();
}

void RigidBodyComponent::Serialize(json11::Json::object & outJsonObj)
{
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["IsKinematic"] = m_isKinematic;
	outJsonObj["Mass"] = m_mass;
	outJsonObj["Friction"] = m_friction;
	outJsonObj["Restitution"] = m_restitution;
	outJsonObj["CollisionGroup"] = m_group;
}
