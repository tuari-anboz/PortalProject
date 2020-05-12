#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"
#include "CollisionFunctions.h"

//==============================================
//
// Mesh Collider�R���|�[�l���g
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

		// ���b�V�����m�͓�����Ȃ�
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
		// ���݂̃��X�^���C�U�X�e�[�g���L�����Ă���
		KdStateSaver ssaver;
		ssaver.SaveRS();

		// ���C���[�t���[���`�惂�[�h
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
	// �X�V
	//--------------------------------
	if (GetOwner()) {

		// Transform�̍s����擾���Ă���
		m_matrix = GetOwner()->GetMatrix();

		// �t�s����쐬���Ă���
		m_matrix.Inverse(m_invMatrix);
	}

}

void MeshColliderComponent::Update()
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

	if (m_model == nullptr)return;

	//--------------------------------
	// �X�V
	//--------------------------------
	if (GetOwner()) {

		// Transform�̍s����擾���Ă���
		m_matrix = GetOwner()->GetMatrix();

		// �t�s����쐬���Ă���
		m_matrix.Inverse(m_invMatrix);
	}

	//--------------------------------
	// �o�^
	//--------------------------------
	ColMgr.Register(KdToSPtr(this));
}

void MeshColliderComponent::ImGuiUpdate()
{
	// 
	BaseColliderComponent::ImGuiUpdate();

	// �����蔻��Model�t�@�C��
	if (ImGuiResourceButton(u8"�����蔻�胂�f��", m_filename, { ".xed" })) {
		LoadModel(m_filename);
	}
}
