#pragma once

class LevelEditor
{
public:
	LevelEditor(){}

	void Init();

	void Update();

	// 
	void ImGuiUpdate();

	void Deserialize(const json11::Json& jsonObj);

	void Serialize(json11::Json::object& outJsonObj);


private:

	// オブジェクトをコピーするかどうか
	bool m_isCopy = true;

	// 生成、置換するプレハブファイル名
	std::string m_generationPrefabName = "";

	//===========================
	// アクションステートクラス
	//===========================
	class BaseLevelEditorAction : public std::enable_shared_from_this<BaseLevelEditorAction> {
	public:
		virtual ~BaseLevelEditorAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseLevelEditorAction> Update() = 0;
		// キャラへのアドレス
		LevelEditor*	m_pObject = nullptr;
	};
	// 「基本」行動
	class Basic : public BaseLevelEditorAction {
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「選択中」行動
	class Selected : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「作成」行動
	class Create : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};
	// 「置換」行動
	class Replace : public BaseLevelEditorAction
	{
	public:
		virtual SPtr<BaseLevelEditorAction> Update() override;
	};

	// 現在の行動
	SPtr<BaseLevelEditorAction>	m_as = nullptr;
};