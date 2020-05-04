#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"

//==============================================
//
// Base Collider�R���|�[�l���g
//
//==============================================

void BaseColliderComponent::IgnoreMapUpdate()
{
	//--------------------------------
	// �����}�b�v����
	//--------------------------------
	for (auto it = m_ignoreMap.begin(); it != m_ignoreMap.end();) {
		// �����J�E���g
		if ((*it).second != 0xFFFFFFFF) {
			(*it).second--;
			if ((*it).second == 0) {
				it = m_ignoreMap.erase(it);
				continue;
			}
		}

		++it;
	}
}

void BaseColliderComponent::EditorDraw()
{
	if (m_enable == false)return;

	// Editor���̂� && �I�𕨂̂�
	if (GameMgr.GetSelectObj().lock().get() == GetOwner()) {
		KdStateSaver StateSaver;
		StateSaver.SaveDS();
		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareOFF_ZWriteOFF, 0);

		// �f�o�b�O�\��
		DebugDraw(KdVec4(1, 1, 1, 1));
	}
}

void BaseColliderComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// Tag
	ImGuiInputString("Tag", m_tag);

	// �����G���W���̌`��Ƃ��Ďg�p����
	ImGui::Checkbox(u8"Use Physics", &m_usePhysics);

	// ���肷�鑤�H
	if (ImGui::Checkbox(u8"���肷�鑤(IsAtk)", &m_isAtk)) {
		// �t�B���^�[�œK��
		m_maskFilter = 0;
		m_groupFilter = 0;
	}

	//================
	// �t�B���^
	//================
	const std::vector<CollisionManager::FilterNode>* groupFilter;
	const std::vector<CollisionManager::FilterNode>* maskFilter;

	if (m_isAtk) {
		groupFilter = &ColMgr.GetAtkFilterList();
		maskFilter = &ColMgr.GetDefFilterList();
	}
	else {
		groupFilter = &ColMgr.GetDefFilterList();
		maskFilter = &ColMgr.GetAtkFilterList();
	}
	// �O���[�v�t�B���^
	ImGui::BeginChild("GroupFilter", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 200), true);
	ImGui::Text("Group Filter");
	for (auto&& node : *groupFilter) {
		ImGui::CheckboxFlags(node.name.c_str(), (UINT*)&m_groupFilter, node.flag);
	}
	if (ImGui::Button(u8"Clear")) {
		m_groupFilter = 0;
	}
	ImGui::EndChild();
	ImGui::SameLine();
	// �}�X�N�t�B���^
	ImGui::BeginChild("MaskFilter", ImVec2(0, 200), true);
	ImGui::Text("Mask Filter");
	for (auto&& node : *maskFilter) {
		ImGui::CheckboxFlags(node.name.c_str(), (UINT*)&m_maskFilter, node.flag);
	}
	if (ImGui::Button(u8"Clear")) {
		m_maskFilter = 0;
	}
	ImGui::EndChild();

}

