#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// Sphere Collider�R���|�[�l���g
//
//==============================================

// �����蔻����s
// ������͉ǐ����d������if�Ŕ��肵�܂����Ă܂����A���ۂɂ�
// �@Visitor�p�^�[�����g����if��S���������Ƃ��o���ėǂ��ł��B
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

// �f�o�b�O�`��
void SphereColliderComponent::DebugDraw(const KdVec4& color)
{
	if (m_enable == false)return;
	if (GetOwner() == nullptr)return;

	// ���`��
	DirectX::BoundingSphere bs;
	bs.Center = m_localPos;
	bs.Radius = m_radius;
	//-----------------------------------------------
	// ���[�J���̋��f�[�^���A�s��Ń��[���h�֕ϊ�����
	//-----------------------------------------------
	if (GetOwner()) {

		// ���[�J�����W�����[���h�ϊ�
		bs.Center = DirectX::SimpleMath::Vector3::Transform(bs.Center, GetOwner()->GetMatrix());

		// ���a�����[���h�ϊ�
		// �ȉ~�͓���̂ŁA���a��XYZ���̈�ԑ傫�����̂��g�p
		KdVec3 scale = GetOwner()->GetMatrix().GetScale();	// XYZ���̊g�嗦
		float maxScale = std::max(scale.x, scale.y);
		maxScale = std::max(maxScale, scale.z);
		bs.Radius = bs.Radius * maxScale;
	}
	ShMgr.m_KdPrimSh.DrawLine_Sphere(bs.Center, bs.Radius, color, &KdMatrix());

	// �q�b�g���
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, color, &KdMatrix());

		ShMgr.m_KdPrimSh.DrawLine(res.Pos, res.Pos + res.Push, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

	}
}

// �X�V����
void SphereColliderComponent::Update()
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
	// ����f�[�^�X�V
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ���[�J���̋��f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------

		// ���[�J�����W�����[���h�ϊ�
		m_sphere.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// ���a�����[���h�ϊ�
		// �ȉ~�͓���̂ŁA���a��XYZ���̈�ԑ傫�����̂��g�p
		KdVec3 scale = GetOwner()->GetMatrix().GetScale();	// XYZ���̊g�嗦
		float maxScale = std::max(scale.x, scale.y);
		maxScale = std::max(maxScale, scale.z);
		m_sphere.Radius = m_radius * maxScale;
	}

	//--------------------------------
	// �o�^
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void SphereColliderComponent::ImGuiUpdate()
{
	BaseColliderComponent::ImGuiUpdate();

	// ���S���W
	ImGui::DragFloat3("CenterPos", m_localPos, 0.01f);

	// ���a
	ImGui::DragFloat("Radius", &m_radius, 0.01f, 0, 1000);

}
