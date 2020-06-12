#include "main.h"
#include "GameManager.h"
#include "LevelEditor.h"

void LevelEditor::Init()
{
	// �����s��
	m_as = std::make_shared<Basic>();
}

void LevelEditor::Update()
{
	// �s������
	if (m_as) 
	{
		// ������̃L�����̃A�h���X�����Ă���
		m_as->m_pObject = this;
		// �s���������s
		auto p = m_as->Update();
		if (p)
		{
			m_as = p;
		}
	}
}

void LevelEditor::ImGuiUpdate()
{
	// ���f���t�@�C��
	ImGuiResourceButton(u8"��������t�@�C����", m_generationPrefabName, { ".pref" });
}

void LevelEditor::Deserialize(const json11::Json& jsonObj)
{

}

void LevelEditor::Serialize(json11::Json::object& outJsonObj)
{

}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Basic::Update()
{
	// �I�𒆂̃I�u�W�F�N�g������ꍇ�I���s����
	if (GameMgr.GetSelectObj().lock())return std::make_shared<Selected>();



	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Selected::Update()
{
	// �I�𒆂̃I�u�W�F�N�g���Ȃ��ꍇ��{�s����
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// C�L�[�ō쐬���[�h��
	if (KdIsKeyDown('C',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"�쐬���[�h�ֈڍs");
		return std::make_shared<Create>();
	}

	// T�L�[�Œu�����[�h��
	if (KdIsKeyDown('T',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"�u�����[�h�ֈڍs");
		return std::make_shared<Replace>();
	}

	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Create::Update()
{
	// �I�𒆂̃I�u�W�F�N�g�Z�b�g���Ȃ��ꍇ��{�s����
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// �ēxC�L�[�őI�����[�h��
	if (KdIsKeyDown('C', false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"�I�����[�h�ֈڍs");
		return std::make_shared<Selected>();
	}

	//// �I�����Ă���v���n�u�t�@�C�����Ȃ��ꍇ
	//if (m_pObject->m_isCopy == false)
	//{
	//	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();
	//}

	// DELETE�L�[�ŃI�u�W�F�N�g���폜
	//if ()
	//{
	//	m_pObject->m_selectedObjectSet.lock()->Delete();
	//	m_pObject->m_selectedObjectSet.reset();
	//	GameMgr.m_Editor_Log.AddLog(u8"�I�������I�u�W�F�N�g�Z�b�g���폜���܂���");
	//}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown(VK_RIGHT,false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		//else
		//{
		//	newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		//	auto parent = GameMgr.GetSelectObj().lock()->GetParent();
		//	newObj->SetParent(parent);

		//	auto obj = GameMgr.GetSelectObj().lock();
		//	KdMatrix mSelectedObj = obj->GetMatrix();
		//	KdMatrix mGeneratedObj = mSelectedObj;

		//	mGeneratedObj.Move(2.0f, 0, 0);

		//	newObj->SetMatrix(mGeneratedObj, true);
		//}
		GameMgr.SetSelectObj(newObj);
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown(VK_LEFT, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown('Q', false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown('Z', false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown(VK_UP, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	// �I�������I�u�W�F�N�g�Z�b�g�̏㉺���E�ɐ�������
	if (KdIsKeyDown(VK_DOWN, false))
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(GameMgr.GetSelectObj().lock());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		GameMgr.SetSelectObj(newObj);
	}

	return nullptr;
}

SPtr<LevelEditor::BaseLevelEditorAction> LevelEditor::Replace::Update()
{
	// �I�𒆂̃I�u�W�F�N�g�Z�b�g���Ȃ��ꍇ��{�s����
	if (GameMgr.GetSelectObj().expired())return std::make_shared<Basic>();

	// �I�����Ă���v���n�u�t�@�C�����Ȃ��ꍇ
	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();

	// T�L�[�őI�����[�h��
	if (KdIsKeyDown('T', false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"�I�����[�h�ֈڍs");
		return std::make_shared<Selected>();
	}


	// O�L�[�݂̂Ȃ�I�𒆂̃I�u�W�F�N�g�̂�
	// SHIFT�ƈꏏ�ɉ����ƑI�𒆂̃I�u�W�F�N�g�Ɠ������O�̃I�u�W�F�N�g���S�Ēu������
	if (KdIsKeyDown('O') && KdIsKeyDown(VK_LSHIFT,true))
	{
		auto objects = GameMgr.m_level->GetRoot()->FindObjects(GameMgr.GetSelectObj().lock()->GetName());
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
	else if (KdIsKeyDown('P',false))
	{
		GameMgr.m_Editor_Log.AddLog(u8"�I�u�W�F�N�g��u�����܂���");
		auto selectedObj = GameMgr.GetSelectObj().lock();
		auto parent = selectedObj->GetParent();
		KdMatrix mObj = selectedObj->GetMatrix();
		auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		ReplaceObj->SetMatrix(mObj, true);
		selectedObj->Delete();
		ReplaceObj->SetParent(parent);
	}

	return nullptr;
}


