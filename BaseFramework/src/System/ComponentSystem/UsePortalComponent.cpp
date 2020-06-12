#include "main.h"

#include "../Gamemanager.h"

#include "UsePortalComponent.h"

void UsePortalComponent::Update()
{
	if (m_enable == false)return;

	// ポータルコンポーネントを持っているオーナー取得
	auto owner = GetOwner();
	if (!owner) return;

	// オーナーの球コライダーコンポーネントをすべて取得
	auto colliders = owner->GetComponents<SphereColliderComponent>();
	for (auto&& coll : colliders)
	{
		if (coll->GetTag() == "Portal")
		{
			// ヒット時の実行される関数を登録
			coll->m_onHitStay = [this, coll](BaseColliderComponent* collider)
			{
				// ヒットした奴ら(Collider)全員
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() == "Portal")
					{
						if (res.Collider->GetOwner()->GetName() == "Portal1")
						{
							// 反発ベクトル
							KdVec3 vPush = res.Push;
							// ポータルの法線ベクトルを取得
							KdMatrix portalMat = res.Collider->GetOwner()->GetMatrix();
							KdVec3 vZ = portalMat.GetZAxis();
							// 法線ベクトルと反発ベクトルの内積
							float dot = vZ.Dot(vPush);
							dot = std::min(1.0f, dot);
							dot = std::max(-1.0f, dot);
							// 0より小さいならポータルをくぐった
							if (dot < 0)
							{
								// もう片方のポータルにワープさせる
								KdMatrix ownerMat = GetOwner()->GetMatrix();
								KdMatrix m = ownerMat;
								auto otherPortal = GameMgr.m_level->GetRoot()->FindObject("Portal2");
								if (otherPortal == nullptr)return;
								KdMatrix mOtherPortal = otherPortal->GetMatrix();
								KdMatrix mInvPortal;
								portalMat.Inverse(mInvPortal);
								KdMatrix mDelta = mInvPortal * mOtherPortal;
								// 追加部分 /////////////////////////////////
								m = mOtherPortal;
								GetOwner()->SetMatrix(m, false);
								m = ownerMat;
								/////////////////////////////////////////////
								m *= mDelta;

								KdVec3 vY = mOtherPortal.GetYAxis();
								KdVec3 vec = m.GetPos() - (mOtherPortal.GetPos()) + coll->GetLocalPos();
								m.SetPos(vec);
								m.RotateAxis(vY, 180);
								vec = m.GetPos() + ((mOtherPortal.GetPos()) - coll->GetLocalPos());
								m.SetPos(vec);
								//GetOwner()->SetMatrix(m, false);

								// カメラの角度も変更
								auto cam = GetOwner()->FindObject("Cam");
								if (cam)
								{
									auto fpscomp = cam->GetComponent<FpsCameraComponent>();
									if (fpscomp)
									{
										KdMatrix mAfterCam = cam->GetMatrix();
										KdMatrix mInvCam;
										GetOwner()->GetPrevMatrix().Inverse(mInvCam);
										KdMatrix mDelta = mInvCam * m;
										mAfterCam *= mDelta;

										KdVec3 aftervZ = mAfterCam.GetZAxis();
										aftervZ.y = 0;
										aftervZ.Normalize();
										float ownerdot = aftervZ.Dot(KdVec3(0, 0, 1));
										ownerdot = std::min(1.0f, ownerdot);
										ownerdot = std::max(-1.0f, ownerdot);
										ownerdot = DirectX::XMConvertToDegrees(acos(ownerdot));
										if (aftervZ.x < 0.0f) ownerdot = -ownerdot;
										fpscomp->GetAngY() = ownerdot;

										KdMatrix mMoveCam = mAfterCam;
										mMoveCam.SetPos(KdVec3(0, 0, 0));
										//mMoveCam.RotateAxis(vY, 180);
										mMoveCam.SetPos(mAfterCam.GetPos());

										aftervZ = mMoveCam.GetZAxis();
										aftervZ.Normalize();
										KdVec3 vHeightZero = aftervZ;
										vHeightZero.y = 0;
										vHeightZero.Normalize();

										ownerdot = aftervZ.Dot(vHeightZero);
										ownerdot = std::min(1.0f, ownerdot);
										ownerdot = std::max(-1.0f, ownerdot);
										ownerdot = DirectX::XMConvertToDegrees(acos(ownerdot));
										if (aftervZ.y > 0.0f) ownerdot = -ownerdot;
										fpscomp->GetAngX() = ownerdot;

										KdMatrix mCam = cam->GetMatrix();
										KdMatrix mRX, mRY, mR;
										mRX.RotateX(fpscomp->GetAngX());
										mRY.RotateY(fpscomp->GetAngY());
										mR = mRX * mRY;
										mCam.SetRotation(mR);
										mCam.LookAtDir(mCam.GetZAxis());
										cam->SetMatrix(mCam, false);
									}
								}

								// 剛体の移動を制御
								auto rigidbodyComp = GetOwner()->GetComponent<RigidBodyComponent>();
								if (rigidbodyComp)
								{
									KdVec3 velocity;
									auto rigidbody = rigidbodyComp->GetRigidBody();
									rigidbody->GetLinearVelocity(velocity);
									rigidbody->SetLinearVelocity(KdVec3(0, 0, 0));
									rigidbody->SetMatrix(m);

									KdMatrix mTmp;
									mTmp.SetPos(velocity + portalMat.GetPos());
									mTmp *= mDelta;
									KdVec3 vTmp;
									vTmp = mTmp.GetPos() - mOtherPortal.GetPos();
									mTmp.SetPos(vTmp);
									mTmp.RotateAxis(vY, 180);
									KdVec3 vTransVelocity = mTmp.GetPos();
									rigidbody->SetLinearVelocity(vTransVelocity);
								}

								//// ワープした後の移動ベクトルを変換する
								//// まず移動前ポータルから移動ベクトルの位置に行列を作成
								KdMatrix mForce;
								auto player = GetOwner()->GetComponent<PlayerScript>();
								if (player == nullptr)return;
								KdVec3 vForce = player->GetForce();
								mForce.SetPos(vForce + portalMat.GetPos());
								mForce *= mDelta;
								KdVec3 vDifference;
								vDifference = mForce.GetPos() - mOtherPortal.GetPos();
								mForce.SetPos(vDifference);
								mForce.RotateAxis(vY, 180);
								KdVec3 vTransForce = mForce.GetPos();
								player->GetForce() = vTransForce;
							}
						}

						if (res.Collider->GetOwner()->GetName() == "Portal2")
						{
							// 反発ベクトル
							KdVec3 vPush = res.Push;
							// ポータルの法線ベクトルを取得
							KdMatrix portalMat = res.Collider->GetOwner()->GetMatrix();
							KdVec3 vZ = portalMat.GetZAxis();
							// 法線ベクトルと反発ベクトルの内積
							float dot = vZ.Dot(vPush);
							dot = std::min(1.0f, dot);
							dot = std::max(-1.0f, dot);
							// 0より小さいならポータルをくぐった
							if (dot < 0)
							{
								// もう片方のポータルにワープさせる
								KdMatrix ownerMat = GetOwner()->GetMatrix();
								KdMatrix m = ownerMat;
								auto otherPortal = GameMgr.m_level->GetRoot()->FindObject("Portal1");
								if (otherPortal == nullptr)return;
								KdMatrix mOtherPortal = otherPortal->GetMatrix();
								KdMatrix mInvPortal;
								portalMat.Inverse(mInvPortal);
								KdMatrix mDelta = mInvPortal * mOtherPortal;
								// 追加部分 /////////////////////////////////
								m = mOtherPortal;
								GetOwner()->SetMatrix(m, false);
								m = ownerMat;
								/////////////////////////////////////////////
								m *= mDelta;

								KdVec3 vY = mOtherPortal.GetYAxis();
								KdVec3 vec = m.GetPos() - (mOtherPortal.GetPos()) + coll->GetLocalPos();
								m.SetPos(vec);
								m.RotateAxis(vY, 180);
								vec = m.GetPos() + ((mOtherPortal.GetPos()) - coll->GetLocalPos());
								m.SetPos(vec);
								//GetOwner()->SetMatrix(m, false);

								// カメラの角度も変更
								auto cam = GetOwner()->FindObject("Cam");
								if (cam)
								{
									auto fpscomp = cam->GetComponent<FpsCameraComponent>();
									if (fpscomp)
									{
										KdMatrix mAfterCam = cam->GetMatrix();
										KdMatrix mInvCam;
										GetOwner()->GetPrevMatrix().Inverse(mInvCam);
										KdMatrix mDelta = mInvCam * m;
										mAfterCam *= mDelta;

										KdVec3 aftervZ = mAfterCam.GetZAxis();
										aftervZ.y = 0;
										aftervZ.Normalize();
										float ownerdot = aftervZ.Dot(KdVec3(0, 0, 1));
										ownerdot = std::min(1.0f, ownerdot);
										ownerdot = std::max(-1.0f, ownerdot);
										ownerdot = DirectX::XMConvertToDegrees(acos(ownerdot));
										if (aftervZ.x < 0.0f) ownerdot = -ownerdot;
										fpscomp->GetAngY() = ownerdot;

										KdMatrix mMoveCam = mAfterCam;
										mMoveCam.SetPos(KdVec3(0, 0, 0));
										//mMoveCam.RotateAxis(vY, 180);
										mMoveCam.SetPos(mAfterCam.GetPos());

										aftervZ = mMoveCam.GetZAxis();
										aftervZ.Normalize();
										KdVec3 vHeightZero = aftervZ;
										vHeightZero.y = 0;
										vHeightZero.Normalize();

										ownerdot = aftervZ.Dot(vHeightZero);
										ownerdot = std::min(1.0f, ownerdot);
										ownerdot = std::max(-1.0f, ownerdot);
										ownerdot = DirectX::XMConvertToDegrees(acos(ownerdot));
										if (aftervZ.y > 0.0f) ownerdot = -ownerdot;
										fpscomp->GetAngX() = ownerdot;

										KdMatrix mCam = cam->GetMatrix();
										KdMatrix mRX, mRY, mR;
										mRX.RotateX(fpscomp->GetAngX());
										mRY.RotateY(fpscomp->GetAngY());
										mR = mRX * mRY;
										mCam.SetRotation(mR);
										mCam.LookAtDir(mCam.GetZAxis());
										cam->SetMatrix(mCam, false);
									}
								}

								// 剛体の移動を制御
								auto rigidbodyComp = GetOwner()->GetComponent<RigidBodyComponent>();
								if (rigidbodyComp)
								{
									KdVec3 velocity;
									auto rigidbody = rigidbodyComp->GetRigidBody();
									rigidbody->GetLinearVelocity(velocity);
									rigidbody->SetLinearVelocity(KdVec3(0, 0, 0));
									rigidbody->SetMatrix(m);

									KdMatrix mTmp;
									mTmp.SetPos(velocity + portalMat.GetPos());
									mTmp *= mDelta;
									KdVec3 vTmp;
									vTmp = mTmp.GetPos() - mOtherPortal.GetPos();
									mTmp.SetPos(vTmp);
									mTmp.RotateAxis(vY, 180);
									KdVec3 vTransVelocity = mTmp.GetPos();
									rigidbody->SetLinearVelocity(vTransVelocity);
								}

								// ワープした後の移動ベクトルを変換する
								// まず移動前ポータルから移動ベクトルの位置に行列を作成
								KdMatrix mForce;
								auto player = GetOwner()->GetComponent<PlayerScript>();
								if (player == nullptr)return;
								KdVec3 vForce = player->GetForce();
								mForce.SetPos(vForce + portalMat.GetPos());
								mForce *= mDelta;
								KdVec3 vDifference;
								vDifference = mForce.GetPos() - mOtherPortal.GetPos();
								mForce.SetPos(vDifference);
								mForce.RotateAxis(vY, 180);
								KdVec3 vTransForce = mForce.GetPos();
								player->GetForce() = vTransForce;
							}
						}
					}
				}
			};
		}
	}

	// ポータルカメラのローカル移動(プレイヤーの移動したベクトルを使用する)
	//KdMatrix mOwner = owner->GetMatrix();
	//KdMatrix mPrevOwner = owner->GetPrevMatrix();
	//KdVec3 ownerMovedVec = mOwner.GetPos() - mPrevOwner.GetPos();
	//auto portal1cam = GameMgr.m_level->GetRoot()->FindObject("Portal1")->FindObject("Cam");
	//if (portal1cam == nullptr)return;
	//auto portal2cam = GameMgr.m_level->GetRoot()->FindObject("Portal2")->FindObject("Cam");
	//if (portal2cam == nullptr)return;
	//KdMatrix mPortal1cam = portal1cam->GetMatrix();
	//KdMatrix mPortal2cam = portal2cam->GetMatrix();
	//mPortal1cam.Move_Local(ownerMovedVec);
	//mPortal2cam.Move_Local(ownerMovedVec);
	//portal1cam->SetMatrix(mPortal1cam, false);
	//portal2cam->SetMatrix(mPortal2cam, false);
	//// デフォルトのカメラ行列も移動
	//m_Portal1DefaultCamMat.Move_Local(ownerMovedVec);
	//m_Portal2DefaultCamMat.Move_Local(ownerMovedVec);
}

void UsePortalComponent::Draw()
{
}

void UsePortalComponent::ImGuiUpdate()
{
}

void UsePortalComponent::EditorDraw()
{
	ShMgr.m_KdPrimSh.DrawLine(cv, { 0,0,0 }, { 1, 1, 0, 1 });

}

// _rayposは発射した位置
// _posはあたった場所
// _normalはあたったポリゴンの法線
void UsePortalComponent::Set(const KdVec3& _raypos, const KdVec3& _pos, const KdVec3& _normal, const int _no)
{
	auto owner = GetOwner();

	if (!(_no == 1 || _no == 2)) return;

	SPtr<Object> portal = GameMgr.Instantiate(KDResFactory.GetJson("data/Portal.pref"));

	if (portal == nullptr)return;

	// 既にポータル生成済みなら削除して新しいポータルを作成する
	if (_no == 1)
	{
		auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
		if (portal1)
		{
			GameMgr.m_level->GetRoot()->RemoveChild(portal1);
		}
		portal->SetName("Portal1");
	}
	else if (_no == 2)
	{
		auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");
		if (portal2)
		{
			GameMgr.m_level->GetRoot()->RemoveChild(portal2);
		}
		portal->SetName("Portal2");
	}

	// ポータルを正しい位置にセットする
	KdMatrix m = portal->GetMatrix();
	KdVec3 raypos = _raypos;
	KdVec3 pos = _pos;
	raypos.y = pos.y;
	KdVec3 normal = _normal;
	normal.Normalize();
	if (normal.y < 0.0f)
	{
		m.LookAtDirMin(pos - raypos);
	}
	else
	{
		m.LookAtDirMin(raypos - pos);
	}

	m.LookAtDirMin(normal);

	m.Move(_pos);

	// 設置した場所から少しだけ浮かせる
	m.Move_Local(0, 0, 0.001f);

	portal->SetMatrix(m, true);
	portal->SetMatrix(m, true);

	//// ポータルのカメラを生成、配置(そのポータルを覗いた時に見るカメラ)
	//auto portalcam = portal->FindObject("Cam");
	//if (portalcam)
	//{
	//	auto ownerCam = owner->FindObject("Cam");
	//	if (ownerCam)
	//	{
	//		KdMatrix ownerCamMat = ownerCam->GetMatrix();
	//		KdMatrix camMat = portalcam->GetMatrix();

	//		// ポータルカメラの位置を決める
	//		KdVec3 normal = _normal;
	//		normal.Normalize();
	//		camMat = ownerCamMat;
	//		KdMatrix rotMat;

	//		KdVec3 vY = portal->GetMatrix().GetYAxis();

	//		// カメラからポータルのベクトルを取得
	//		KdVec3 vec = camMat.GetPos() - portal->GetMatrix().GetPos();

	//		rotMat.LookAtDirMin(vY);

	//		rotMat.SetPos(vec);

	//		rotMat.RotateAxis(vY, 180);

	//		rotMat.Move(portal->GetMatrix().GetPos());
	//		// 予定 ************************************
	//		// このカメラ行列をもう片方のポータルに移動させてもう一つポータルの方向を注視させる
	//		//******************************************
	//		camMat = rotMat;

	//		if (_no == 1)
	//		{
	//			m_Portal1DefaultCamMat = camMat;
	//		}
	//		else if (_no == 2)
	//		{
	//			m_Portal2DefaultCamMat = camMat;
	//		}

	//		portalcam->SetMatrix(camMat, false);


	//		// test***************************************************************
	//		// 
	//		// fovを算出
	//		KdVec3 camtoportal = camMat.GetPos() - _raypos;
	//		KdVec3 texlefttop = camtoportal + KdVec3{ -1, 0, 0 };
	//		texlefttop.Normalize();
	//		KdVec3 texrighttop = camtoportal + KdVec3{ 1,0,0 };
	//		texrighttop.Normalize();
	//		float dot = texlefttop.Dot(texrighttop);
	//		dot = std::min(1.0f, dot);
	//		dot = std::max(-1.0f, dot);
	//		float fov = DirectX::XMConvertToDegrees(acos(dot));

	//		// portalcamのカメラコンポーネントにセット
	//		auto camcomponent = portalcam->GetComponent<CameraComponent>();
	//		if (camcomponent)
	//		{
	//			camcomponent->SetProjection(fov, 0.1f, 1000.0f);
	//			camcomponent->SetAspect(1.0f);
	//		}

	//		KdVec3 testvec = { 0,0,10 };
	//		KdMatrix testmat;

	//		//******************************************************************
	//	}
	//}

	// ポータルの親を設定
	portal->SetParent(GameMgr.m_level->GetRoot());
	//portal->SetParent(owner->shared_from_this());

	// 2つのポータルの存在チェック
	auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
	if (!portal1)return;
	auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");
	if (!portal2)return;

	auto portal1cam = portal1->FindObject("Cam");
	if (!portal1cam)return;

	{
		// ポータルが両方生成されたとき1度だけ実行するべき？
		// ポータル1のカメラを移動
		//KdMatrix camMat = m_Portal1DefaultCamMat;
		//KdMatrix portalMat;
		//portal1->GetMatrix().Inverse(portalMat);
		//KdMatrix mDelta = portalMat * portal2->GetMatrix();
		//mDelta.NormalizeScale();
		//camMat *= mDelta;

		//// カメラの向く方向ベクトルを取得
		//KdVec3 vLookDir = portal2->GetMatrix().GetPos() - camMat.GetPos();

		//// カメラを一度原点に戻し方向ベクトルに向かせて元の位置に戻す
		//KdVec3 vPrevCamVec = camMat.GetPos();
		//camMat.SetPos(0.0f, 0.0f, 0.0f);
		//camMat.LookAtDirMin(vLookDir);
		//camMat.SetPos(vPrevCamVec);

		//portal1cam->SetMatrix(camMat, false);
	}
}

void UsePortalComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);
}

void UsePortalComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);
}

