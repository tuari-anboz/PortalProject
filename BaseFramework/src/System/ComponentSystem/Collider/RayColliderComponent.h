#pragma once

//==============================================
//
// ���CCollider �R���|�[�l���g
//
//==============================================
class RayColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̊֐�
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ���
	virtual Types GetType() const override { return Types::Ray; }

	// �����蔻��@���� vs obj
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// �f�o�b�O�`��
	virtual void DebugDraw(const KdVec4& color) override;


	// ���C���W�擾
	const KdVec3& GetRayPos() const { return m_rayPos; }
	// ���C�����擾
	const KdVec3& GetRayDir() const { return m_rayDir; }
	// ���C�̒����擾
	float GetRayLen() const { return m_rayLen; }

	// ���C�̃��[�J�����W���擾
	const KdVec3& GetRayLocalPos() const { return m_localPos; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, const KdVec3& dir, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_rayPos = m_localPos = pos;
		m_rayDir = m_localDir = dir;
		m_rayLen = dir.Length();

		m_rayDir.Normalize();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ��������ʏ�R���|�[�l���g�̊֐��I�o�[���C�h
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
		m_localDir.Deserialize(jsonObj["LocalDir"]);
		m_rayLen = (float)jsonObj["Len"].number_value();
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["LocalDir"] = m_localDir.Serialize();
		outJsonObj["RayLen"] = m_rayLen;
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

	// ���C�ݒ�f�[�^
	KdVec3	m_localPos;		// ���[�J�����W
	KdVec3	m_localDir;		// ���[�J���x�N�g��(�������g�p)

	// ���ۂɔ���Ŏg�p���郌�C�f�[�^(m_localPos��m_localDir��Transform�̍s��ŕϊ���������)
	KdVec3	m_rayPos;		// �ʒu
	KdVec3	m_rayDir;		// ����(���K��)
	float	m_rayLen;		// ���C�̒���
};
