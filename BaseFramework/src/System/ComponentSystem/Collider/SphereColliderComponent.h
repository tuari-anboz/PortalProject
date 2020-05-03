#pragma once

//==============================================
//
// ��Collider �R���|�[�l���g
//
//==============================================
class SphereColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̊֐�
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ���
	virtual Types GetType() const override { return Types::Sphere; }

	// �����蔻����s�@���� vs obj
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// �f�o�b�O�`��
	virtual void DebugDraw(const KdVec4& color) override;

	// ������f�[�^�擾
	const DirectX::BoundingSphere& GetSphere() const { return m_sphere; }

	const KdVec3& GetLocalPos() const { return m_localPos; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, float radius, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localPos = pos;
		m_sphere.Center = pos;
		m_radius = radius;
		m_sphere.Radius = radius;
	}


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	//  ��������ʏ�R���|�[�l���g�̊֐��I�[�o�[���C�h
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//----------------------------
	// �X�V����
	//----------------------------
	virtual void Update() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseColliderComponent::Deserialize(jsonObj);

		m_localPos.Deserialize(jsonObj["LocalPos"]);
		m_radius = (float)jsonObj["Radius"].number_value();
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["Radius"] = m_radius;
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

	// ���ݒ�f�[�^
	KdVec3	m_localPos;		// ���[�J�����W
	float	m_radius = 1;	// ���a

	// ����p���f�[�^�@���ۂ̂����蔻��Ŏg�p�����f�[�^(m_localPos��m_radius��Transform�̍s��ŕϊ���������)
	DirectX::BoundingSphere			m_sphere;

};