#include "main.h"
#include "CollisionFunctions.h"

#include "../../GameManager.h"
#include "BaseColliderComponent.h"


// ��Collider vs ��Collider
bool CollisionFunctions::SphereToSphere(SphereColliderComponent & sph1, SphereColliderComponent & sph2)
{
	// �� vs ��
	KdVec3 vDir = (KdVec3&)sph2.GetSphere().Center - (KdVec3&)sph1.GetSphere().Center;
	float dist = vDir.Length();

	// �������ĂȂ��Ȃ�I��
	if (dist >= sph1.GetSphere().Radius + sph2.GetSphere().Radius)return false;

	vDir.Normalize();

	// ���P���Ɍ��ʃf�[�^�ǉ�
	if (sph1.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph2);														// ����̃R���C�_�[
		res.Dist = (dist - sph2.GetSphere().Radius);										// �ŒZ����
		res.Pos = sph1.GetSphere().Center + vDir * res.Dist;								// �ڐG�ʒu
		res.Push = -vDir * ((sph2.GetSphere().Radius + sph1.GetSphere().Radius) - dist);	// �����x�N�g��
		sph1.m_results.push_back(res);
	}

	// ���Q���Ɍ��ʃf�[�^�ǉ�
	if (sph2.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph1);														// ����̃R���C�_�[
		res.Dist = (dist - sph1.GetSphere().Radius);										// �ŒZ����
		res.Pos = sph2.GetSphere().Center - vDir * res.Dist;								// �ڐG�ʒu
		res.Push = vDir * ((sph2.GetSphere().Radius + sph1.GetSphere().Radius) - dist);		// �����x�N�g��
		sph2.m_results.push_back(res);
	}

	return true;
}

// ��Collider vs ���CCollider
bool CollisionFunctions::SphereToRay(SphereColliderComponent & sph, RayColliderComponent & ray)
{
	// ���C��������������
	float dist;

	// ����
	if (sph.GetSphere().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist) == false) {
		return false;
	}

	// �����`�F�b�N
	if (dist > ray.GetRayLen())return false;

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;		// �ڐG�ʒu
		sph.m_results.push_back(res);
	}

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;
		ray.m_results.push_back(res);
	}
	return true;
}

// ��Collider vs ��Collider
bool CollisionFunctions::SphereToBox(SphereColliderComponent & sph, BoxColliderComponent & box)
{
	// ���Ɣ��̔���
	KdVec3 vNearPt;
	KdCollision::PointToBox(sph.GetSphere().Center,
		(KdVec3&)box.GetBox().Center,
		(KdVec3&)box.GetBox().Extents,
		box.GetRotaMat(),
		vNearPt
	);

	KdVec3 vD = vNearPt - sph.GetSphere().Center;

	float len = vD.Length();
	// �������Ă��Ȃ��Ȃ�I��
	if (len >= sph.GetSphere().Radius)return false;

	vD.Normalize();

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);
		res.Dist = len;
		res.Pos = vNearPt;
		res.Push = -vD * (sph.GetSphere().Radius - len);
		sph.m_results.push_back(res);
	}

	// �����Ɍ��ʃf�[�^�ǉ�
	if (box.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph);
		res.Dist = len;
		res.Pos = vNearPt;
		res.Push = vD * (sph.GetSphere().Radius - len);
		box.m_results.push_back(res);
	}

	return true;
}

// ��Collider vs ���b�V��Collider
bool CollisionFunctions::SphereToMesh(SphereColliderComponent & sph, MeshColliderComponent & mesh)
{
	// ���ƃ��b�V���̔���
	bool bHit = false;

	KdVec3 vOut;

	// �S�Ẵ��b�V���Ɣ���
	for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {

		// ���b�V���̎Q��
		auto& kdMesh = mesh.GetModel()->GetModels()[mi]->GetMesh();

		//			std::list<KdCollision::MeshHitInfo> detail;
		KdVec3 v;
		if (KdCollision::SphereToMesh(*kdMesh,
			&mesh.GetMatrix(),
			(KdVec3&)sph.GetSphere().Center,
			sph.GetSphere().Radius,
			v, 0, -1,
			nullptr)
			) {

			bHit = true;
			vOut += v;

		}
	}

	if (bHit == false) {
		// �������ĂȂ��Ȃ�I��
		return false;
	}

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);		// ����
		res.Pos = sph.GetSphere().Center;	// ����A�q�b�g�ʒu�͓K���ł��B
		res.Push = vOut;					// �����x�N�g��
		sph.m_results.push_back(res);
	}

	// ���b�V�����Ɍ��ʃf�[�^�ǉ�
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph);
		res.Pos = mesh.GetMatrix().GetPos();
		res.Push = -vOut;
		mesh.m_results.push_back(res);
	}

	return true;
}

// ���CCollider vs ��Collider
bool CollisionFunctions::RayToBox(RayColliderComponent & ray, BoxColliderComponent & box)
{
	// ���C vs ��
	float dist;
	if (!box.GetBox().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist))return false;

	if (dist < 0 || dist > ray.GetRayLen())return false;

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);						// ����
		res.Dist = dist;									// ����
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;	// �ʒu
		ray.m_results.push_back(res);
	}

	// �����Ɍ��ʃf�[�^�ǉ�
	if (box.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;
		box.m_results.push_back(res);
	}

	return true;
}

// ���CCollider vs ���b�V��Collider
bool CollisionFunctions::RayToMesh(RayColliderComponent & ray, MeshColliderComponent & mesh)
{

	// �q�b�g���
	float nearestDist = FLT_MAX;
	UINT hitMeshIndex;
	UINT hitFaceIndex;
	bool bHit = false;

	{
		// ���C�������b�V�����ւ̋�Ԃ֕ϊ�
		KdVec3 rayLocalPos = ray.GetRayPos();
		KdVec3 rayLocalDir = ray.GetRayDir();
		KdVec3 rayLocalPosEnd = rayLocalPos + rayLocalDir * ray.GetRayLen();
		rayLocalPos.Transform(mesh.GetInvMatrix());			// ���C�J�n�ʒu
		rayLocalPosEnd.Transform(mesh.GetInvMatrix());		// ���C�I���ʒu
		rayLocalDir.TransformNormal(mesh.GetInvMatrix());	// ���C����
		float length = (rayLocalPosEnd - rayLocalPos).Length();	//�ϊ���̃��C�̒���

		// �S�Ẵ��b�V���Ɣ���
		for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {
			// ���b�V��
			KdMesh* kdm = mesh.GetModel()->GetModels()[mi]->GetMesh().get();

			float dist;
			UINT faceIdx;

			if (KdCollision::RayToMesh(*kdm, nullptr, rayLocalPos, rayLocalDir, length, dist, faceIdx)) {
				// �q�b�g�@�O��q�b�g�����������߂��Ȃ�A�������L��
				if (dist < nearestDist) {
					nearestDist = dist;			// ���C�̃q�b�g����

					hitMeshIndex = mi;			// ���b�V���ԍ�
					hitFaceIndex = faceIdx;		// �ʔԍ�
					bHit = true;
				}

			}

		}
	}

	// �Ȃ�ɂ��������ĂȂ��Ȃ�I��
	if (bHit == false)return false;

	// ���C�̒������Z���Ȃ�A�q�b�g���Ă��Ȃ��Ƃ���
	if (nearestDist > ray.GetRayLen())return false;

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);								// ����
		res.Dist = nearestDist;										// ����
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * nearestDist;	// �q�b�g�ʒu
		res.MeshIdx = hitMeshIndex;									// ���b�V���̃C���f�b�N�X
		res.FaceIdx = hitFaceIndex;									// �ʂ̃C���f�b�N�X
		ray.m_results.push_back(res);
	}

	// ���b�V���Ɍ��ʃf�[�^�ǉ�
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = nearestDist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * nearestDist;
		mesh.m_results.push_back(res);
	}

	return true;
}

// ��Collider vs ��Collider
bool CollisionFunctions::BoxToBox(BoxColliderComponent & box1, BoxColliderComponent & box2)
{
	// ����@�����̊֐��ł́A���m�ȃq�b�g�ʒu�����߂邱�Ƃ͏o���Ȃ�
	if (!box1.GetBox().Intersects(box2.GetBox()))return false;

	// ���P���Ɍ��ʃf�[�^�ǉ�
	if (box1.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box2);
		res.Pos = box2.GetBox().Center;
		box1.m_results.push_back(res);
	}

	// ���Q���Ɍ��ʃf�[�^�ǉ�
	if (box2.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box1);
		res.Pos = box1.GetBox().Center;
		box2.m_results.push_back(res);
	}

	return true;
}

// ��Collider vs ���b�V��Collider
bool CollisionFunctions::BoxToMesh(BoxColliderComponent & box, MeshColliderComponent & mesh)
{
	bool bHit = false;
	UINT meshIdx = 0;
	KdVec3 pushvec;
	int hitcnt = 0;

	// �������b�V����Ԃ֕ϊ�
	DirectX::BoundingOrientedBox obb;
	box.GetBox().Transform(obb, mesh.GetInvMatrix());

	// �S�Ẵ��b�V���Ɣ���
	for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {
		// ���b�V��
		auto& kdMesh = mesh.GetModel()->GetModels()[mi]->GetMesh();

		// �ʐ�
		const UINT faceNum = kdMesh->GetFaceArray().size();

		// �S�Ă̖ʂƔ���
		for (UINT fi = 0; fi < faceNum; fi++) {

			// �ʂ̃C���f�b�N�X���
			const UINT* idx = &kdMesh->GetFaceArray()[fi].idx[0];

			//// OBB vs Plane
			//// ���ʂ̖@���ɑ΂���OBB�̎ˉe���̒������Z�o
			//float r = 0.0f;          // �ߐڋ���
			//KdVec3 PlaneNormal; // ���ʂ̖@���x�N�g��
			//PlaneNormal = kdMesh->GetExtFaceArray()[fi].vN;
			//PlaneNormal.Normalize();

			//float dirx = obb.Extents.x;
			//float diry = obb.Extents.y;
			//float dirz = obb.Extents.z;
			//KdVec3 Direct = { dirx,0,0 };
			//r += fabs(Direct.Dot(PlaneNormal));
			//Direct = { 0,diry,0 };
			//r += fabs(Direct.Dot(PlaneNormal));
			//Direct = { 0,0,dirz };
			//r += fabs(Direct.Dot(PlaneNormal));

			//// ���ʂ�OBB�̋������Z�o
			//KdVec3 ObbPos = obb.Center;
			//KdVec3 PlanePos = kdMesh->GetVertexPosArray()[fi];
			//KdVec3 vec = ObbPos - PlanePos;
			//float s = vec.Dot(PlaneNormal);

			//float Len = 0.0f;

			//// �Փ˔���
			//if (fabs(s) - r < 0.0f)
			//{
			//	meshIdx = mi;
			//	bHit = true;
			//	hitcnt++;

			//	// �߂��������Z�o
			//	if (s > 0)
			//	{
			//		Len = r - fabs(s);
			//	}
			//	else
			//	{
			//		Len = r + fabs(s);
			//	}

			//	PlaneNormal.Normalize();
			//	pushvec += PlaneNormal * Len;
			//}
			// �� vs ���b�V������@�����̊֐��ł́A���m�ȃq�b�g�ʒu�����߂邱�Ƃ͏o���Ȃ�
			if (obb.Intersects(	kdMesh->GetVertexPosArray()[idx[0]],
								kdMesh->GetVertexPosArray()[idx[1]],
								kdMesh->GetVertexPosArray()[idx[2]])
			) {
				meshIdx = mi;
				bHit = true;
				break;
			}
		}
		if (bHit == false)break;
	}

	if (bHit == false)return false;



	// �����Ɍ��ʃf�[�^�ǉ�
	if (box.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);		// ����
		res.MeshIdx = meshIdx;				// ���b�V���̃C���f�b�N�X
		//res.Push = pushvec / hitcnt;		// �����o���x�N�g��
		box.m_results.push_back(res);
	}

	// ���b�V�����Ɍ��ʃf�[�^�ǉ�
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);
		mesh.m_results.push_back(res);
	}
	return true;
}
