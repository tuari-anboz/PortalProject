#include "main.h"

#include "../GameManager.h"

#include "CameraComponent.h"

//==============================================
//
// �J�����R���|�[�l���g
//
//==============================================

void CameraComponent::PreDraw()
{
	if (m_enable == false)return;
	// ��A�N�e�B�u�J�����̓V�F�[�_�փZ�b�g���Ȃ�
	if (m_active == false)return;
	// Editor�p
	if (GameMgr.m_Editor_UseCameraComponent == false)return;

	//============================
	// �ˉe�s��
	//============================
	ShMgr.m_mProj.CreatePerspectiveFovLH(m_fov, m_aspect, m_nearFarDist.x, m_nearFarDist.y);
	

	//============================
	// �r���[�s��
	//============================
	ShMgr.m_mView = GetOwner()->GetMatrix();
	ShMgr.m_mView.NormalizeScale();
	ShMgr.m_mView.Inverse();

	// �J�������(�r���[�s��A�ˉe�s��)���A�e�V�F�[�_�̒萔�o�b�t�@�փZ�b�g
	ShMgr.UpdateCamera();
}

void CameraComponent::EditorDraw()
{
	if (m_enable == false)return;


	// �ˉe�s��
	KdMatrix mProj;
	mProj.CreatePerspectiveFovLH(m_fov, m_aspect, m_nearFarDist.x, m_nearFarDist.y);
	// �r���[�s��
	KdMatrix mView;
	mView = GetOwner()->GetMatrix();
	mView.NormalizeScale();
	mView.Inverse();

	// �r���[�Ǝˉe�̋t�s�񂩂�A������̊e���_�����߂�
	KdMatrix mInvVP = mView * mProj;
	mInvVP.Inverse();

	KdVec3 points[8] = {
		{-1, -1, 0},
		{-1,  1, 0},
		{ 1,  1, 0},
		{ 1, -1, 0},

		{-1, -1, 1},
		{-1,  1, 1},
		{ 1,  1, 1},
		{ 1, -1, 1},
	};
	for (auto&& v : points) {
		v.Transform(mInvVP);
	}

	// 
	ShMgr.m_KdPrimSh.DrawLine(points[0], points[4], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[1], points[5], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[2], points[6], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[3], points[7], { 1, 1, 0, 1 });

	// �ŋߐڋ���
	ShMgr.m_KdPrimSh.DrawLine(points[0], points[1], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[1], points[2], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[2], points[3], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[3], points[0], { 1, 1, 0, 1 });

	// �ŉ�������
	ShMgr.m_KdPrimSh.DrawLine(points[4], points[5], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[5], points[6], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[6], points[7], { 1, 1, 0, 1 });
	ShMgr.m_KdPrimSh.DrawLine(points[7], points[4], { 1, 1, 0, 1 });
}

void CameraComponent::Deserialize(const json11::Json & jsonObj)
{
	// �p�����̃f�[�^���f�V���A���C�Y
	BaseComponent::Deserialize(jsonObj);

	m_fov = (float)jsonObj["FOV"].number_value();
	m_nearFarDist.Deserialize(jsonObj["NearFarDist"]);
	m_attachToParent = jsonObj["AttachToParent"].bool_value();
	m_attachDistance = (float)jsonObj["AttachDistance"].number_value();
	m_attachOffset.Deserialize(jsonObj["AttachOffset"]);
	m_active = jsonObj["Active"].bool_value();
}

void CameraComponent::Serialize(json11::Json::object & outJsonObj)
{
	// �p�����̃f�[�^���V���A���C�Y
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["FOV"] = m_fov;
	outJsonObj["NearFarDist"] = m_nearFarDist.Serialize();
	outJsonObj["AttachToParent"] = m_attachToParent;
	outJsonObj["AttachDistance"] = m_attachDistance;
	outJsonObj["AttachOffset"] = m_attachOffset.Serialize();
	outJsonObj["Active"] = m_active;
}

void CameraComponent::ImGuiUpdate()
{
	// �p������ImGui����
	BaseComponent::ImGuiUpdate();

	// �A�N�e�B�u���
	ImGui::Checkbox(u8"�`��Ŏg�p����", &m_active);

	// ����p
	ImGui::DragFloat(u8"FOV", &m_fov, 1, 1, 179);

	// ����̋���
	ImGui::DragFloat2("Near and Far", m_nearFarDist, 0.01f, 0.001f, 10000);
	if (m_nearFarDist.x < 0.0001f) {
		m_nearFarDist.x = 0.0001f;
	}

	// �e�Ɏ��t��
	ImGui::Indent();
	if (ImGui::CollapsingHeader(u8"�eObject���t��"))
	{
		ImGui::Checkbox(u8"�e�Ɏ��t��", &m_attachToParent);
		ImGui::DragFloat(u8"����", &m_attachDistance, 0.01f, 0, 100);
		ImGui::DragFloat3(u8"Offset", m_attachOffset, 0.01f);
	}
	ImGui::Unindent();
}
