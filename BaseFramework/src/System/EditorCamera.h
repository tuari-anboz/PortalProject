#pragma once

//======================================================
//
// マウスでぐりぐり動かせるカメラ
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

	// 処理
	void Update();

	// ビュー・射影行列をセット
	void SetToShader();


	KdVec3		m_vCamBasePos;
	KdMatrix	m_mCam;

private:

	POINT		m_PrevMousePos = { 0, 0 };
};
