#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"

#include "CollisionFunctions.h"

//==============================================
//
// Box Colliderコンポーネント
//
//==============================================

bool BoxColliderComponent::HitTest(BaseColliderComponent * obj)
{
	// vs Sphere
	if (obj->GetType() == Types::Sphere)
	{
		SphereColliderComponent* sph = static_cast<SphereColliderComponent*>(obj);

		return CollisionFunctions::SphereToBox(*sph, *this);
	}
	// vs Ray
	else if (obj->GetType() == Types::Ray)
	{
		RayColliderComponent* ray = static_cast<RayColliderComponent*>(obj);

		return CollisionFunctions::RayToBox(*ray, *this);
	}
	// vs Box
	else if (obj->GetType() == Types::Box)
	{
		BoxColliderComponent* box = static_cast<BoxColliderComponent*>(obj);

		return CollisionFunctions::BoxToBox(*this, *box);
	}
	// vs Mesh
	else if (obj->GetType() == Types::Mesh)
	{
		MeshColliderComponent* mesh = static_cast<MeshColliderComponent*>(obj);

		return CollisionFunctions::BoxToMesh(*this, *mesh);
	}

	return false;
}

void BoxColliderComponent::DebugDraw(const KdVec4& color)
{
	if (m_enable == false)return;
	if (GetOwner() == nullptr)return;

	// 線描画
	ShMgr.m_KdPrimSh.DrawLine_Box(m_localPos, m_localSize, color, &GetOwner()->GetMatrix());

	// ヒット情報
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

		ShMgr.m_KdPrimSh.DrawLine(GetOwner()->GetMatrix().GetPos(), GetOwner()->GetMatrix().GetPos() + res.Push, KdVec4(1, 1, 0, 0.5f), &KdMatrix());
	}
}

void BoxColliderComponent::Start()
{
	//--------------------------------
	// 更新
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ローカルの箱データを、行列でワールドへ変換する
		//-----------------------------------------------

		// サイズをワールド変換
		KdVec3 scale;
		scale = GetOwner()->GetMatrix().GetScale();
		m_box.Extents = m_localSize * scale;

		// 座標をワールド変換
		m_box.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// 回転のみの行列
		m_rotaMat.SetRotation(GetOwner()->GetMatrix());
		m_rotaMat.NormalizeScale();

		// 回転をクォータニオンに変換
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}
}

void BoxColliderComponent::Update()
{
	//--------------------------------
	// 無視マップ処理
	//--------------------------------
	IgnoreMapUpdate();

	// 
	if (m_enable == false)return;

	//--------------------------------
	// 無効カウンター処理
	//--------------------------------
	UpdateDisableCounter();

	//--------------------------------
	// 更新
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ローカルの箱データを、行列でワールドへ変換する
		//-----------------------------------------------

		// サイズをワールド変換
		KdVec3 scale;
		scale = GetOwner()->GetMatrix().GetScale();
		m_box.Extents = m_localSize * scale;

		// 座標をワールド変換
		m_box.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// 回転のみの行列
		m_rotaMat.SetRotation(GetOwner()->GetMatrix());
		m_rotaMat.NormalizeScale();

		// 回転をクォータニオンに変換
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}

	//--------------------------------
	// 登録
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void BoxColliderComponent::ImGuiUpdate()
{
	// 
	BaseColliderComponent::ImGuiUpdate();

	// 中心座標
	ImGui::DragFloat3("LocalPos", m_localPos, 0.01f);

	// 半径
	ImGui::DragFloat3("LocalSize", m_localSize, 0.01f, 0, 1000);

}
