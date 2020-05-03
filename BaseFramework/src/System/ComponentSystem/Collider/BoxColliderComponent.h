#pragma once

//==============================================
//
// ��Collider�R���|�[�l���g
//
//==============================================
class BoxColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̊֐�
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ��ނ��擾
	virtual Types GetType() const override { return Types::Box; }

	// �����蔻��
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// �f�o�b�O�`��
	virtual void DebugDraw(const KdVec4& color) override;

	// ������f�[�^�擾
	const DirectX::BoundingOrientedBox& GetBox() const { return m_box; }
	// ����p��]�s��擾
	const KdMatrix& GetRotaMat() const { return m_rotaMat; }

	const KdVec3& GetLocalPos() const { return m_localPos; }
	const KdVec3& GetLocalSize() const { return m_localSize; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, const KdVec3& size, const KdMatrix& rotaMat, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localPos = pos;
		m_box.Center = pos;

		m_localSize = size;
		m_box.Extents = size;

		// ��]�݂̂̍s��
		m_rotaMat.SetRotation(rotaMat);
		m_rotaMat.NormalizeScale();

		// ��]���N�H�[�^�j�I���ɕϊ�
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}

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

		m_localPos.Deserialize(jsonObj["LocalPos"]);
		m_localSize.Deserialize(jsonObj["LocalSize"]);
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["LocalSize"] = m_localSize.Serialize();
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// �R���C�_�[�̃f�[�^
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ���ݒ�f�[�^�@�������牺�L��m_box��m_rotaMat�𖈃t���[���v�Z����
	KdVec3	m_localPos;
	KdVec3	m_localSize = {1, 1, 1};

	// �����蔻��p���f�[�^�@���ۂ̂����蔻��Ŏg�p�����f�[�^(m_localPos��m_localSize��Transform�̍s��ŕϊ���������)
	DirectX::BoundingOrientedBox	m_box;
	KdMatrix						m_rotaMat;	// ��]�݂̂̍s��(�e���͐��K��)

};
