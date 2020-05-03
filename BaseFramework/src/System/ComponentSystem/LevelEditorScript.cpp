#include "main.h"

#include "../GameManager.h"

#include "LevelEditorScript.h"

void LevelEditorScript::Start()
{
	// �����s��
	m_as = std::make_shared<Basic>();
}

void LevelEditorScript::Update()
{
	if (m_enable == false)return;

	// �I�[�i�[�擾
	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return;

	// �s������
	if (m_as) {
		// ������̃L�����̃A�h���X�����Ă���
		m_as->m_pObject = this;
		// �s���������s
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}
}

void LevelEditorScript::ImGuiUpdate()
{
	// Object�̂ق���ImGuiUpdate�����s
	BaseComponent::ImGuiUpdate();

	// �R�s�[
	ImGui::Checkbox("IsCopy", &m_isCopy);
	// ��������v���n�u�t�@�C��
	ImGuiResourceButton("Prefab File", m_generationPrefabName, { ".pref" });
	// �v���C���[�v���n�u�t�@�C��
	ImGuiResourceButton("PlayerPrefab", m_playerPrefabName, { ".pref" });
}

void LevelEditorScript::Deserialize(const json11::Json & jsonObj)
{
	// �p�����̊֐������s���Ă���
	BaseComponent::Deserialize(jsonObj);

	m_isCopy = jsonObj["IsCopy"].bool_value();
	m_generationPrefabName = jsonObj["PrefabName"].string_value();
	m_playerPrefabName = jsonObj["PlayerPrefabName"].string_value();
}

void LevelEditorScript::Serialize(json11::Json::object & outJsonObj)
{
	// �p�����̊֐������s���Ă���
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["IsCopy"] = m_isCopy;
	outJsonObj["PrefabName"] = m_generationPrefabName;
	outJsonObj["PlayerPrefabName"] = m_playerPrefabName;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Basic::Update()
{
	// �I�[�i�[�擾
	auto owner = m_pObject->GetOwner();

	// ���̓R���|�[�l���g�擾
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return nullptr;
	// �J�����I�u�W�F�N�g�擾
	auto cam = owner->FindObject("Cam");
	if (cam == nullptr)return nullptr;

	// �G�f�B�^�[���[�h�I��
	if (input->m_keysRepeat[VK_SHIFT] && input->m_keys['P'])
	{
		if (m_pObject->m_playerPrefabName.empty())return nullptr;
		// �v���C���[�̏����|�W���擾
		auto startObj = GameMgr.m_level->GetRoot()->FindObject("StartPoint");
		if (startObj == false)return nullptr;
		// pref�t�@�C������v���C���[�𐶐�
		auto playerPref = KDResFactory.GetJson(m_pObject->m_playerPrefabName);
		if (playerPref.NUL)return nullptr;
		auto playerObj = GameMgr.Instantiate(playerPref);
		if (playerObj == false)return nullptr;
		// �v���C���[�������|�W�ɃZ�b�g���ăG�f�B�^�[���[�h���I������
		KdMatrix mPos = startObj->GetMatrix();
		playerObj->SetMatrix(mPos, true);
		playerObj->SetParent(GameMgr.m_level->GetRoot());
		owner->Delete();
		GameMgr.m_Editor_Log.AddLog(u8"�G�f�B�^�[���[�h�I��");
		return nullptr;
	}

	// �I�𒆂̃I�u�W�F�N�g�Z�b�g������ΑI�𒆃��[�h�ֈڍs
	if (m_pObject->m_selectedObjectSet.expired() == false)
	{
		return std::make_shared<Selected>();
	}

	KdVec3 vDir;
	if (cam) 
	{
		//�J������Z����
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y�����ׂ͒�
		camZ.Normalize();	// ���K��

		// �J������X����
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y�����ׂ͒�
		camX.Normalize();	// ���K��

		// �J������Y����

		KdVec3 moveDir;

		if (input->m_keysRepeat['W'])
		{
			moveDir.z += 1.0f;
		}
		if (input->m_keysRepeat['A'])
		{
			moveDir.x -= 1.0f;
		}
		if (input->m_keysRepeat['S'])
		{
			moveDir.z -= 1.0f;
		}
		if (input->m_keysRepeat['D'])
		{
			moveDir.x += 1.0f;
		}
		if (input->m_keysRepeat['Q'])
		{
			moveDir.y += 1.0f;
		}
		if (input->m_keysRepeat['Z'])
		{
			moveDir.y -= 1.0f;
		}


		// �����L�[�����ƂɁA�ړ�����������
		vDir = (camX * moveDir.x) + (camZ * moveDir.z);
		vDir.y = moveDir.y;

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
	// �}�E�X���N���b�N�őΏۂ̃I�u�W�F�N�g��I��
	if (input->m_keys[VK_LBUTTON])
	{
		// ���C����̃R���|�[�l���g���擾����
		auto coll = cam->GetComponent<RayColliderComponent>();
		coll->SetEnable(true);
		if (coll) {
			// �q�b�g���Ɏ��s�����֐���o�^
			coll->m_onHitStay = [this](BaseColliderComponent* collider)
			{
				// �S�Ă̂�������������A��ԋ߂�������o
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults())
				{
					// ���x���G�f�B�^�[�p�̃^�O�̂�
					if (res.Collider->GetTag() == "LevelEditor")
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
					m_pObject->m_selectedObjectSet = nearestRes->Collider->GetOwner()->shared_from_this()->GetParent();
					GameMgr.m_Editor_SelectObj = m_pObject->m_selectedObjectSet;
					m_pObject->m_selectedObject = nearestRes->Collider->GetOwner()->shared_from_this();
					GameMgr.m_Editor_Log.AddLog(u8"�I�����[�h�ֈڍs");
				}
				collider->SetEnable(false);
			};
		}
	}



	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Selected::Update()
{
	// �I�𒆂̃I�u�W�F�N�g�Z�b�g���Ȃ��ꍇ��{�s����
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// �I�[�i�[�擾
	auto owner = m_pObject->GetOwner();

	// ���̓R���|�[�l���g�擾
	auto input = owner->GetComponent<InputComponent>();

	// C�L�[�ō쐬���[�h��
	if (input->m_keys['C'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"�쐬���[�h�ֈڍs");
		return std::make_shared<Create>();
	}

	// R�L�[�ŕҏW���[�h��
	if (input->m_keys['R'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"�ҏW���[�h�ֈڍs");
		return std::make_shared<Edit>();
	}

	// T�L�[�Œu�����[�h��
	if (input->m_keys['T'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"�u�����[�h�ֈڍs");
		return std::make_shared<Replace>();
	}

	// �E�N���b�N�őI������
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�����������܂���");
		return std::make_shared<Basic>();
	}

	// DELETE�L�[�ŃI�u�W�F�N�g�Z�b�g���폜
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�������I�u�W�F�N�g�Z�b�g���폜���܂���");
	}

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Create::Update()
{
	// �I�𒆂̃I�u�W�F�N�g�Z�b�g���Ȃ��ꍇ��{�s����
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// �I�����Ă���v���n�u�t�@�C�����Ȃ��ꍇ
	if (m_pObject->m_isCopy == false)
	{
		if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();
	}

	// �I�[�i�[�擾
	auto owner = m_pObject->GetOwner();
	if (owner == nullptr) return nullptr;

	// ���̓R���|�[�l���g�擾
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr) return nullptr;

	// DELETE�L�[�ŃI�u�W�F�N�g���폜
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�������I�u�W�F�N�g�Z�b�g���폜���܂���");
	}

	// �E�N���b�N�őI������
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�����������܂���");
		return std::make_shared<Basic>();
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (input->m_keys[VK_RIGHT])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (input->m_keys[VK_LEFT])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (input->m_keys['Q'])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (input->m_keys['Z'])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, -2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, -2.0f, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}
	// �I�������I�u�W�F�N�g�Z�b�g�̑O��ɐ�������
	if (input->m_keys[VK_UP])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}
	// �I�������I�u�W�F�N�g�Z�b�g�̑O��ɐ�������
	if (input->m_keys[VK_DOWN])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, -2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 0, -2.0f);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Edit::Update()
{

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Replace::Update()
{
	// �I�𒆂̃I�u�W�F�N�g�Z�b�g���Ȃ��ꍇ��{�s����
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// �I�����Ă���v���n�u�t�@�C�����Ȃ��ꍇ
	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();

	// �I�[�i�[�擾
	auto owner = m_pObject->GetOwner();
	if (owner == nullptr) return nullptr;

	// ���̓R���|�[�l���g�擾
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr) return nullptr;

	// DELETE�L�[�ŃI�u�W�F�N�g�Z�b�g���폜
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�������I�u�W�F�N�g�Z�b�g���폜���܂���");
	}

	// �E�N���b�N�őI������
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"�I�����������܂���");
		return std::make_shared<Basic>();
	}

	// O�L�[�݂̂Ȃ�I�𒆂̃I�u�W�F�N�g�̂�
	// SHIFT�ƈꏏ�ɉ����ƑI�𒆂̃I�u�W�F�N�g�Ɠ������O�̃I�u�W�F�N�g���S�Ēu������
	if (input->m_keys['O'] && input->m_keysRepeat[VK_LSHIFT])
	{
		auto objects = GameMgr.m_level->GetRoot()->FindObjects(m_pObject->m_selectedObject.lock()->GetName());
		for (auto && obj : objects)
		{
			GameMgr.m_Editor_Log.AddLog(u8"�I�u�W�F�N�g��u�����܂���");
			auto parent = obj->GetParent();
			KdMatrix mObj = obj->GetMatrix();
			auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			ReplaceObj->SetMatrix(mObj, true);
			obj->Delete();
			ReplaceObj->SetParent(parent);
		}
	}
	else if (input->m_keys['O'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"�I�u�W�F�N�g��u�����܂���");
		auto selectedObj = m_pObject->m_selectedObject.lock();
		auto parent = selectedObj->GetParent();
		KdMatrix mObj = selectedObj->GetMatrix();
		auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		ReplaceObj->SetMatrix(mObj, true);
		selectedObj->Delete();
		ReplaceObj->SetParent(parent);
	}

	return nullptr;
}

