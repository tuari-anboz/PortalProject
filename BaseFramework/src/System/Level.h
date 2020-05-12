#pragma once

//===============================
//
// Level�N���X
// �E�P�̃V�[���ƂȂ�N���X
//
//===============================
class Level {
public:
	// �R���X�g���N�^
	Level();

	// Serialize/Deserialize
	void Deserialize(const json11::Json& jsonObj);
	void Serialize(json11::Json::object& outJson);
	
	// �t�@�C������
	void Load( const std::string& filename );
	void Save();

	void Update();
	void Draw();

	// Editor����
	void EditorUpdate();

	// ImGui����
	void ImGuiUpdate();

	// �I�u�W�F�N�g�ǉ�
//	void AddObject(SPtr<Object> obj) {
//		m_objectList.push_back(obj);
//	}

	// Root�I�u�W�F�N�g�̎擾
	const SPtr<Object>& GetRoot() { return m_root; }

private:
	// �t�@�C����
	std::string			m_levelFilename;

	// �I�u�W�F�N�g���X�g
//	std::list< SPtr<Object> >	m_objectList;

	// Root�I�u�W�F�N�g
	SPtr<Object> m_root;

	//-------------------------
	// �O���t�B�b�N�ݒ�
	//-------------------------
	// IBL�e�N�X�`��
	SPtr<KdTexture> m_texIBL;

	// �t�H�O�ݒ�
	KdVec3 m_fogColor = { 1,1,1 };
	float m_fogDensity = 0.02f;

	// Work�����_�[�^�[�Q�b�g(shared_ptr�ō��ׂ�)
	KdTexture		m_rtWork[2];		// ��ƃe�N�X�`��
	KdTexture       m_rtHighBrightness;		// ���P�x���o�e�N�X�`��
	BlurTexture		m_blurTex;		// �u���[�e�N�X�`��
};

