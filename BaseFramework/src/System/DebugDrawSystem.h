#pragma once

//================================================
//
// �f�o�b�O�`��V�X�e��
//
//================================================
class DebugDrawSystem {
public:

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// ���`��
	// �Epos1		: �J�n���W
	// �Epos2		: �I�����W
	// �Ecolor		: �F
	// �Emat		: �ϊ��s��
	// �Eduration	: �\������
	void Line(const KdVec3& pos1, const KdVec3& pos2, const KdVec4& color, const KdMatrix& mat, int duration);

	// ���̐��`��
	// �Epos		: ���W
	// �Eradius		: ���a
	// �Ecolor		: �F
	// �Emat		: �ϊ��s��
	// �Eduration	: �\������
	void SphereLine(const KdVec3& pos, float radius, const KdVec4& color, const KdMatrix& mat, int duration);

	// ���̐��`��
	// �EvLocalCenter	: ���W(���[�J��)
	// �EvHalfSize		: �T�C�Y(���[�J��)
	// �Ecolor			: �F
	// �Emat			: �ϊ��s��
	// �Eduration		: �\������
	void BoxLine(const KdVec3& vLocalCenter, const KdVec3& vHalfSize, const KdVec4& color, const KdMatrix& mat, int duration);

	// 2D�e�L�X�g��\��(3D���W����2D�֕ϊ����܂�)
	// �Etext			: �\��������
	// �Epos3D			: 3D���W(2D�֕ϊ������)
	// �Ecolor			: �\���F
	// �Eduration		: �\������
	void Text(const std::string& text, const KdVec3& pos3D, const KdVec4& color, int duration);

private:

	struct Node {
		std::function<void()>	proc;
		int						lifespan = 1;
	};

	std::vector< Node >	m_Nodes;

	std::vector< Node >	m_TextNodes;

//============================================================
// �V���O���g��
//============================================================
private:
	DebugDrawSystem() {
	}
public:
	static DebugDrawSystem &GetInstance(){
		static DebugDrawSystem Instance;
		return Instance;
	}


};

#define DebugDrawSys DebugDrawSystem::GetInstance()

