#pragma once



//==============================================
//
// ���b�V��Collider�R���|�[�l���g
//
//==============================================
class MeshColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̊֐�
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ���
	virtual Types GetType() const override { return Types::Mesh; }

	// �����蔻��
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// �f�o�b�O�`��
	virtual void DebugDraw(const KdVec4& color) override;

	// �擾
	const SPtr<KdGameModel>& GetModel() { return m_model; }
	const KdMatrix& GetMatrix() const { return m_matrix; }
	const KdMatrix& GetInvMatrix() const { return m_invMatrix; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const std::string& filename, const KdMatrix& mat, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_filename = filename;
		m_matrix = mat;
		m_matrix.Inverse(m_invMatrix);
		LoadModel(m_filename);
	}

	//
	void LoadModel(const std::string& filename);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ��������ʏ�R���|�[�l���g�̊֐��I�[�o�[���C�h
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//----------------------------
	// ������s����
	//----------------------------
	virtual void Start() override;

	//----------------------------
	// �X�V����
	//----------------------------
	virtual void Update() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseColliderComponent::Deserialize(jsonObj);

		m_filename = jsonObj["ModelFilename"].string_value();

		// �ǂݍ���
		LoadModel(m_filename);
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_filename;
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̃f�[�^
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// �t�@�C����
	std::string			m_filename;

	// ����p�f�[�^
	SPtr<KdGameModel>	m_model;		// ���b�V��
	KdMatrix			m_matrix;		// Transform�̍s��
	KdMatrix			m_invMatrix;	// Transform�̋t�s��
};

