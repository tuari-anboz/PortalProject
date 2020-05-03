#pragma once

//======================================================
//
// �}�E�X�ł��肮�蓮������J����
//
//======================================================
class EditorCamera {
public:

	// 
	EditorCamera() {
		m_mCam.CreateMove(0, 0, -5);
		m_mCam.RotateX(20);
		m_mCam.RotateY(20);
	}

	// ����
	void Update();

	// �r���[�E�ˉe�s����Z�b�g
	void SetToShader();


	KdVec3		m_vCamBasePos;
	KdMatrix	m_mCam;

private:

	POINT		m_PrevMousePos = { 0, 0 };
};
