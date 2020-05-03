#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// ���CCollider �R���|�[�l���g
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

		// ���C���m�͂�����Ȃ�
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
	// ���[�J���̃��C�f�[�^���A�s��Ń��[���h�֕ϊ�����
	//-----------------------------------------------
	/*
	KdVec3 rayPos = DirectX::SimpleMath::Vector3::Transform(m_localPos, trans->GetMatrix());
	KdVec3 rayDir = DirectX::SimpleMath::Vector3::TransformNormal(m_localDir, trans->GetMatrix());
	float rayLen = m_rayDir.Length();
	rayDir.Normalize();
	*/
	// ���`��
//	ShMgr.m_KdPrimSh.DrawLine(rayPos, rayPos + rayDir * rayLen, color, &KdMatrix());
	ShMgr.m_KdPrimSh.DrawLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, color, &KdMatrix());

	// �q�b�g���
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

	}
}

void RayColliderComponent::Update()
{
	//--------------------------------
	// �����}�b�v����
	//--------------------------------
	IgnoreMapUpdate();

	//
	if (m_enable == false)return;

	//--------------------------------
	// �����J�E���^�[����
	//--------------------------------
	UpdateDisableCounter();

	//--------------------------------
	// �X�V
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ���[�J���̃��C�f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------
		m_rayPos = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());
		m_rayDir = DirectX::SimpleMath::Vector3::TransformNormal(m_localDir, GetOwner()->GetMatrix());
		m_rayLen = m_rayDir.Length();

		m_rayDir.Normalize();

	}

	//--------------------------------
	// �o�^
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void RayColliderComponent::ImGuiUpdate()
{
	BaseColliderComponent::ImGuiUpdate();

	// ���S���W
	ImGui::DragFloat3("Pos", m_localPos, 0.01f);

	ImGui::DragFloat3("Dir", m_localDir, 0.01f);

}

