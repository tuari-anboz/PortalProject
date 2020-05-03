#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// レイCollider コンポーネント
//
//==============================================
bool RayColliderComponent::HitTest(BaseColliderComponent* obj)
{
	// vs Sphere
	if (obj->GetType() == Types::Sphere) {
		SphereColliderComponent* sph = static_cast<SphereColliderComponent*>(obj);

		return CollisionFunctions::SphereToRay(*sph, *this);
	}
	// vs Ray
	else if (obj->GetType() == Types::Ray) {
		RayColliderComponent* ray = static_cast<RayColliderComponent*>(obj);

		// レイ同士はあたらない
		return false;
	}
	// vs Box
	if (obj->GetType() == Types::Box) {
		BoxColliderComponent* box = static_cast<BoxColliderComponent*>(obj);
		
		return CollisionFunctions::RayToBox(*this, *box);
	}
	// vs Mesh
	else if (obj->GetType() == Types::Mesh) {
		MeshColliderComponent* mesh = static_cast<MeshColliderComponent*>(obj);

		return CollisionFunctions::RayToMesh(*this, *mesh);
	}

	return false;
}

void RayColliderComponent::DebugDraw(const KdVec4 & color)
{
	if (m_enable == false)return;
	if (GetOwner() == nullptr)return;

	if (m_tag == "CameraRay") {
		int a = 0;
	}

	//-----------------------------------------------
	// ローカルのレイデータを、行列でワールドへ変換する
	//-----------------------------------------------
	/*
	KdVec3 rayPos = DirectX::SimpleMath::Vector3::Transform(m_localPos, trans->GetMatrix());
	KdVec3 rayDir = DirectX::SimpleMath::Vector3::TransformNormal(m_localDir, trans->GetMatrix());
	float rayLen = m_rayDir.Length();
	rayDir.Normalize();
	*/
	// 線描画
//	ShMgr.m_KdPrimSh.DrawLine(rayPos, rayPos + rayDir * rayLen, color, &KdMatrix());
	ShMgr.m_KdPrimSh.DrawLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, color, &KdMatrix());

	// ヒット情報
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

	}
}

void RayColliderComponent::Update()
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
		// ローカルのレイデータを、行列でワールドへ変換する
		//-----------------------------------------------
		m_rayPos = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());
		m_rayDir = DirectX::SimpleMath::Vector3::TransformNormal(m_localDir, GetOwner()->GetMatrix());
		m_rayLen = m_rayDir.Length();

		m_rayDir.Normalize();

	}

	//--------------------------------
	// 登録
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void RayColliderComponent::ImGuiUpdate()
{
	BaseColliderComponent::ImGuiUpdate();

	// 中心座標
	ImGui::DragFloat3("Pos", m_localPos, 0.01f);

	ImGui::DragFloat3("Dir", m_localDir, 0.01f);

}

