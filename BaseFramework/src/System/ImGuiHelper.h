#pragma once

//============================
// std::stringバージョンInputText
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
// ImGuzmo用 マニピュレータ表示と操作パネル表示
// ・mat : 操作する行列
// ・mView : ビュー行列
// ・mProj : 射影行列
// ・bb : 拡大操作時のサイズ枠用バウンディングボックスを指定(AABB) nullptrだと-0,5～0.5のサイズ枠が表示される
//===========================================
void ImGuizmoEditTransform(KdMatrix& mat, const KdMatrix& mView, const KdMatrix& mProj, const
	DirectX::BoundingBox* bb);

//===========================================
// リソースファイル選択ポップアップ
// ・id … ID
// ・inoutFilename … 選択されたファイルが入る
// ・extFilter … 拡張子フィルタ ここで指定された拡張子のみ表示される
// ・folderSelectMode … フォルダ選択モード
// ・戻り値 … true:選択された false:何もしていない
//===========================================
bool ImGuiBeginPopup_ResourceBrowser(const std::string& id, std::string& inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode = false);
//===========================================
// リソース選択ボタン(ImGuiBeginPopup_ResourceBrowser関数使用)
// ボタンを左クリックすると、ファイルブライザが開く簡易版
//===========================================
bool ImGuiResourceButton(const std::string& label, std::string& inoutFilename, const
	std::unordered_set<std::string> extFilter, bool folderSelectMode = false);

//ヘルプツールチップ表示関数
// ・text … 表示文字列
// ・showIcom … true:(?)を表示しその上にマウスがきたら表示
// false:現在のコントロールの上にマウスがきたら表示
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
// ログウィンドウクラス
//============================
class ImGuiLogWindow {
public:
	// ログをクリア
	void Clear() { m_Buf.clear(); }
	// 文字列を追加
	template<class... Args>
	void AddLog(const char* fmt, Args... args)
	{
		// 改行を加える
		std::string str = fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), args...);
		m_ScrollToBottom = true;
	}
	// ウィンドウ描画
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
