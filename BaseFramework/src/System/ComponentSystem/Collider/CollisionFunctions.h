#pragma once

class SphereColliderComponent;
class BoxColliderComponent;
class RayColliderComponent;
class MeshColliderComponent;

//==============================================================
//
// �R���C�_�[���m�̏Փ˔���̏������܂Ƃ߂��N���X
//
//  �R���C�_�[���m���Փ˔�����s���AAtk�����̃R���C�_�[�ɑ΂��Č��ʃf�[�^������B
//
//==============================================================
class CollisionFunctions {
public:

	// ��Collider vs ��Collider
	static bool SphereToSphere(SphereColliderComponent& sph1, SphereColliderComponent& sph2);

	// ��Collider vs ���CCollider
	static bool SphereToRay(SphereColliderComponent& sph, RayColliderComponent& ray);

	// ��Collider vs ��Collider
	static bool SphereToBox(SphereColliderComponent& sph, BoxColliderComponent& box);

	// ��Collider vs ���b�V��Collider
	static bool SphereToMesh(SphereColliderComponent& sph, MeshColliderComponent& mesh);

	// ���CCollider vs ���CCollider

	// ���CCollider vs ��Collider
	static bool RayToBox(RayColliderComponent& ray, BoxColliderComponent& box);

	// ���CCollider vs ���b�V��Collider
	static bool RayToMesh(RayColliderComponent& ray, MeshColliderComponent& mesh);


	// ��Collider vs ��Collider
	static bool BoxToBox(BoxColliderComponent& box1, BoxColliderComponent& box2);

	// ��Collider vs ���b�V��Collider
	static bool BoxToMesh(BoxColliderComponent& box, MeshColliderComponent& mesh);

	// ���b�V��Collider vs ���b�V��Collider

};
