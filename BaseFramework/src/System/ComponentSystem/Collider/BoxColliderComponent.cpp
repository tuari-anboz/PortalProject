#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"

#include "CollisionFunctions.h"

//==============================================
//
// Box Collider�R���|�[�l���g
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

	// ���`��
	ShMgr.m_KdPrimSh.DrawLine_Box(m_localPos, m_localSize, color, &GetOwner()->GetMatrix());

	// �q�b�g���
	for (auto&& res : m_results) {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(res.Pos, 0.02f, KdVec4(1, 1, 0, 0.5f), &KdMatrix());

		ShMgr.m_KdPrimSh.DrawLine(GetOwner()->GetMatrix().GetPos(), GetOwner()->GetMatrix().GetPos() + res.Push, KdVec4(1, 1, 0, 0.5f), &KdMatrix());
	}
}

void BoxColliderComponent::Start()
{
	//--------------------------------
	// �X�V
	//--------------------------------
	if (GetOwner()) {

		//-----------------------------------------------
		// ���[�J���̔��f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------

		// �T�C�Y�����[���h�ϊ�
		KdVec3 scale;
		scale = GetOwner()->GetMatrix().GetScale();
		m_box.Extents = m_localSize * scale;

		// ���W�����[���h�ϊ�
		m_box.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// ��]�݂̂̍s��
		m_rotaMat.SetRotation(GetOwner()->GetMatrix());
		m_rotaMat.NormalizeScale();

		// ��]���N�H�[�^�j�I���ɕϊ�
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}
}

void BoxColliderComponent::Update()
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
		// ���[�J���̔��f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------

		// �T�C�Y�����[���h�ϊ�
		KdVec3 scale;
		scale = GetOwner()->GetMatrix().GetScale();
		m_box.Extents = m_localSize * scale;

		// ���W�����[���h�ϊ�
		m_box.Center = DirectX::SimpleMath::Vector3::Transform(m_localPos, GetOwner()->GetMatrix());

		// ��]�݂̂̍s��
		m_rotaMat.SetRotation(GetOwner()->GetMatrix());
		m_rotaMat.NormalizeScale();

		// ��]���N�H�[�^�j�I���ɕϊ�
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}

	//--------------------------------
	// �o�^
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void BoxColliderComponent::ImGuiUpdate()
{
	// 
	BaseColliderComponent::ImGuiUpdate();

	// ���S���W
	ImGui::DragFloat3("LocalPos", m_localPos, 0.01f);

	// ���a
	ImGui::DragFloat3("LocalSize", m_localSize, 0.01f, 0, 1000);

}
