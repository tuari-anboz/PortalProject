#include "main.h"

// �V�X�e���n
#include "../GameManager.h"

// �Q�[���n
#include "PlayerScript.h"

//=================================

void PlayerScript::Start()
{
	// �I�[�i�[�擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// �����s��
	m_as = std::make_shared<Stand>();
}

void PlayerScript::Update()
{
	// �I�[�i�[�擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// ���f���擾
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// ���͎擾
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return;

	// �q�b�g�X�g�b�v���łȂ��Ƃ�
	if (m_hitStop <= 0) {

		// �A�j���[�^�������߂�
		if (model) {
			model->GetAnimator().GetTrack()[0]->m_Enable = true;
		}

		// �s������
		if (m_as) {
			// ������̃L�����̃A�h���X�����Ă���
			m_as->m_pChara = this;
			// �s���������s
			auto p = m_as->Update();
			if (p) {
				m_as = p;
			}
		}

		// [����]�����s������
		if (m_asExt) {
			m_asExt->m_pChara = this;
			m_asExt = m_asExt->Update();

		}
	}
	// �q�b�g�X�g�b�v��
	else {
		m_hitStop--;

		// �A�j���[�^���~�߂�
		if (model) {
			// �A�j���[�V�����g���b�N[0]�𖳌��ɂ���
			// ���g���b�N �c ���݂̃A�j���[�V�����̐i�s�f�[�^
			model->GetAnimator().GetTrack()[0]->m_Enable = false;
		}
	}

	// E�L�[�Ń{�b�N�X���������������肷��
	if (input->m_keys['E'])
	{
		auto cam = owner->FindObject("Cam");
		if (cam)
		{
			auto getbox = cam->GetComponent<GettingBoxComponent>();
			if (getbox)
			{
				getbox->GetBox();
			}
		}
	}

	// �����蔻��
	{
		SetIsPortalCollide() = false;
		// 2�̃|�[�^���̑��݃`�F�b�N
		auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
		auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");

		auto colliders = owner->GetComponents<BaseColliderComponent>();
		for (auto&& coll : colliders)
		{
			// �T�[�`
			if (coll->GetTag() == "Search" && portal1 && portal2)
			{
				// ���̋����q�b�g�������Ɏ��s�����֐������Ă���
				coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
				{
					for (auto&& res : collider->GetResults())
					{
						if (res.Collider->GetTag() == "EnterPortal")
						{
							SetIsPortalCollide() = true;
							// �R���C�_�[�R���|�[�l���g���u�S�āv�擾����
							auto colliders = owner->GetComponents<BaseColliderComponent>();
							for (auto&& coll : colliders)
							{
								// �Ԃ���n�̔���
								if (coll->GetTag() == "Collision")
								{
									// ���̋����q�b�g�������Ɏ��s�����֐������Ă���
									coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
									{
										// �q�b�g�������(Collider)�S��
										for (auto&& res : collider->GetResults())
										{
											if (res.Collider->GetTag() == "Portal")
											{
												// ���ʔ�v�Z
												float myMass = owner->GetMass();	// �����̎���
												float youMass = res.Collider->GetOwner()->GetMass();// ����̎���
												float massPower = 0;
												// �����̎��ʂ�0�Ȃ�A�����͕s��
												if (myMass == 0) {
													massPower = 0;
												}
												// ����̎��ʂ�0�Ȃ�A�����͓���
												else if (youMass == 0) {
													massPower = 1.0f;
												}
												else {
													// �������ʂ���@�䗦�v�Z(0.0�`1.0)
													massPower = youMass / (myMass + youMass);
												}

												KdMatrix m = owner->GetMatrix();
												m.Move(res.Push * massPower);	// �����߂�
												owner->SetMatrix(m, false);
											}
										}
									};
								}
							}
							// �n�ʔ���
							{
								// ���C����̃R���|�[�l���g���擾����
								auto coll = owner->GetComponent<RayColliderComponent>();
								if (coll) {
									float rayY = coll->GetRayLocalPos().y * owner->GetMatrix().GetYScale() + 1.0f;
									// �q�b�g���̎��s�����֐���o�^
									coll->m_onHitStay = [this, rayY, owner](BaseColliderComponent* collider) {
										// �S�Ă̂�������������A��ԋ߂�������o
										float nearest = FLT_MAX;
										const CollisionResult* nearestRes = nullptr;
										for (auto&& res : collider->GetResults()) {
											if (res.Collider->GetTag() != "EnterPortal")
											{
												// ���߂����̂�����΁A�����
												if (nearest > res.Dist) {
													nearestRes = &res; 
													nearest = res.Dist;
												}
											}
										}
										// ��ԋ߂����
										if (nearestRes) {

											KdMatrix m = owner->GetMatrix();

											// �󒆎�
											if (m_isSky) {
												if (nearestRes->Collider->GetTag() != "Portal")
												{
													// �߂荞��ł���
													if (nearestRes->Dist < rayY && m_vForce.y < 0)
													{
														m_isSky = false;
														// �߂荞�񂾂Ԃ�A�ړ�������
														m.Move(0, rayY - nearestRes->Dist, 0);
														// �d�̓��Z�b�g
														m_vForce.y = 0;
													}
												}
											}
											// �n�㎞
											else {
												// ���ȏ�n�ʂ��痣���ƁA�󒆂Ɣ��f����
												if (nearestRes->Dist >= rayY + 0.1f || m_vForce.y > 0 || nearestRes->Collider->GetTag() == "Portal" || nearestRes->Collider->GetTag() == "EnterPortal") {
													// �󒆂�
													m_isSky = true;
												}
												else {
													// �߂荞�񂾂Ԃ�A�ړ�������
													m.Move(0, rayY - nearestRes->Dist, 0);
													// �d�̓��Z�b�g
													m_vForce.y = 0;

													//-------------------------------
													// �n�ʂ̕ω��ʂԂ�A�ړ�������
													//-------------------------------
													// �n�ʂ̂�̕ω��ʂ��擾
													KdMatrix mDelta;
													nearestRes->Collider->GetOwner()->GetDeltaMatrix(mDelta);
													mDelta.NormalizeScale();	// �g��͖�������
													m *= mDelta;
													// Z�����́AXZ���ʂɐ����ɂ���
													KdVec3 vZ = m.GetZAxis();
													vZ.y = 0;
													m.LookTo(vZ, { 0,1,0 });
												}
											}
											// �s��Z�b�g
											owner->SetMatrix(m, false);
										}
									};
								}
							}
						}
					}
				};
			}
		}
		// �|�[�^���ɓ������ĂȂ��ꍇ
		for (auto&& coll : colliders)
		{
			// �Ԃ���n�̔���
			if (coll->GetTag() == "Collision")
			{
				// ���̋����q�b�g�������Ɏ��s�����֐������Ă���
				coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
				{
					if (SetIsPortalCollide() == false)
					{
						for (auto&& res : collider->GetResults())
						{
							if (res.Collider->GetTag() == "Collision")
							{
								// ���ʔ�v�Z
								float myMass = owner->GetMass();	// �����̎���
								float youMass = res.Collider->GetOwner()->GetMass();// ����̎���
								float massPower = 0;
								// �����̎��ʂ�0�Ȃ�A�����͕s��
								if (myMass == 0) {
									massPower = 0;
								}
								// ����̎��ʂ�0�Ȃ�A�����͓���
								else if (youMass == 0) {
									massPower = 1.0f;
								}
								else {
									// �������ʂ���@�䗦�v�Z(0.0�`1.0)
									massPower = youMass / (myMass + youMass);
								}

								KdMatrix m = owner->GetMatrix();
								m.Move(res.Push * massPower);	// �����߂�
								owner->SetMatrix(m, false);
							}
						}
					}
				};
			}
		}
		// �n�ʔ���
		{
			// ���C����̃R���|�[�l���g���擾����
			auto coll = owner->GetComponent<RayColliderComponent>();
			if (coll) {
				float rayY = coll->GetRayLocalPos().y * owner->GetMatrix().GetYScale() + 1.0f;
				// �q�b�g���̎��s�����֐���o�^
				coll->m_onHitStay = [this, rayY, owner](BaseColliderComponent* collider)
				{
					if (SetIsPortalCollide() == false)
					{
						// �S�Ă̂�������������A��ԋ߂�������o
						float nearest = FLT_MAX;
						const CollisionResult* nearestRes = nullptr;
						for (auto&& res : collider->GetResults()) {
							if (res.Collider->GetTag() != "EnterPortal" && res.Collider->GetTag() != "Portal")
							{
								// ���߂����̂�����΁A�����
								if (nearest > res.Dist) {
									nearestRes = &res; nearest = res.Dist;
								}
							}
						}
						// ��ԋ߂����
						if (nearestRes)
						{
							KdMatrix m = owner->GetMatrix();

							// �󒆎�
							if (m_isSky) {
								// �߂荞��ł���
								if (nearestRes->Dist < rayY && m_vForce.y < 0)
								{
									m_isSky = false;
									// �߂荞�񂾂Ԃ�A�ړ�������
									m.Move(0, rayY - nearestRes->Dist, 0);
									// �d�̓��Z�b�g
									m_vForce.y = 0;
								}
							}
							// �n�㎞
							else {
								// ���ȏ�n�ʂ��痣���ƁA�󒆂Ɣ��f����
								if (nearestRes->Dist >= rayY + 0.1f || m_vForce.y > 0) {
									// �󒆂�
									m_isSky = true;
								}
								else {
									// �߂荞�񂾂Ԃ�A�ړ�������
									m.Move(0, rayY - nearestRes->Dist, 0);
									// �d�̓��Z�b�g
									m_vForce.y = 0;

									//-------------------------------
									// �n�ʂ̕ω��ʂԂ�A�ړ�������
									//-------------------------------
									// �n�ʂ̂�̕ω��ʂ��擾
									KdMatrix mDelta;
									nearestRes->Collider->GetOwner()->GetDeltaMatrix(mDelta);
									mDelta.NormalizeScale();	// �g��͖�������
									m *= mDelta;
									// Z�����́AXZ���ʂɐ����ɂ���
									KdVec3 vZ = m.GetZAxis();
									vZ.y = 0;
									m.LookTo(vZ, { 0,1,0 });
								}
							}
							// �s��Z�b�g
							owner->SetMatrix(m, false);
						}
					}
				};
			}
		}
	}
}

void PlayerScript::DrawEffect()
{

}

void PlayerScript::ImGuiUpdate()
{
	// Object�̂ق���ImGuiUpdate�����s
	BaseComponent::ImGuiUpdate();

	// �I�[�i�[�擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	if (ImGui::CollapsingHeader(owner->GetObjectClassName().c_str(),
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Hp
		ImGui::InputInt("Hp", &m_hp);

		// �ړ����x
		ImGui::InputFloat(u8"�ړ����x", &m_ms);

		// �v���n�u�t�@�C��
		ImGuiResourceButton("Prefab File", m_editorPrefabName, { ".pref" });
	}
}


SPtr<PlayerScript::BaseAction> PlayerScript::Stand::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// InputComponent����������
	auto input = owner->GetComponent<InputComponent>();
	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	if (input)
	{
		// �����L�[�����Ă���
		if (input->m_keysRepeat['W'] || input->m_keysRepeat['S'] || input->m_keysRepeat['A'] || input->m_keysRepeat['D']) {
			// �A�j���ύX
			//model->GetAnimator().ChangeAnimeSmooth("����", true, 0.05f);
			// �s���؂�ւ�
			return std::make_shared<Walk>();
		}

		// �W�����v
		if (input->m_keysRepeat[VK_SPACE]) {
			// �s���؂�ւ�
			auto as = std::make_shared<JumpStart>();
			as->m_vForce.Set(0, 0.075f, 0);

			return as;
		}
		// X�L�[�Ń|�[�^���̕Е��𔭎�
		if (input->m_keys['X'])
		{

			// �s���ؑ�
			return std::make_shared<Shot>();
		}
		// C�L�[�Ń|�[�^���̂����Е��𔭎�
		if (input->m_keys['C'])
		{
			// �s���ؑ�
			return std::make_shared<Shot2>();
		}
	}

	// �󒆁H
	if (m_pChara->m_isSky) {
		// �A�j���ύX
		model->GetAnimator().ChangeAnimeSmooth("����", false, 0.1f);
		// �s���؂�ւ�
		return std::make_shared<JumpDown>();
	}

	// ���C
	m_pChara->m_vForce *= 0.9f;

	// �d��
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// �͈ړ�
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Walk::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// InputComponent����������
	SPtr<InputComponent> input = owner->GetComponent<InputComponent>();
	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;



	KdVec3 vDir;
	// �q����J�����I�u�W�F�N�g�������E�擾
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//�J������Z����
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y�����ׂ͒�
		camZ.Normalize();	// ���K��

		// �J������X����
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y�����ׂ͒�
		camX.Normalize();	// ���K��

		// �����L�[�����ƂɁA�ړ�����������
		if(input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

		// ���͂����鎞
		if (vDir.LengthSquared() > 0) {
			vDir.Normalize();

			// �L�����̍s��
			KdMatrix m = owner->GetMatrix();

			// �L�����̌���(Z)
			KdVec3 vZ = m.GetZAxis();
			vZ.Normalize();	// ���K��

			// �p�x�����߂�
			const float maxAng = 10;
			float dot = vZ.Dot(vDir);
			dot = std::min(1.0f, dot);// if(dot > 1.0f)dot = 1.0f;
			dot = std::max(-1.0f, dot);
			// �p�x����
			float ang = DirectX::XMConvertToDegrees(acos(dot));
			if (ang > maxAng) {
				ang = maxAng;
			}
			// vZ��ang������]
			// �O�ςŉ�]�����Z�o
			KdVec3 crs = vZ.Cross(vDir);
			//float L = crs.Length();
			// �����x�N�g�������߂��Ȃ��Ƃ��́A
			// �K����(0, 1, 0)�Ƃ���B
			if (crs.LengthSquared() <= 0.0001f) {
				crs.Set(0, 1, 0);
			}
			crs.Normalize();

			// crs�����̉�]�s������߂�
			KdMatrix mR;
			mR.CreateRotateAxis(crs, ang);
			// vZ��mR�ŕϊ�
			vZ.TransformNormal(mR);
			// vZ�̕����Ɍ�����
			m.LookTo(vZ, { 0, 1, 0 });

			// �ړ�
			m.Move(vDir*0.05f);
			//m_pChara->m_vForce += vDir * 0.05f;
			owner->SetMatrix(m, false);
		}
	}

	if (input) {
		// �����L�[�����Ă���
		if (input->m_keysRepeat['W'] == 0 && input->m_keysRepeat['S'] == 0 && input->m_keysRepeat['A'] == 0 && input->m_keysRepeat['D'] == 0) {
			// �A�j���ύX
			//model->GetAnimator().ChangeAnimeSmooth("����", true, 0.05f);
			// �s���؂�ւ�
			return std::make_shared<Stand>();
		}
		// �W�����v
		if (input->m_keysRepeat[VK_SPACE]) {
			// �A�j���ύX
			//model->GetAnimator().ChangeAnimeSmooth("�W�����v�J�n", false, 1.0f);
			// �s���؂�ւ�
			auto as = std::make_shared<JumpStart>();
			as->m_vForce.Set(0, 0.075f, 0);

			return as;
		}
		// X�L�[
		if (input->m_keysRepeat['X'])
		{

			// �s���ؑ�
			return std::make_shared<Shot>();
		}
		// C�L�[
		if (input->m_keysRepeat['C'])
		{
			// �s���ؑ�
			return std::make_shared<Shot2>();
		}
	}

	// �󒆁H
	if (m_pChara->m_isSky) {
		// �A�j���ύX
		//model->GetAnimator().ChangeAnimeSmooth("����", false, 0.1f);
		// �s���؂�ւ�
		return std::make_shared<JumpDown>();
	}

	// ���C
	m_pChara->m_vForce *= 0.9f;

	// �d��
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// �͈ړ�
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);


	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Shot::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam)
	{
		// �|�[�^���W�J
		// ���C����̃R���|�[�l���g���擾����
		auto coll = cam->GetComponent<RayColliderComponent>();
		if (coll) {
			coll->SetEnable(true);
			// �q�b�g���̎��s�����֐���o�^
			coll->m_onHitStay = [this, rayPos = coll->GetRayPos(), camObj = cam](BaseColliderComponent* collider)
			{
				// �S�Ă̂�������������A��ԋ߂�������o
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults()) {
					if (res.Collider->GetTag() != "Portal")
					{
						// ���߂����̂�����΁A�����
						if (nearest > res.Dist) {
							nearestRes = &res;
							nearest = res.Dist;
						}
					}
				}
				// 
				if (nearestRes) {
					// ��ԋ߂������|�[�^�����ݒu�\�ȏꍇ�̓|�[�^����ݒu���A�����łȂ��ꍇ�̓G�t�F�N�g��ύX����
					if (nearestRes->Collider->GetTag() == "SetPortal")
					{
						auto portal = camObj->GetParent()->GetComponent<UsePortalComponent>();
						if (portal)
						{
							// ���������I�u�W�F�N�g�̃��f���R���|�[�l���g���擾
							auto modelcomp = nearestRes->Collider->GetOwner()->GetComponent<ModelComponent>();
							if (!modelcomp) return;
							// ���f���R���|�[�l���g���烂�f�����擾
							auto model = modelcomp->GetModel();
							if (!model) return;
							// ���f����3D���b�V���z����擾
							auto models = model->GetModels();
							// 3D���b�V���z�񂩂�Ώۂ̃��b�V�����擾
							auto mesh = models[nearestRes->MeshIdx];
							if (!mesh) return;
							// ���b�V���̖@�����擾
							auto meshNormal = mesh->GetMesh()->GetExtFaceArray()[nearestRes->FaceIdx].vN;
							if (!meshNormal) return;
							// ���b�V���̖@�����s��ŕϊ�
							meshNormal.TransformNormal(nearestRes->Collider->GetOwner()->GetMatrix());
							// �|�[�^����ݒu
							portal->Set(camObj->GetMatrix().GetPos(), nearestRes->Pos, meshNormal, 1);
						}
					}
					else
					{
						// �e���G�t�F�N�g�𐶐�
						auto obj = GameMgr.Instantiate(KDResFactory.GetJson("data/Texture/Effect/FlashEffect.pref"));
						if (obj)
						{
							KdMatrix m = obj->GetMatrix();
							m.SetPos(nearestRes->Pos);
							obj->SetMatrix(m, false);
							obj->SetParent(GameMgr.m_level->GetRoot());
						}
					}
				}
				collider->SetEnable(false);
			};
		}
	}


	return std::make_shared<Stand>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::Shot2::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam)
	{
		// �|�[�^���W�J
		// ���C����̃R���|�[�l���g���擾����
		auto coll = cam->GetComponent<RayColliderComponent>();
		if (coll)
		{
			coll->SetEnable(true);
			// �q�b�g���̎��s�����֐���o�^
			coll->m_onHitStay = [this, rayY = coll->GetRayLocalPos().y, coll, camObj = cam](BaseColliderComponent* collider)
			{
				// �S�Ă̂�������������A��ԋ߂�������o
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() != "Portal")
					{
						// ���߂����̂�����΁A�����
						if (nearest > res.Dist) {
							nearestRes = &res;
							nearest = res.Dist;
						}
					}
				}
				//
				if (nearestRes)
				{
					if (nearestRes->Collider->GetTag() == "SetPortal")
					{
						auto portal = camObj->GetParent()->GetComponent<UsePortalComponent>();
						if (portal)
						{
							// ���������I�u�W�F�N�g�̃��f���R���|�[�l���g���擾
							auto modelcomp = nearestRes->Collider->GetOwner()->GetComponent<ModelComponent>();
							if (!modelcomp) return;
							// ���f���R���|�[�l���g���烂�f�����擾
							auto model = modelcomp->GetModel();
							if (!model) return;
							// ���f����3D���b�V���z����擾
							auto models = model->GetModels();
							// 3D���b�V���z�񂩂�Ώۂ̃��b�V�����擾
							auto mesh = models[nearestRes->MeshIdx];
							if (!mesh) return;
							// ���b�V���̖@�����擾
							auto meshNormal = mesh->GetMesh()->GetExtFaceArray()[nearestRes->FaceIdx].vN;
							if (!meshNormal) return;
							// ���b�V���̖@�����s��ŕϊ�
							meshNormal.TransformNormal(nearestRes->Collider->GetOwner()->GetMatrix());
							// �|�[�^����ݒu
							portal->Set(camObj->GetMatrix().GetPos(), nearestRes->Pos, meshNormal, 2);
						}
					}
				}
				collider->SetEnable(false);
			};
		}
	}


	return std::make_shared<Stand>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::GetBox::Update()
{
	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Generic::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	// �A�j���I������
	if (model->GetAnimator().IsAnimationEnd()) {
		// �A�j���ύX
		model->GetAnimator().ChangeAnimeSmooth("����", true, 0.05f);
		// �s���؂�ւ�
		return std::make_shared<Stand>();
	}

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Die::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	// �A�j���I������
	if (model->GetAnimator().IsAnimationEnd()) {
		m_cnt++;

		auto m = owner->GetMatrix();
		m.Move(0, 0.1f, 0);
		owner->SetMatrix(m, false);

		if (m_cnt > 60) {
			// �L����������
			m_pChara->Delete();
		}
	}

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpStart::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponent���擾����
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	//// �A�j���I������
	//if (model->GetAnimator().IsAnimationEnd()) {

	//	// �A�j���ύX
	//	model->GetAnimator().ChangeAnimeSmooth("�㏸", false, 1);
	//	// �s���؂�ւ�
	//	return std::make_shared<JumpUp>();
	//}

	KdVec3 vDir;
	// �q����J�����I�u�W�F�N�g�������E�擾
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//�J������Z����
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y�����ׂ͒�
		camZ.Normalize();	// ���K��

		// �J������X����
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y�����ׂ͒�
		camX.Normalize();	// ���K��

		// �����L�[�����ƂɁA�ړ�����������
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

		// ���͂����鎞
		if (vDir.LengthSquared() > 0) {
			vDir.Normalize();

			// �L�����̍s��
			KdMatrix m = owner->GetMatrix();

			// �L�����̌���(Z)
			KdVec3 vZ = m.GetZAxis();
			vZ.Normalize();	// ���K��

			// �p�x�����߂�
			const float maxAng = 10;
			float dot = vZ.Dot(vDir);
			dot = std::min(1.0f, dot);// if(dot > 1.0f)dot = 1.0f;
			dot = std::max(-1.0f, dot);
			// �p�x����
			float ang = DirectX::XMConvertToDegrees(acos(dot));
			if (ang > maxAng) {
				ang = maxAng;
			}
			// vZ��ang������]
			// �O�ςŉ�]�����Z�o
			KdVec3 crs = vZ.Cross(vDir);
			//float L = crs.Length();
			// �����x�N�g�������߂��Ȃ��Ƃ��́A
			// �K����(0, 1, 0)�Ƃ���B
			if (crs.LengthSquared() <= 0.0001f) {
				crs.Set(0, 1, 0);
			}
			crs.Normalize();

			// crs�����̉�]�s������߂�
			KdMatrix mR;
			mR.CreateRotateAxis(crs, ang);
			// vZ��mR�ŕϊ�
			vZ.TransformNormal(mR);
			// vZ�̕����Ɍ�����
			m.LookTo(vZ, { 0, 1, 0 });

			// �ړ�
			m.Move(vDir*0.05f);
			owner->SetMatrix(m, false);
		}
	}

	// ��ɗ͂�������
	m_pChara->m_vForce = m_vForce;

	// �s���؂�ւ�
	return std::make_shared<JumpUp>();

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpUp::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponent���擾����
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	// �A�j���I��
	//if (model->GetAnimator().IsAnimationEnd()) {
	//	// �A�j���ύX
	//	model->GetAnimator().ChangeAnimeSmooth("����", false, 0.1f);
	//	// �s���؂�ւ�
	//	return std::make_shared<JumpDown>();
	//}

	KdVec3 vDir;
	// �q����J�����I�u�W�F�N�g�������E�擾
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//�J������Z����
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y�����ׂ͒�
		camZ.Normalize();	// ���K��

		// �J������X����
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y�����ׂ͒�
		camX.Normalize();	// ���K��

		// �����L�[�����ƂɁA�ړ�����������
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

		// ���͂����鎞
		if (vDir.LengthSquared() > 0) {
			vDir.Normalize();

			// �L�����̍s��
			KdMatrix m = owner->GetMatrix();

			// �L�����̌���(Z)
			KdVec3 vZ = m.GetZAxis();
			vZ.Normalize();	// ���K��

			// �p�x�����߂�
			const float maxAng = 10;
			float dot = vZ.Dot(vDir);
			dot = std::min(1.0f, dot);// if(dot > 1.0f)dot = 1.0f;
			dot = std::max(-1.0f, dot);
			// �p�x����
			float ang = DirectX::XMConvertToDegrees(acos(dot));
			if (ang > maxAng) {
				ang = maxAng;
			}
			// vZ��ang������]
			// �O�ςŉ�]�����Z�o
			KdVec3 crs = vZ.Cross(vDir);
			//float L = crs.Length();
			// �����x�N�g�������߂��Ȃ��Ƃ��́A
			// �K����(0, 1, 0)�Ƃ���B
			if (crs.LengthSquared() <= 0.0001f) {
				crs.Set(0, 1, 0);
			}
			crs.Normalize();

			// crs�����̉�]�s������߂�
			KdMatrix mR;
			mR.CreateRotateAxis(crs, ang);
			// vZ��mR�ŕϊ�
			vZ.TransformNormal(mR);
			// vZ�̕����Ɍ�����
			m.LookTo(vZ, { 0, 1, 0 });

			// �ړ�
			m.Move(vDir*0.05f);
			owner->SetMatrix(m, false);
		}
	}

	// �d��
	m_pChara->m_vForce.y -= 9.81f * 0.016f * 0.016f;

	// �͈ړ�
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	// �s���؂�ւ�
	return std::make_shared<JumpDown>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpDown::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent���擾����
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponent���擾����
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	KdVec3 vDir;
	// �q����J�����I�u�W�F�N�g�������E�擾
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//�J������Z����
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y�����ׂ͒�
		camZ.Normalize();	// ���K��

		// �J������X����
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y�����ׂ͒�
		camX.Normalize();	// ���K��

		// �����L�[�����ƂɁA�ړ�����������
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

		// ���͂����鎞
		if (vDir.LengthSquared() > 0) {
			vDir.Normalize();

			// �L�����̍s��
			KdMatrix m = owner->GetMatrix();

			// �L�����̌���(Z)
			KdVec3 vZ = m.GetZAxis();
			vZ.Normalize();	// ���K��

			// �p�x�����߂�
			const float maxAng = 10;
			float dot = vZ.Dot(vDir);
			dot = std::min(1.0f, dot);// if(dot > 1.0f)dot = 1.0f;
			dot = std::max(-1.0f, dot);
			// �p�x����
			float ang = DirectX::XMConvertToDegrees(acos(dot));
			if (ang > maxAng) {
				ang = maxAng;
			}
			// vZ��ang������]
			// �O�ςŉ�]�����Z�o
			KdVec3 crs = vZ.Cross(vDir);
			//float L = crs.Length();
			// �����x�N�g�������߂��Ȃ��Ƃ��́A
			// �K����(0, 1, 0)�Ƃ���B
			if (crs.LengthSquared() <= 0.0001f) {
				crs.Set(0, 1, 0);
			}
			crs.Normalize();

			// crs�����̉�]�s������߂�
			KdMatrix mR;
			mR.CreateRotateAxis(crs, ang);
			// vZ��mR�ŕϊ�
			vZ.TransformNormal(mR);
			// vZ�̕����Ɍ�����
			m.LookTo(vZ, { 0, 1, 0 });

			// �ړ�
			m.Move(vDir*0.05f);
			owner->SetMatrix(m, false);
		}
	}

	// �n��H
	if (m_pChara->m_isSky == false) {
		// �A�j���ύX
		//model->GetAnimator().ChangeAnimeSmooth("���n", false, 1);
		// �s���؂�ւ�
		return std::make_shared<Landing>();
	}

	// �d��
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// �͈ړ�
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Landing::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pChara->GetOwner();

	// ModelComponent����������
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	//// �A�j���I��
	//if (model->GetAnimator().IsAnimationEnd()) {
	//	// �A�j���ύX
	//	model->GetAnimator().ChangeAnimeSmooth("����", true, 0.05f);
	//	// �s���؂�ւ�
	//	return std::make_shared<Stand>();
	//}

	// �s���؂�ւ�
	return std::make_shared<Stand>();

	// �d��
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// �͈ړ�
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}