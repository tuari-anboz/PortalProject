#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// Mesh Colliderコンポーネント
//
//==============================================

bool MeshColliderComponent::HitTest(BaseColliderComponent * obj)
{
	// vs Sphere
	if (obj->GetType() == Types::Sphere) {
		SphereColliderComponent* sph = static_cast<SphereColliderComponent*>(obj);

		return CollisionFunctions::SphereToMesh(*sph, *this);
	}
	// vs Ray
	else if (obj->GetType() == Types::Ray) {
		RayColliderComponent* ray = static_cast<RayColliderComponent*>(obj);

		return CollisionFunctions::RayToMesh(*ray, *this);
	}
	// vs Box
	if (obj->GetType() == Types::Box) {
		BoxColliderComponent* box = static_cast<BoxColliderComponent*>(obj);

		return CollisionFunctions::BoxToMesh(*box, *this);
	}
	// vs Mesh
	else if (obj->GetType() == Types::Mesh) {
		MeshColliderComponent* mesh = static_cast<MeshColliderComponent*>(obj);

		// メッシュ同士は当たらない
		return false;
	}

	return false;
}

void MeshColliderComponent::DebugDraw(const KdVec4 & color)
{
	if (m_enable == false)return;
	if (m_model == nullptr)return;
	if (GetOwner() == nullptr)return;

	{
		// 現在のラスタライザステートを記憶しておく
		KdStateSaver ssaver;
		ssaver.SaveRS();

		// ワイヤーフレーム描画モード
		KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_WireframeBoth);

		ShMgr.m_KdPrimSh.SetTexture(nullptr);
		for (auto& model : m_model->GetModels()) {
			ShMgr.m_KdPrimSh.DrawMesh(*model->GetMesh(), color, &GetOwner()->GetMatrix());
		}
	}

}

void MeshColliderComponent::LoadModel(const std::string& filename)
{
	m_filename = filename;
	auto model = KDResFactory.GetGameModel(m_filename);
	if (model)
	{
		m_model = model;
	}

}

void MeshColliderComponent::Start()
{
	//--------------------------------
	// 更新
	//--------------------------------
	if (GetOwner()) {

		// Transformの行列を取得しておく
		m_matrix = GetOwner()->GetMatrix();

		// 逆行列も作成しておく
		m_matrix.Inverse(m_invMatrix);
	}

}

void MeshColliderComponent::Update()
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

	if (m_model == nullptr)return;

	//--------------------------------
	// 更新
	//--------------------------------
	if (GetOwner()) {

		// Transformの行列を取得しておく
		m_matrix = GetOwner()->GetMatrix();

		// 逆行列も作成しておく
		m_matrix.Inverse(m_invMatrix);
	}

	//--------------------------------
	// 登録
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void MeshColliderComponent::ImGuiUpdate()
{
	// 
	BaseColliderComponent::ImGuiUpdate();

	// 当たり判定Modelファイル
	if (ImGuiResourceButton(u8"あたり判定モデル", m_filename, { ".xed" })) {
		LoadModel(m_filename);
	}
}
