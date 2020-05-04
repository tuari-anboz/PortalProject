#include "main.h"

#include "../../GameManager.h"

#include "BaseColliderComponent.h"

//==============================================
//
// Base Colliderコンポーネント
//
//==============================================

void BaseColliderComponent::IgnoreMapUpdate()
{
	//--------------------------------
	// 無視マップ処理
	//--------------------------------
	for (auto it = m_ignoreMap.begin(); it != m_ignoreMap.end();) {
		// 寿命カウント
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

	// Editor時のみ && 選択物のみ
	if (GameMgr.GetSelectObj().lock().get() == GetOwner()) {
		KdStateSaver StateSaver;
		StateSaver.SaveDS();
		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareOFF_ZWriteOFF, 0);

		// デバッグ表示
		DebugDraw(KdVec4(1, 1, 1, 1));
	}
}

void BaseColliderComponent::ImGuiUpdate()
{
	BaseComponent::ImGuiUpdate();

	// Tag
	ImGuiInputString("Tag", m_tag);

	// 物理エンジンの形状として使用する
	ImGui::Checkbox(u8"Use Physics", &m_usePhysics);

	// 判定する側？
	if (ImGui::Checkbox(u8"判定する側(IsAtk)", &m_isAtk)) {
		// フィルター最適化
		m_maskFilter = 0;
		m_groupFilter = 0;
	}

	//================
	// フィルタ
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
	// グループフィルタ
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
	// マスクフィルタ
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

