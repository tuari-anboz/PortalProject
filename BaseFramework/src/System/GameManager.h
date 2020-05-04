#pragma once

#include "EditorCamera.h"

// �R���|�[�l���g�n
#include "ComponentSystem/BaseComponent.h"
#include "ComponentSystem/InputComponent.h"
#include "ComponentSystem/ModelComponent.h"
#include "ComponentSystem/EffectModelComponent.h"
#include "ComponentSystem/SpriteComponent.h"
#include "ComponentSystem/CameraComponent.h"
#include "ComponentSystem/LightComponent.h"
#include "ComponentSystem/AudioComponent.h"
#include "ComponentSystem/PortalComponent.h"
#include "ComponentSystem/TitleScript.h"
#include "ComponentSystem/PlayerScript.h"
#include "ComponentSystem/FpsCameraComponent.h"
#include "ComponentSystem/LevelEditorScript.h"
#include "ComponentSystem/LoadLevelComponent.h"
#include "ComponentSystem/UIButtonComponent.h"
#include "ComponentSystem/RigidBodyComponent.h"
#include "ComponentSystem/RideButtonComponent.h"
#include "ComponentSystem/BoxGeneratorComponent.h"
#include "ComponentSystem/DoorComponent.h"
#include "ComponentSystem/ButtonPushComponent.h"
#include "ComponentSystem/FadeComponent.h"
#include "ComponentSystem/Collider/CollisionManager.h"

// �Q�[���n�R���|�[�l���g
#include "../Game/Components/AutomaticDoorComponent.h"
#include "../Game/Components/PortalGunComponent.h"
#include "../Game/Components/BoxComponent.h"
#include "../Game/Components/GettingBoxComponent.h"

// �I�u�W�F�N�g��{�N���X
class Object : public std::enable_shared_from_this<Object>
{
public:
	// ���z�f�X�g���N�^
	virtual ~Object() {}

private:
	// �X�V����
	virtual void Update() {}
	// �`��O����
	virtual void PreDraw() {}
	// �`�揈��
	virtual void Draw() {}
	// �����`�揈��
	virtual void DrawTransparency() {};
	// �V���h�E�}�b�v�`�揈��
	virtual void DrawShadowMap() {};
	//
	virtual void DrawEffect() {}
public:

	// ��������(�����Update�̒��O�Ŏ��s�����)
	virtual void Start() {}

	// �X�V����
	void UpdateObject();
	// �`��O����
	void PreDrawObject();
	// �`�揈��
	void DrawObject();
	// �s�����`�揈��
	void DrawTransparencyObject();
	// �V���h�E�}�b�v�`�揈��
	void DrawShadowMapObject();
	// �G�t�F�N�g�`�揈��
	void DrawEffectObject();
	// 2D�`��֐�
	void DrawSpriteObject()
	{
		// �S�R���|�[�l���g��EditorDraw�����s
		for (auto&& comp : m_components)
		{
			comp->DrawSprite();
		}

		// �q������
		for (auto&& child : m_childs)
		{
			child->DrawSpriteObject();
		}
	}

	// (Editor)ImGui����
	virtual void ImGuiUpdate();

	// (Editor)�G�f�B�^�[�`��
	void EditorDrawObject() {
		// �S�R���|�[�l���g��EditorDraw�����s
		for (auto&& comp : m_components) {
			comp->EditorDraw();
		}
	}

	// �N���X���𕶎���Ŏ擾
	std::string GetObjectClassName() {
		return &typeid(*this).name()[6];
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonObj);

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJson) {
		// �N���X����tyieid�͊��ˑ��ł��I
		outJson["ClassName"] = GetObjectClassName();
		// ���O
		outJson["Name"] = m_name;
		// �s��
		outJson["Matrix"] = m_mWorld.Serialize();
		// ����
		outJson["Mass"] = m_mass;
		// �L��
		outJson["Active"] = m_isActive;
		// �S�R���|�[�l���g
		json11::Json::array compAry;
		for (auto&& comp : m_components) {
			// �R���|�[�l���g���V���A���C�Y
			json11::Json::object serial;
			comp->Serialize(serial);
			// �z��֒ǉ�
			compAry.push_back(serial);
		}
		outJson["Components"] = compAry;

		// �S�Ă̎q���V���A���C�Y���A�z�񉻂���
		json11::Json::array childAry;
		for (auto&& child : m_childs)
		{
			// Object���V���A���C�Y
			json11::Json::object serial;
			child->Serialize(serial);
			// �z��ɒǉ�
			childAry.push_back(serial);
		}
		outJson["Childs"] = childAry;
	}

	//===============================
	// �R���|�[�l���g�֌W
	//===============================
	// �w�肵���^�̃R���|�[�l���g������
	// T�͂��̊֐������s����Ƃ��Ɏw�肷��
	template<class T>
	SPtr<T> GetComponent() {
		// �S�R���|�[�l���g������
		for (auto&& comp : m_components) {
			// �_�E���L���X�g��T�^�փL���X�g�����݂�
			auto p = std::dynamic_pointer_cast<T>(comp);
			// �L���X�g�ł�����A�����Ŋm��I�I
			if (p)return p;
		}
		// ������Ȃ�������Anull��Ԃ�
		return nullptr;
	}

	// �w�肵���^�̃R���|�[�l���g������
	// T�͂��̊֐������s����Ƃ��Ɏw�肷��
	template<class T>
	std::vector<SPtr<T>> GetComponents() {
		std::vector<SPtr<T>> results;
		// �S�R���|�[�l���g������
		for (auto&& comp : m_components) {
			// �_�E���L���X�g��T�^�փL���X�g�����݂�
			auto p = std::dynamic_pointer_cast<T>(comp);
			// �L���X�g�ł�����A�����Ŋm��I�I
			if (p)results.push_back(p);
		}
		// ������Ȃ�������Anull��Ԃ�
		return std::move(results);
	}

	// �R���|�[�l���g��ǉ�
	void AddComponent(const SPtr<BaseComponent>& comp)
	{
		comp->SetOwner(this);
		m_components.push_back(comp);
	}

	// �R���|�[�l���g���X�g���擾
	std::list<SPtr<BaseComponent>>& GetComponentList()
	{
		return m_components;
	}

	//===============================
	// �e�Ǝq
	//===============================
	// �q���X�g�擾
	const std::list < SPtr<Object>>& GetChilds()
	{
		return m_childs;
	}

	// �w�肵���q���폜(�����ɏ������߁A�Ăяo�����͒��ӁI)
	void RemoveChild(const SPtr<Object>& obj)
	{
		// ����
		auto it = std::find(m_childs.begin(),m_childs.end(), obj);
		// ����
		if (it != m_childs.end())
		{
			// �폜
			m_childs.erase(it);
		}
	}

	// �e���擾
	SPtr<Object> GetParent() const { return m_parent.lock(); }

	// �e��ݒ�
	void SetParent(const SPtr<Object>& parent)
	{
		// �e����E�ނ���O�ɁA������shared_ptr���L��
		SPtr<Object> spThis = shared_from_this();

		// ���݂̐eGameObject����E�ނ���
		auto nowParent = GetParent();
		if(nowParent)
		{
			nowParent->RemoveChild(spThis);
		}

		// �e�|�C���^��ύX
		m_parent = parent;

		// �V�����e�̎q�Ƃ��āA������o�^
		if (parent)
		{
			parent->m_childs.push_back(spThis);
		}
	}

	// ��v���閼�O��Object������(�ŏ��Ɍ���������̂�)
	SPtr<Object> FindObject(const std::string & name)
	{
		SPtr<Object> result;
		// �ċA���Ĕ��肷��֐�
		std::function<bool(const SPtr<Object>&)> rec = [&rec, &result, &name](const SPtr<Object>& obj)
		{
			// ����
			if (obj->m_name == name)
			{
				result = obj;
				return true;
			}

			// �q�ċA
			for (auto&& child : obj->GetChilds())
			{
				if (rec(child))return true;
			}
			return false;
		};

		// Root���猟�����Ă���
		rec(shared_from_this());

		return result;
	}
	
	// ��v���閼�O��Object���u�S�āv����
	std::vector<SPtr<Object>> FindObjects(const std::string& name)
	{
		std::vector<SPtr<Object>> results;
		// �ċA���Ĕ��肷��֐�
		std::function<void(const SPtr<Object>&)> rec = [&rec, &results, &name](const SPtr<Object>& obj)
		{
			// ����
			if (obj->m_name == name)
			{
				results.push_back(obj);
			}
			
			// �q�ċA
			for (auto&& child : obj->GetChilds())
			{
				rec(child);
			}
		};

		// Root�`�������Ă���
		rec(shared_from_this());

		return results;
	};

	// 1���
	void MoveUp()
	{
		auto parent = GetParent();
		if (parent)
		{
			auto it = std::find(parent->m_childs.begin(), parent->m_childs.end(), shared_from_this());
			if (it != parent->m_childs.end() && it != parent->m_childs.begin())
			{
				auto prev = it;
				prev--;
				std::swap(*it, *prev);
			}
		}
	}

	// 1����
	void MoveDown()
	{
		auto parent = GetParent();
		if (parent)
		{
			auto it = std::find(parent->m_childs.begin(), parent->m_childs.end(), shared_from_this());
			if (it != parent->m_childs.end())
			{
				auto next = it;
				next++;

				if (next != parent->m_childs.end())
				{
					std::swap(*it, *next);
				}
			}
		}
	}

	//===============================
	// ���̑� �ݒ�
	//===============================
	// ���O�ݒ�
	void SetName(const std::string& _name) { m_name = _name; }
	// ���O�擾
	std::string GetName() { return m_name; }
	// �s��ݒ�
	void SetMatrix(const KdMatrix& m, bool moveChild);
	// �s��擾
	const KdMatrix& GetMatrix() const { return m_mWorld; }
	// �ύX�O�s��擾
	const KdMatrix& GetPrevMatrix() const { return m_mPrevWorld; }
	// �ω��ʍs��擾
	void GetDeltaMatrix(KdMatrix& out)
	{
		KdMatrix mInv = m_mPrevWorld;
		mInv.Inverse();
		out = mInv * m_mWorld;
	}
	// ���ʎ擾
	float GetMass() const { return m_mass; }
	// �L��On/Off
	void SetActive(bool active) { m_isActive = active; }
	// �L�����H
	bool IsActive() { return m_isActive; }
	// �폜����
	void Delete() { m_isDelete = true; }
	// �폜��Ԃ��H
	bool IsDelete() { return m_isDelete; }

protected:
private:	// �s��͌p���悩������ڂ�����Ȃ��悤�ɂ���I�I

	// ���O
	std::string m_name = "No Name";
	// �s��
	KdMatrix	m_mWorld;
	KdMatrix	m_mPrevWorld;		// �ύX�O�̍s��

	// ���� 0:���S�Œ蕨 0���傫��:���I��
	float m_mass = 0;

	// �R���|�[�l���g���X�g
	std::list< SPtr<BaseComponent > >	m_components;

	// �q���X�g 
	std::list< SPtr<Object> > m_childs;
	// �e�̃|�C���^ 
	WPtr<Object>    m_parent; 

	// �e�I�u�W�F�N�g�̃{�[���Ǐ]������{�[����
	std::string m_followBoneName;

	// �폜�t���O
	bool	m_isDelete = false;

	// ����H
	bool m_isStart = true;

	// �L���H
	bool m_isActive = true;
};

#include "Level.h"

//=============================
// �Q�[���̊�ՂƂȂ�N���X
//=============================
class GameManager {
public:
	// �����ݒ�
	void Init();

	// �Q�[������
	void Run();

	// Object����
	SPtr<Object> Instantiate(const json11::Json& jsonObj);
	// Object����
	SPtr<Object> Duplicate(const SPtr<Object> obj);

	// ���x��
	SPtr<Level>		m_level;

	// �������[���h
	SPtr<KdPhysicsWorld> m_physicsWorld;

	// �t�F�[�h�C���t�F�[�h�A�E�g(true�̎��t�F�[�h�C���ς�)
	bool m_fade = false;

	// ���[�h���̃t���O(�}���`�X���b�h�ł̃��[�h��ʕ\���p)
	bool m_isLoading = false;

	//------------------------------------------
	// Editor
	//------------------------------------------
	// (Editor)�I�𕨃Z�b�g
	void SetSelectObj(const SPtr<Object>& obj)
	{
		m_Editor_JustSelected = true;
		m_Editor_BeforeSelectObj = m_Editor_SelectObj;
		m_Editor_SelectObj = obj;
	}
	// (Editor)�I�𕨃Q�b�g
	WPtr<Object> GetSelectObj()
	{
		return m_Editor_SelectObj;
	}
	// (Editor)�I�������΂���̎�
	bool m_Editor_JustSelected;
	// (Editor)�J����
	EditorCamera	m_Editor_Camera;
	// (Editor)�J�����R���|�[�l���g���g�p����
	bool m_Editor_UseCameraComponent = false;
	// (Editor)���s��
	bool m_Editor_IsPlay = false;
	// (Editor)UI�\��
	bool m_Editor_showUI = true;
	// (Editor)LevelEditor���s
	bool m_Editor_Enable_LevelEditor = false;
	// (Editor)���O�E�B���h�E
	ImGuiLogWindow m_Editor_Log;
	// (Editor)�f�o�b�O�\��
	bool m_Editor_showDebug = true;

	// �I�u�W�F�N�g�N���X�̃��X�g & ����
	std::unordered_map< std::string, std::function<SPtr<Object>()>  >
		m_objectClassMaker;

	// �R���|�[�l���g�N���X�̃��X�g & ����
	std::unordered_map< std::string, std::function<SPtr<BaseComponent>()>  >
		m_componentClassMaker;

	// Level�؂�ւ�(�\��)
	void ChangeLevel(const std::string& levelFile)
	{
		m_changeLevelFile = levelFile;
	}

	// �g�e�N�X�`�����擾
	WaveTexture&	GetWaveTex() { return m_wave; }

private:
	// ImGui����
	void ImGuiUpdate();

	// �ύX���x���t�@�C����(�\��p)
	std::string		m_changeLevelFile;

	//------------------------------------------
	// Editor
	//------------------------------------------
	// (Editor)�I��
	WPtr<Object>	m_Editor_SelectObj;
	// (Editor)�O��̑I��
	WPtr<Object>	m_Editor_BeforeSelectObj;

	// (��)�g�e�N�X�`��
	WaveTexture m_wave;

//=======================
// �V���O���g��
//=======================
private:
	GameManager() {}

public:
	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}
};

#define GameMgr GameManager::GetInstance()

// Object�N���X�o�^�p�}�N��
#define REGISTER_OBJECT_CLASS(className) \
GameMgr.m_objectClassMaker[#className] = []() { \
	return std::make_shared<className>(); \
};

// �R���|�[�l���g�N���X�o�^�p�}�N��
#define REGISTER_COMP_CLASS(className) \
GameMgr.m_componentClassMaker[#className] = []() { \
	return std::make_shared<className>(); \
};
