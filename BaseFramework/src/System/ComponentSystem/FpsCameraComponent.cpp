#include "main.h"

#include "../GameManager.h"

#include "FpsCameraComponent.h"

void FpsCameraComponent::Update()
{

	// ������擾
	auto owner = GetOwner();
	if (!owner)return;

	// ������̐e���擾(���̓R���|�[�l���g�擾�̂���)
	auto parentObj = owner->GetParent();
	if (!parentObj)return;

	// �e�̓��̓R���|�[�l���g���擾
	auto input = parentObj->GetComponent<InputComponent>();
	if (!input)return;

	// �J�����R���|�[�l���g���擾
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
		// ���[�g�I�u�W�F�N�g�͕ύX�����Ȃ�
		if (parentObj == GameMgr.m_level->GetRoot()) return;
		// �J�����̕����ɐe��������
		KdVec3 camvZ = mCam.GetZAxis();
		camvZ.y = 0;		// Y������������XZ���ʏ�̃x�N�g���ɂ���
		KdMatrix m = parentObj->GetMatrix();
		m.LookAtDir(camvZ);
		parentObj->SetMatrix(m, false);
	}
}

void FpsCameraComponent::ImGuiUpdate()
{
	// �p������ImGui����
	BaseComponent::ImGuiUpdate();

	// �e�Ƃ̃����N
	ImGui::Checkbox(u8"�e�ƃ����N���Đe�̕������J�����ɒǏ]�����邩", &m_linkToParent);

	// ����̋���
	ImGui::DragFloat("����", &m_height, -100.0f, 0.001f, 10000);
}

void FpsCameraComponent::EditorDraw()
{
}

void FpsCameraComponent::Deserialize(const json11::Json & jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	m_linkToParent = jsonObj["LinkToParent"].bool_value();
	m_height = (float)jsonObj["Height"].number_value();

	// �J�����̉�]�p�x
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
