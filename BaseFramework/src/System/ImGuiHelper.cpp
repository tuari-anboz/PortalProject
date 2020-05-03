#include "main.h"

// ImGuizmo�p�}�j�s�����[�^�\���Ƒ���p�l���\���֐�
void ImGuizmoEditTransform(KdMatrix& mat, const KdMatrix& mView, const KdMatrix& mProj, const
	DirectX::BoundingBox* bb)
{
	// ����p�l��
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	// ���W�A��]�A�g��̐؂�ւ����W�I�{�^��
	if (ImGui::RadioButton("Pos", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)) {
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)) {
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}
	// �s�񂩂�A���W�A��]�A�g��֕ϊ�
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(mat, matrixTranslation, matrixRotation, matrixScale);
	// ����
	ImGui::DragFloat3("Pos", matrixTranslation, 0.01f);
	ImGui::DragFloat3("Rotate", matrixRotation, 1);
	ImGui::DragFloat3("Scale", matrixScale, 0.001f);
	// ���W�A��]�A�g�傩��s��֕ϊ�
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, mat);
	// ���[�J�����삩���[���h����
	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL)) {
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD)) {
			mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}
	// �X�i�b�vOn/Off
	static bool useSnap(false);
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	// �X�i�b�v�ݒ�
	KdVec3 snap;
	static KdVec3 sPosSnap = { 1,1,1 };
	static float sRotateSnap = 10;
	static float sScaleSnap = 0.1f;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = sPosSnap;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap.x = sRotateSnap;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap.x = sScaleSnap;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	// ���E�T�C�Y�ύX�g
	static bool boundSizing = false;
	ImGui::Checkbox("Bound Sizing", &boundSizing);
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	if (bb) {
		bounds[0] = bb->Center.x - bb->Extents.x;
		bounds[1] = bb->Center.y - bb->Extents.y;
		bounds[2] = bb->Center.z - bb->Extents.z;
		bounds[3] = bb->Center.x + bb->Extents.x;
		bounds[4] = bb->Center.y + bb->Extents.y;
		bounds[5] = bb->Center.z + bb->Extents.z;
	}
	// ImGuizmo
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(mView, mProj, mCurrentGizmoOperation, mCurrentGizmoMode, mat, NULL, useSnap ?
		&snap.x : NULL, boundSizing ? bounds : NULL);
}

bool ImGuiBeginPopup_ResourceBrowser(const std::string &id, std::string & inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode)
{
	// �P�̃t�H���_�f�[�^
	struct FolderNode {
		// �t�H���_�p�X
		filesystem::path m_Path;
		// �z������Ă���t�@�C��
		std::list<filesystem::path> m_Childs;
	};
	static std::vector<FolderNode> sFolderList;
	static int sSelectIndex = -1;
	//
	bool bSelected = false;
	ImGui::PushID(id.c_str());
	//----------------------------------------------
	// ���O�̃R���g���[�������N���b�N�����ƁA�t�@�C���u���E�U�[���J��
	//----------------------------------------------
	if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		// �N���b�N���ꂽ��A�t�H���_�ƃt�@�C���ꗗ���쐬
		sFolderList.clear();
		sSelectIndex = -1;
		std::string parentpath = filesystem::path(inoutFilename).parent_path().string();
		// data�t�H���_�ȉ��̃t�@�C������S�擾
		std::vector<filesystem::path> fileList = KdEnumFiles("data", true, true);
		for (auto&& item : fileList) {
			// �t�H���_���ǂ���
			if (filesystem::is_directory(item)) {
				sFolderList.push_back(FolderNode());
				sFolderList.back().m_Path = item;
				// ���݂̃t�@�C���Ɠ����t�H���_�Ȃ�A�I��index���w�肷��
				if (parentpath == item) {
					sSelectIndex = sFolderList.size() - 1;
				}
			}
		}
		if (folderSelectMode == false) {
			// �S�t�@�C�����𒲂ׁA�Y������t�H���_�֓o�^
			for (auto&& item : fileList) {
				// �t�@�C�����ǂ���
				if (filesystem::is_directory(item) == false) {
					// �w��g���q�̃t�@�C���̂�
					if (extFilter.count(item.extension().string()) == 0) continue;
					// �����̏�������t�H���_
					auto parent = item.parent_path();
					// �t�H���_�ꗗ����T��
					for (auto&& n : sFolderList) {
						if (n.m_Path == parent) {
							// ���̃t�H���_�̎q�Ƃ��ăt�@�C������o�^
							n.m_Childs.push_back(item);
							break;
						}
					}
				}
			}
		}
	}
	//----------------------------------------------
	// �t�@�C���u���E�U�[�|�b�v�A�b�v
	//----------------------------------------------
	if (ImGui::BeginPopupContextItem("ResourceFileBrowserContextMenu", 0)) {
		// ����
		if (ImGui::Button(u8"����")) {
			// �I��
			inoutFilename = "";
			bSelected = true;
			// �|�b�v�A�b�v�����
			ImGui::CloseCurrentPopup();
		}
		ImGui::BeginChild("ResourceFileBrowserContextMenu Left", ImVec2(300, 400), true);
		ImGui::Text(u8"�t�H���_");
		ImGui::Separator();
		// �t�H���_�ꗗ
		{
			int idx = 0;
			for (auto&& node : sFolderList) {
				// �I������Ă���t�H���_�H
				bool selected = idx == sSelectIndex ? true : false;
				// �t�@�C����
				if (ImGui::Selectable(node.m_Path.string().c_str(), selected)) {
					sSelectIndex = idx;
					// �t�H���_�I�����[�h
					if (folderSelectMode) {
						// �I��
						inoutFilename = node.m_Path.string();
						bSelected = true;
						// �|�b�v�A�b�v�����
						ImGui::CloseCurrentPopup();
					}
				}
				idx++;
			}
		}
		ImGui::EndChild();
		if (folderSelectMode == false) {
			ImGui::SameLine();
			ImGui::BeginChild("ResourceFileBrowserContextMenu Right", ImVec2(300, 400), true);
			ImGui::Text(u8"�t�@�C��");
			ImGui::Separator();
			// �t�@�C���ꗗ
			if (sSelectIndex >= 0) {
				auto& currentFolder = sFolderList[sSelectIndex];
				for (auto&& item : currentFolder.m_Childs) {
					// �t�@�C����
					if (ImGui::Selectable(item.filename().string().c_str())) {
						// �I��
						inoutFilename = item.string();
						bSelected = true;
						// �|�b�v�A�b�v�����
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
	return bSelected;
}
bool ImGuiResourceButton(const std::string & label, std::string & inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode)
{
	//----------------------------------------------
	// Label
	//----------------------------------------------
	ImGui::Text(label.c_str());
	std::string str;
	if (inoutFilename.size() == 0)str = u8"�t�@�C������";
	else str = inoutFilename;
	// �{�^��
	ImGui::Button(str.c_str());
	// ��L�̃R���g���[���̍��N���b�N�����m���A���\�[�X�u���E�U�̏��������s�����
	if (ImGuiBeginPopup_ResourceBrowser(label, str, extFilter, folderSelectMode)) {
		inoutFilename = str;
		return true;
	}
	return false;
}