#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// Sphere Colliderコンポーネント
//
//==============================================

// あたり判定実行
// ※今回は可読性を重視してifで判定しまくってますが、実際には
// 　Visitorパターンを使うとifを全部消すことが出来て良いです。
bool SphereColliderComponent::HitTest(BaseColliderComponent * obj)
{
	// vs Sphere
	if (obj->GetType() == Types::Sphere) {
		SphereColliderComponent* sph = static_cast<SphereColliderComponent*>(obj);

		return CollisionFunctions::SphereToSphere(*this, *sph);
	}
	// vs Ray
	else if (obj->GetType() == Types::Ray) {
		RayColliderComponent* ray = static_cast<RayColliderComponent*>(obj);

		return CollisionFunctions::SphereToRay(*this, *ray);
	}
	// vs Box
	if (obj->GetType() == Types::Box) {
		BoxColliderComponent* box = static_cast<BoxColliderComponent*>(obj);

		return CollisionFunctions::SphereToBox(*this, *box);
	}
	// vs Mesh
	else if (obj->GetType() == Types::Mesh) {
		MeshColliderComponent* mesh = static_cast<MeshColliderComponent*>(obj);

		return CollisionFunctions::SphereToMesh(*this, *mesh);
	}

	return false;
}

// デバッグ描画
void SphereColliderComponent::DebugDraw(const KdVec4& color)
{
	if (m_enable == false)return;
	if (GetOwner() == nullptr)return;

	// 線描画
	DirectX::BoundingSphere bs;
	bs.Center = m_localPos;
	bs.Radius = m_radius;
	//-----------------------------------------------
	// ローカルの球データを、行列でワールドへ変換する
	//-----------------------------------------------
	if (GetOwner()) {

		// ローカル座標をワールド変換
		bs.Center = DirectX::SimpleMath::Vector3::Transform(bs.Center, GetOwner()->GetMatrix());

		// 半径をワールド変換
		// 楕円は難しいので、半径はXYZ軸の一番大きいものを使用
		KdVec3 scale = GetOwner()->GetMatrix().GetScale();	// XYZ軸の拡大率
		float maxScale = std::max(scale.x, scale.y);
		maxScale = std::max(maxScale, scale.z);
		bs.Radius = bs.Radius * maxScale;
	}
	ShMgr.m_KdPrimSh.DrawLine_Sphere(bs.Center, bs.Radius, color, &KdMatrix());

	// ヒット情報
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, color, &KdMatrix());

		ShMgr.m_KdPrimSh.DrawLine(res.Pos, res.Pos + res.Push, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

	}
}

// 更新処理
void SphereColliderComponent::Update()
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
	// 判定データ更新
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ローカルの球データを、行列でワールドへ変換する
		//-----------------------------------------------

		// ローカル座標をワールド変換
		m_sphere.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// 半径をワールド変換
		// 楕円は難しいので、半径はXYZ軸の一番大きいものを使用
		KdVec3 scale = GetOwner()->GetMatrix().GetScale();	// XYZ軸の拡大率
		float maxScale = std::max(scale.x, scale.y);
		maxScale = std::max(maxScale, scale.z);
		m_sphere.Radius = m_radius * maxScale;
	}

	//--------------------------------
	// 登録
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void SphereColliderComponent::ImGuiUpdate()
{
	BaseColliderComponent::ImGuiUpdate();

	// 中心座標
	ImGui::DragFloat3("CenterPos", m_localPos, 0.01f);

	// 半径
	ImGui::DragFloat("Radius", &m_radius, 0.01f, 0, 1000);

}
