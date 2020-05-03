// BaseComponent.h
#pragma once

class Object;

//====================================
// �R���|�[�l���g��{�N���X
//====================================
class BaseComponent : public std::enable_shared_from_this<BaseComponent>{
public:
	virtual ~BaseComponent() {}

	// �N���X���擾
	std::string ClassName() {
		return &typeid(*this).name()[6];	// �����ˑ�
	}

	// ������s����
	virtual void Start() {}

	void CallStart()
	{
		if (m_isStart) {
			Start();
			m_isStart = false;
		}
	}

	// �X�V����
	virtual void Update() {}
	// �`��O����
	virtual void PreDraw() {}
	// �`�揈��
	virtual void Draw() {}
	// �G�t�F�N�g�n�̕`��
	virtual void DrawEffect() {}
	// 3D�������`�揈��
	virtual void DrawTransparency() {}
	// �V���h�E�}�b�v�`�揈��
	virtual void DrawShadowMap() {}

	// 2D�`�揈��
	virtual void DrawSprite() {}

	// (Editor)ImGui����
	virtual void ImGuiUpdate() {
		// �L��/����
		ImGui::Checkbox("Enable", &m_enable);
	}

	// (Editor)�G�f�B�^�[�`��
	virtual void EditorDraw() {}

	// �P���
	void MoveUp();
	// �P����
	void MoveDown();

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonObj) {
		m_enable = jsonObj["Enable"].bool_value();
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJson) {
		outJson["ClassName"] = ClassName();
		outJson["Enable"] = m_enable;
	}

	void Delete() { m_isDelete = true; }
	bool IsDelete() { return m_isDelete; }

	void SetOwner(Object* owner) {
		m_pOwner = owner;
	}
	Object* GetOwner() { return m_pOwner; }

	// �L���t���O�Z�b�g
	virtual void SetEnable(bool enable) {
		m_enable = enable;
	}
	// �L���t���O�擾
	bool GetEnable() const { return m_enable; }

protected:
	// �L��/�����t���O
	bool	m_enable = true;
	// �폜�t���O
	bool	m_isDelete = false;
	// ������ւ̃A�h���X
	Object*	m_pOwner = nullptr;
	// ����H
	bool	m_isStart = true;
};
