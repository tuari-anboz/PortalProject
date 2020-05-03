#pragma once

// �p�����N���X�̃w�b�_�[���C���N���[�h
#include "BaseComponent.h"

//=============================
// �ˌ��R���|�[�l���g
//=============================
// ***************************
// �K�v�Ȏ�
// �E�Ή�����^�O��ImGui��Ō��߂��邱��
// �E�˒���������ImGui��Ō��߂��邱��
// �E���������Ƃ��Ɏ��s����֐��̓����o�Ɏ����Ă����H
// �E��������ImGui�ƃ\�[�X���Ō��߂��邱��

class ShotComponent : public BaseComponent
{
public:

	//=============================
	// ����
	//=============================

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw() override;

	// (Editor)GUI����
	virtual void ImGuiUpdate() override;

	// (Editor�p)�`��
	virtual void EditorDraw() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;

	//=============================
	// �A�N�Z�T
	//=============================
	void SetDir(const KdVec3& dir) { m_dir = dir; }
	void AddFunction();

private:
	// �˒�
	float m_range;

	// ����
	KdVec3 m_dir;
	// ���������ʒu
	KdVec3 m_hitPos;
	// ���˂����ʒu
	KdVec3 m_shotPos;
	// �^�O
	std::string m_tag;
	// ���s�֐�
	std::function <void(void)> m_function;
};