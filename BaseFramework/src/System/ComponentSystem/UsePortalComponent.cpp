#include "main.h"

#include "../Gamemanager.h"

#include "UsePortalComponent.h"

void UsePortalComponent::Update()
{
	if (m_enable == false)return;

	// �|�[�^���R���|�[�l���g�������Ă���I�[�i�[�擾
	auto owner = GetOwner();
	if (!owner) return;

	// �I�[�i�[�̋��R���C�_�[�R���|�[�l���g�����ׂĎ擾
	auto colliders = owner->GetComponents<SphereColliderComponent>();
	for (auto&& coll : colliders)
	{
		if (coll->GetTag() == "Portal")
		{
			// �q�b�g���̎��s�����֐���o�^
			coll->m_onHitStay = [this, coll](BaseColliderComponent* collider)
			{
				// �q�b�g�����z��(Collider)�S��
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() == "Portal")
					{
						if (res.Collider->GetOwner()->GetName() == "Portal1")
						{
							// �����x�N�g��
							KdVec3 vPush = res.Push;
							// �|�[�^���̖@���x�N�g�����擾
							KdMatrix portalMat = res.Collider->GetOwner()->GetMatrix();
							KdVec3 vZ = portalMat.GetZAxis();
							// �@���x�N�g���Ɣ����x�N�g���̓���
							float dot = vZ.Dot(vPush);
							dot = std::min(1.0f, dot);
							dot = std::max(-1.0f, dot);
							// 0��菬�����Ȃ�|�[�^������������
							if (dot < 0)
							{
								// �����Е��̃|�[�^���Ƀ��[�v������
								KdMatrix ownerMat = GetOwner()->GetMatrix();
								KdMatrix m = ownerMat;
								auto otherPortal = GameMgr.m_level->GetRoot()->FindObject("Portal2");
								if (otherPortal == nullptr)return;
								KdMatrix mOtherPortal = otherPortal->GetMatrix();
								KdMatrix mInvPortal;
								portalMat.Inverse(mInvPortal);
								KdMatrix mDelta = mInvPortal * mOtherPortal;
								// �ǉ����� /////////////////////////////////
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

								// �J�����̊p�x���ύX
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

								// ���̂̈ړ��𐧌�
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

								//// ���[�v������̈ړ��x�N�g����ϊ�����
								//// �܂��ړ��O�|�[�^������ړ��x�N�g���̈ʒu�ɍs����쐬
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
							// �����x�N�g��
							KdVec3 vPush = res.Push;
							// �|�[�^���̖@���x�N�g�����擾
							KdMatrix portalMat = res.Collider->GetOwner()->GetMatrix();
							KdVec3 vZ = portalMat.GetZAxis();
							// �@���x�N�g���Ɣ����x�N�g���̓���
							float dot = vZ.Dot(vPush);
							dot = std::min(1.0f, dot);
							dot = std::max(-1.0f, dot);
							// 0��菬�����Ȃ�|�[�^������������
							if (dot < 0)
							{
								// �����Е��̃|�[�^���Ƀ��[�v������
								KdMatrix ownerMat = GetOwner()->GetMatrix();
								KdMatrix m = ownerMat;
								auto otherPortal = GameMgr.m_level->GetRoot()->FindObject("Portal1");
								if (otherPortal == nullptr)return;
								KdMatrix mOtherPortal = otherPortal->GetMatrix();
								KdMatrix mInvPortal;
								portalMat.Inverse(mInvPortal);
								KdMatrix mDelta = mInvPortal * mOtherPortal;
								// �ǉ����� /////////////////////////////////
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

								// �J�����̊p�x���ύX
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

								// ���̂̈ړ��𐧌�
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

								// ���[�v������̈ړ��x�N�g����ϊ�����
								// �܂��ړ��O�|�[�^������ړ��x�N�g���̈ʒu�ɍs����쐬
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

	// �|�[�^���J�����̃��[�J���ړ�(�v���C���[�̈ړ������x�N�g�����g�p����)
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
	//// �f�t�H���g�̃J�����s����ړ�
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

// _raypos�͔��˂����ʒu
// _pos�͂��������ꏊ
// _normal�͂��������|���S���̖@��
void UsePortalComponent::Set(const KdVec3& _raypos, const KdVec3& _pos, const KdVec3& _normal, const int _no)
{
	auto owner = GetOwner();

	if (!(_no == 1 || _no == 2)) return;

	SPtr<Object> portal = GameMgr.Instantiate(KDResFactory.GetJson("data/Portal.pref"));

	if (portal == nullptr)return;

	// ���Ƀ|�[�^�������ς݂Ȃ�폜���ĐV�����|�[�^�����쐬����
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

	// �|�[�^���𐳂����ʒu�ɃZ�b�g����
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

	// �ݒu�����ꏊ���班��������������
	m.Move_Local(0, 0, 0.001f);

	portal->SetMatrix(m, true);
	portal->SetMatrix(m, true);

	//// �|�[�^���̃J�����𐶐��A�z�u(���̃|�[�^����`�������Ɍ���J����)
	//auto portalcam = portal->FindObject("Cam");
	//if (portalcam)
	//{
	//	auto ownerCam = owner->FindObject("Cam");
	//	if (ownerCam)
	//	{
	//		KdMatrix ownerCamMat = ownerCam->GetMatrix();
	//		KdMatrix camMat = portalcam->GetMatrix();

	//		// �|�[�^���J�����̈ʒu�����߂�
	//		KdVec3 normal = _normal;
	//		normal.Normalize();
	//		camMat = ownerCamMat;
	//		KdMatrix rotMat;

	//		KdVec3 vY = portal->GetMatrix().GetYAxis();

	//		// �J��������|�[�^���̃x�N�g�����擾
	//		KdVec3 vec = camMat.GetPos() - portal->GetMatrix().GetPos();

	//		rotMat.LookAtDirMin(vY);

	//		rotMat.SetPos(vec);

	//		rotMat.RotateAxis(vY, 180);

	//		rotMat.Move(portal->GetMatrix().GetPos());
	//		// �\�� ************************************
	//		// ���̃J�����s��������Е��̃|�[�^���Ɉړ������Ă�����|�[�^���̕����𒍎�������
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
	//		// fov���Z�o
	//		KdVec3 camtoportal = camMat.GetPos() - _raypos;
	//		KdVec3 texlefttop = camtoportal + KdVec3{ -1, 0, 0 };
	//		texlefttop.Normalize();
	//		KdVec3 texrighttop = camtoportal + KdVec3{ 1,0,0 };
	//		texrighttop.Normalize();
	//		float dot = texlefttop.Dot(texrighttop);
	//		dot = std::min(1.0f, dot);
	//		dot = std::max(-1.0f, dot);
	//		float fov = DirectX::XMConvertToDegrees(acos(dot));

	//		// portalcam�̃J�����R���|�[�l���g�ɃZ�b�g
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

	// �|�[�^���̐e��ݒ�
	portal->SetParent(GameMgr.m_level->GetRoot());
	//portal->SetParent(owner->shared_from_this());

	// 2�̃|�[�^���̑��݃`�F�b�N
	auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
	if (!portal1)return;
	auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");
	if (!portal2)return;

	auto portal1cam = portal1->FindObject("Cam");
	if (!portal1cam)return;

	{
		// �|�[�^���������������ꂽ�Ƃ�1�x�������s����ׂ��H
		// �|�[�^��1�̃J�������ړ�
		//KdMatrix camMat = m_Portal1DefaultCamMat;
		//KdMatrix portalMat;
		//portal1->GetMatrix().Inverse(portalMat);
		//KdMatrix mDelta = portalMat * portal2->GetMatrix();
		//mDelta.NormalizeScale();
		//camMat *= mDelta;

		//// �J�����̌��������x�N�g�����擾
		//KdVec3 vLookDir = portal2->GetMatrix().GetPos() - camMat.GetPos();

		//// �J��������x���_�ɖ߂������x�N�g���Ɍ������Č��̈ʒu�ɖ߂�
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

