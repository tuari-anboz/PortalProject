#pragma once

// �p�����N���X�̃w�b�_�[���C���N���[�h
#include "BaseComponent.h"

// �K�v�Ȏ�
// �E���b�V���̕\�ʂɃ|�[�^���𐶐�
// �E�|�[�^���ɓ����蔻���ݒu
// �E�����蔻�肪���b�V���̖@���ƕ��s�ɂȂ�悤�ɉ�]������

//=============================
// �|�[�^���R���|�[�l���g
//=============================
class PortalComponent : public BaseComponent
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

	// �|�[�^���ւ̏��Z�b�g
	virtual void Set(const KdVec3& _raypos,const KdVec3& _pos, const KdVec3& _normal,const int _no);

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override;

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override;


private:
	// �|�[�^���̃T�C�Y
	KdVec3 m_size;

	// �|�[�^���J�����̃f�t�H���g�s��
	KdMatrix m_Portal1DefaultCamMat;
	KdMatrix m_Portal2DefaultCamMat;

	//test
	KdMatrix cm;
	KdVec3 cv;

};