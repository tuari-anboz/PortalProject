#include "main.h"
#include "CollisionFunctions.h"

#include "../../GameManager.h"
#include "BaseColliderComponent.h"


// 球Collider vs 球Collider
bool CollisionFunctions::SphereToSphere(SphereColliderComponent & sph1, SphereColliderComponent & sph2)
{
	// 球 vs 球
	KdVec3 vDir = (KdVec3&)sph2.GetSphere().Center - (KdVec3&)sph1.GetSphere().Center;
	float dist = vDir.Length();

	// 当たってないなら終了
	if (dist >= sph1.GetSphere().Radius + sph2.GetSphere().Radius)return false;

	vDir.Normalize();

	// 球１側に結果データ追加
	if (sph1.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph2);														// 相手のコライダー
		res.Dist = (dist - sph2.GetSphere().Radius);										// 最短距離
		res.Pos = sph1.GetSphere().Center + vDir * res.Dist;								// 接触位置
		res.Push = -vDir * ((sph2.GetSphere().Radius + sph1.GetSphere().Radius) - dist);	// 反発ベクトル
		sph1.m_results.push_back(res);
	}

	// 球２側に結果データ追加
	if (sph2.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph1);														// 相手のコライダー
		res.Dist = (dist - sph1.GetSphere().Radius);										// 最短距離
		res.Pos = sph2.GetSphere().Center - vDir * res.Dist;								// 接触位置
		res.Push = vDir * ((sph2.GetSphere().Radius + sph1.GetSphere().Radius) - dist);		// 反発ベクトル
		sph2.m_results.push_back(res);
	}

	return true;
}

// 球Collider vs レイCollider
bool CollisionFunctions::SphereToRay(SphereColliderComponent & sph, RayColliderComponent & ray)
{
	// レイが当たった距離
	float dist;

	// 判定
	if (sph.GetSphere().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist) == false) {
		return false;
	}

	// 長さチェック
	if (dist > ray.GetRayLen())return false;

	// 球側に結果データ追加
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;		// 接触位置
		sph.m_results.push_back(res);
	}

	// レイ側に結果データ追加
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;
		ray.m_results.push_back(res);
	}
	return true;
}

// 球Collider vs 箱Collider
bool CollisionFunctions::SphereToBox(SphereColliderComponent & sph, BoxColliderComponent & box)
{
	// 球と箱の判定
	KdVec3 vNearPt;
	KdCollision::PointToBox(sph.GetSphere().Center,
		(KdVec3&)box.GetBox().Center,
		(KdVec3&)box.GetBox().Extents,
		box.GetRotaMat(),
		vNearPt
	);

	KdVec3 vD = vNearPt - sph.GetSphere().Center;

	float len = vD.Length();
	// 当たっていないなら終了
	if (len >= sph.GetSphere().Radius)return false;

	vD.Normalize();

	// 球側に結果データ追加
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);
		res.Dist = len;
		res.Pos = vNearPt;
		res.Push = -vD * (sph.GetSphere().Radius - len);
		sph.m_results.push_back(res);
	}

	// 箱側に結果データ追加
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

// 球Collider vs メッシュCollider
bool CollisionFunctions::SphereToMesh(SphereColliderComponent & sph, MeshColliderComponent & mesh)
{
	// 球とメッシュの判定
	bool bHit = false;

	KdVec3 vOut;

	// 全てのメッシュと判定
	for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {

		// メッシュの参照
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
		// 当たってないなら終了
		return false;
	}

	// 球側に結果データ追加
	if (sph.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);		// 相手
		res.Pos = sph.GetSphere().Center;	// 今回、ヒット位置は適当です。
		res.Push = vOut;					// 反発ベクトル
		sph.m_results.push_back(res);
	}

	// メッシュ側に結果データ追加
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&sph);
		res.Pos = mesh.GetMatrix().GetPos();
		res.Push = -vOut;
		mesh.m_results.push_back(res);
	}

	return true;
}

// レイCollider vs 箱Collider
bool CollisionFunctions::RayToBox(RayColliderComponent & ray, BoxColliderComponent & box)
{
	// レイ vs 箱
	float dist;
	if (!box.GetBox().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist))return false;

	if (dist < 0 || dist > ray.GetRayLen())return false;

	// レイ側に結果データ追加
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);						// 相手
		res.Dist = dist;									// 距離
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;	// 位置
		ray.m_results.push_back(res);
	}

	// 箱側に結果データ追加
	if (box.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = dist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * dist;
		box.m_results.push_back(res);
	}

	return true;
}

// レイCollider vs メッシュCollider
bool CollisionFunctions::RayToMesh(RayColliderComponent & ray, MeshColliderComponent & mesh)
{

	// ヒット情報
	float nearestDist = FLT_MAX;
	UINT hitMeshIndex;
	UINT hitFaceIndex;
	bool bHit = false;

	{
		// レイ情報をメッシュ側への空間へ変換
		KdVec3 rayLocalPos = ray.GetRayPos();
		KdVec3 rayLocalDir = ray.GetRayDir();
		KdVec3 rayLocalPosEnd = rayLocalPos + rayLocalDir * ray.GetRayLen();
		rayLocalPos.Transform(mesh.GetInvMatrix());			// レイ開始位置
		rayLocalPosEnd.Transform(mesh.GetInvMatrix());		// レイ終了位置
		rayLocalDir.TransformNormal(mesh.GetInvMatrix());	// レイ方向
		float length = (rayLocalPosEnd - rayLocalPos).Length();	//変換後のレイの長さ

		// 全てのメッシュと判定
		for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {
			// メッシュ
			KdMesh* kdm = mesh.GetModel()->GetModels()[mi]->GetMesh().get();

			float dist;
			UINT faceIdx;

			if (KdCollision::RayToMesh(*kdm, nullptr, rayLocalPos, rayLocalDir, length, dist, faceIdx)) {
				// ヒット　前回ヒットした距離より近いなら、そいつを記憶
				if (dist < nearestDist) {
					nearestDist = dist;			// レイのヒット距離

					hitMeshIndex = mi;			// メッシュ番号
					hitFaceIndex = faceIdx;		// 面番号
					bHit = true;
				}

			}

		}
	}

	// なんにも当たってないなら終了
	if (bHit == false)return false;

	// レイの長さより短いなら、ヒットしていないとする
	if (nearestDist > ray.GetRayLen())return false;

	// レイ側に結果データ追加
	if (ray.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);								// 相手
		res.Dist = nearestDist;										// 距離
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * nearestDist;	// ヒット位置
		res.MeshIdx = hitMeshIndex;									// メッシュのインデックス
		res.FaceIdx = hitFaceIndex;									// 面のインデックス
		ray.m_results.push_back(res);
	}

	// メッシュに結果データ追加
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&ray);
		res.Dist = nearestDist;
		res.Pos = ray.GetRayPos() + ray.GetRayDir() * nearestDist;
		mesh.m_results.push_back(res);
	}

	return true;
}

// 箱Collider vs 箱Collider
bool CollisionFunctions::BoxToBox(BoxColliderComponent & box1, BoxColliderComponent & box2)
{
	// 判定　※この関数では、正確なヒット位置を求めることは出来ない
	if (!box1.GetBox().Intersects(box2.GetBox()))return false;

	// 箱１側に結果データ追加
	if (box1.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box2);
		res.Pos = box2.GetBox().Center;
		box1.m_results.push_back(res);
	}

	// 箱２側に結果データ追加
	if (box2.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box1);
		res.Pos = box1.GetBox().Center;
		box2.m_results.push_back(res);
	}

	return true;
}

// 箱Collider vs メッシュCollider
bool CollisionFunctions::BoxToMesh(BoxColliderComponent & box, MeshColliderComponent & mesh)
{
	bool bHit = false;
	UINT meshIdx = 0;
	KdVec3 pushvec;
	int hitcnt = 0;

	// 箱をメッシュ空間へ変換
	DirectX::BoundingOrientedBox obb;
	box.GetBox().Transform(obb, mesh.GetInvMatrix());

	// 全てのメッシュと判定
	for (UINT mi = 0; mi < mesh.GetModel()->GetModels().size(); mi++) {
		// メッシュ
		auto& kdMesh = mesh.GetModel()->GetModels()[mi]->GetMesh();

		// 面数
		const UINT faceNum = kdMesh->GetFaceArray().size();

		// 全ての面と判定
		for (UINT fi = 0; fi < faceNum; fi++) {

			// 面のインデックス情報
			const UINT* idx = &kdMesh->GetFaceArray()[fi].idx[0];

			//// OBB vs Plane
			//// 平面の法線に対するOBBの射影線の長さを算出
			//float r = 0.0f;          // 近接距離
			//KdVec3 PlaneNormal; // 平面の法線ベクトル
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

			//// 平面とOBBの距離を算出
			//KdVec3 ObbPos = obb.Center;
			//KdVec3 PlanePos = kdMesh->GetVertexPosArray()[fi];
			//KdVec3 vec = ObbPos - PlanePos;
			//float s = vec.Dot(PlaneNormal);

			//float Len = 0.0f;

			//// 衝突判定
			//if (fabs(s) - r < 0.0f)
			//{
			//	meshIdx = mi;
			//	bHit = true;
			//	hitcnt++;

			//	// 戻し距離を算出
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
			// 箱 vs メッシュ判定　※この関数では、正確なヒット位置を求めることは出来ない
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



	// 箱側に結果データ追加
	if (box.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&mesh);		// 相手
		res.MeshIdx = meshIdx;				// メッシュのインデックス
		//res.Push = pushvec / hitcnt;		// 押し出しベクトル
		box.m_results.push_back(res);
	}

	// メッシュ側に結果データ追加
	if (mesh.IsAtk()) {
		CollisionResult res;
		res.Collider = KdToSPtr(&box);
		mesh.m_results.push_back(res);
	}
	return true;
}
