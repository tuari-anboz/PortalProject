#include "main.h"
#include "EditorCamera.h"

void EditorCamera::Update()
{
	//============================
	// 射影行列
	//============================
	ShMgr.m_mProj.CreatePerspectiveFovLH(60, KD3D.GetBackBuffer()->GetAspect(), 0.01f, 1000);


	//============================
	// ビュー行列
	//============================
	POINT nowPos;
	GetCursorPos(&nowPos);

	// Shiftで大きく
	float ratio = 1.0f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		ratio = 5;
	}
	// Controlで小さく
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		ratio = 0.2f;
	}

	// マウスの右ボタン
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
		// マウスの前回からの移動量
		float deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.5f * ratio;
		float deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.5f * ratio;

		// カメラ回転 Y軸回転
		KdMatrix mR;
		mR.CreateRotateY(deltaX);
		m_mCam *= mR;

		// カメラ回転 X軸回転
		mR.CreateRotateAxis(m_mCam.GetXAxis(), deltaY);
		m_mCam *= mR;
	}

	// ホイールドラッグ
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
		float deltaX = (float)(nowPos.x - m_PrevMousePos.x) * 0.05f * ratio;
		float deltaY = (float)(nowPos.y - m_PrevMousePos.y) * 0.05f * ratio;

		m_vCamBasePos -= m_mCam.GetXAxis() * deltaX;
		m_vCamBasePos += m_mCam.GetYAxis() * deltaY;
	}

	// ホイール
	if (APP.m_Window.GetMouseWheelVal()) {
		KdMatrix mT;
		mT.CreateMove(0, 0, (float)APP.m_Window.GetMouseWheelVal() * 0.01f * ratio);

		m_mCam = mT * m_mCam;
	}

	m_PrevMousePos = nowPos;
}

void EditorCamera::SetToShader()
{
	// カメラ行列からビュー行列へ変換
	ShMgr.m_mView = m_mCam;
	ShMgr.m_mView.Move(m_vCamBasePos);
	ShMgr.m_mView.Inverse();

	// カメラ情報(ビュー行列、射影行列)を、各シェーダの定数バッファへセット
	ShMgr.UpdateCamera();
}
