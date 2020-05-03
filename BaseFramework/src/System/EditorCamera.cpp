#include "main.h"
#include "EditorCamera.h"

void EditorCamera::Update()
{
	//============================
	// �ˉe�s��
	//============================
	ShMgr.m_mProj.CreatePerspectiveFovLH(60, KD3D.GetBackBuffer()->GetAspect(), 0.01f, 1000);


	//============================
	// �r���[�s��
	//============================
	POINT nowPos;
	GetCursorPos(&nowPos);

	// Shift�ő傫��
	float ratio = 1.0f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		ratio = 5;
	}
	// Control�ŏ�����
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		ratio = 0.2f;
	}

	// �}�E�X�̉E�{�^��
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
		// �}�E�X�̑O�񂩂�̈ړ���
		float deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.5f * ratio;
		float deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.5f * ratio;

		// �J������] Y����]
		KdMatrix mR;
		mR.CreateRotateY(deltaX);
		m_mCam *= mR;

		// �J������] X����]
		mR.CreateRotateAxis(m_mCam.GetXAxis(), deltaY);
		m_mCam *= mR;
	}

	// �z�C�[���h���b�O
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
		float deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.05f * ratio;
		float deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.05f * ratio;

		m_vCamBasePos -= m_mCam.GetXAxis() * deltaX;
		m_vCamBasePos += m_mCam.GetYAxis() * deltaY;
	}

	// �z�C�[��
	if (APP.m_Window.GetMouseWheelVal()) {
		KdMatrix mT;
		mT.CreateMove(0, 0, (float)APP.m_Window.GetMouseWheelVal() * 0.01f * ratio);

		m_mCam = mT * m_mCam;
	}

	m_PrevMousePos = nowPos;
}

void EditorCamera::SetToShader()
{
	// �J�����s�񂩂�r���[�s��֕ϊ�
	ShMgr.m_mView = m_mCam;
	ShMgr.m_mView.Move(m_vCamBasePos);
	ShMgr.m_mView.Inverse();

	// �J�������(�r���[�s��A�ˉe�s��)���A�e�V�F�[�_�̒萔�o�b�t�@�փZ�b�g
	ShMgr.UpdateCamera();
}
