#pragma once

//============================
// std::string�o�[�W����InputText
//============================
inline bool ImGuiInputString(const char* label, std::string& str, ImGuiInputTextFlags flags = 0)
{
	static char tmp[1024];
	strcpy(tmp, str.c_str());
	if (ImGui::InputText(label, tmp, sizeof(tmp), flags)) {
		str = tmp;
		return true;
	}
	return false;
}

//===========================================
// ImGuzmo�p �}�j�s�����[�^�\���Ƒ���p�l���\��
// �Emat : ���삷��s��
// �EmView : �r���[�s��
// �EmProj : �ˉe�s��
// �Ebb : �g�呀�쎞�̃T�C�Y�g�p�o�E���f�B���O�{�b�N�X���w��(AABB) nullptr����-0,5�`0.5�̃T�C�Y�g���\�������
//===========================================
void ImGuizmoEditTransform(KdMatrix& mat, const KdMatrix& mView, const KdMatrix& mProj, const
	DirectX::BoundingBox* bb);

//===========================================
// ���\�[�X�t�@�C���I���|�b�v�A�b�v
// �Eid �c ID
// �EinoutFilename �c �I�����ꂽ�t�@�C��������
// �EextFilter �c �g���q�t�B���^ �����Ŏw�肳�ꂽ�g���q�̂ݕ\�������
// �EfolderSelectMode �c �t�H���_�I�����[�h
// �E�߂�l �c true:�I�����ꂽ false:�������Ă��Ȃ�
//===========================================
bool ImGuiBeginPopup_ResourceBrowser(const std::string& id, std::string& inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode = false);
//===========================================
// ���\�[�X�I���{�^��(ImGuiBeginPopup_ResourceBrowser�֐��g�p)
// �{�^�������N���b�N����ƁA�t�@�C���u���C�U���J���ȈՔ�
//===========================================
bool ImGuiResourceButton(const std::string& label, std::string& inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode = false);

//�w���v�c�[���`�b�v�\���֐�
// �Etext �c �\��������
// �EshowIcom �c true:(?)��\�������̏�Ƀ}�E�X��������\��
// false:���݂̃R���g���[���̏�Ƀ}�E�X��������\��
inline void ImGuiShowHelp(const std::string& text, bool showIcon)
{
	if (showIcon) {
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(text.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

//============================
// ���O�E�B���h�E�N���X
//============================
class ImGuiLogWindow {
public:
	// ���O���N���A
	void Clear() { m_Buf.clear(); }
	// �������ǉ�
	template<class... Args>
	void AddLog(const char* fmt, Args... args)
	{
		// ���s��������
		std::string str = fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), args...);
		m_ScrollToBottom = true;
	}
	// �E�B���h�E�`��
	void ImGuiUpdate(const char* title, bool* p_opened = NULL)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(m_Buf.begin());
		if (m_ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		m_ScrollToBottom = false;
		ImGui::End();
	}
private:
	ImGuiTextBuffer m_Buf;
	bool m_ScrollToBottom = false;
};
