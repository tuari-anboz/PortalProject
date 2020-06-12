#include "main.h"

#include "../GameManager.h"

#include "FpsCameraComponent.h"

void FpsCameraComponent::Update()
{

	// 持ち主取得
	auto owner = GetOwner();
	if (!owner)return;

	// 持ち主の親を取得(入力コンポーネント取得のため)
	auto parentObj = owner->GetParent();
	if (!parentObj)return;

	// 親の入力コンポーネントを取得
	auto input = parentObj->GetComponent<InputComponent>();
	if (!input)return;

	// カメラコンポーネントを取得
	auto camcomp = owner->GetComponent<CameraComponent>();
	if (!camcomp)return;

	KdMatrix mCam = owner->GetMatrix();

	KdVec3 vZ = mCam.GetZAxis();
	vZ.Normalize();

	KdMatrix mRX, mRY, mR;

	m_angX += input->m_deltaY;
	m_angY += input->m_deltaX;

	if (m_angX >= 80)
	{
		m_angX = 80;
	}
	if (m_angX <= -80)
	{
		m_angX = -80;
	}

	mRX.RotateX(m_angX);
	mRY.RotateY(m_angY);

	mR = mRX * mRY;

	mCam.SetRotation(mR);

	mCam.LookAtDirMin(mCam.GetZAxis());

	KdVec3 vec{ 0,m_height,0 };
	mCam.SetPos(parentObj->GetMatrix().GetPos() + vec);

	owner->SetMatrix(mCam, false);

	if (m_linkToParent)
	{
		// ルートオブジェクトは変更させない
		if (parentObj == GameMgr.m_level->GetRoot()) return;
		// カメラの方向に親を向ける
		KdVec3 camvZ = mCam.GetZAxis();
		camvZ.y = 0;		// Y成分を消してXZ平面上のベクトルにする
		KdMatrix m = parentObj->GetMatrix();
		m.LookAtDir(camvZ);
		parentObj->SetMatrix(m, false);
	}
}

void FpsCameraComponent::ImGuiUpdate()
{
	// 継承元のImGui処理
	BaseComponent::ImGuiUpdate();

	// 親とのリンク
	ImGui::Checkbox(u8"親とリンクして親の方向をカメラに追従させるか", &m_linkToParent);

	// 視野の距離
	ImGui::DragFloat("高さ", &m_height, -100.0f, 0.001f, 10000);
}

void FpsCameraComponent::EditorDraw()
{
}

void FpsCameraComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_linkToParent = jsonObj["LinkToParent"].bool_value();
	m_height = (float)jsonObj["Height"].number_value();

	// カメラの回転角度
	m_angX = (float)jsonObj["AngX"].number_value();
	m_angY = (float)jsonObj["AngY"].number_value();
}

void FpsCameraComponent::Serialize(json11::Json::object & outJson)
{
	BaseComponent::Serialize(outJson);

	outJson["LinkToParent"] = m_linkToParent;
	outJson["Height"] = m_height;
	outJson["AngX"] = m_angX;
	outJson["AngY"] = m_angY;
}
