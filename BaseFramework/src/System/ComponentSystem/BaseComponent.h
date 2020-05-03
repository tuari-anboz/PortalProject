// BaseComponent.h
#pragma once

class Object;

//====================================
// コンポーネント基本クラス
//====================================
class BaseComponent : public std::enable_shared_from_this<BaseComponent>{
public:
	virtual ~BaseComponent() {}

	// クラス名取得
	std::string ClassName() {
		return &typeid(*this).name()[6];	// ※環境依存
	}

	// 初回実行処理
	virtual void Start() {}

	void CallStart()
	{
		if (m_isStart) {
			Start();
			m_isStart = false;
		}
	}

	// 更新処理
	virtual void Update() {}
	// 描画前処理
	virtual void PreDraw() {}
	// 描画処理
	virtual void Draw() {}
	// エフェクト系の描画
	virtual void DrawEffect() {}
	// 3D半透明描画処理
	virtual void DrawTransparency() {}
	// シャドウマップ描画処理
	virtual void DrawShadowMap() {}

	// 2D描画処理
	virtual void DrawSprite() {}

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() {
		// 有効/無効
		ImGui::Checkbox("Enable", &m_enable);
	}

	// (Editor)エディター描画
	virtual void EditorDraw() {}

	// １つ上へ
	void MoveUp();
	// １つ下へ
	void MoveDown();

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonObj) {
		m_enable = jsonObj["Enable"].bool_value();
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJson) {
		outJson["ClassName"] = ClassName();
		outJson["Enable"] = m_enable;
	}

	void Delete() { m_isDelete = true; }
	bool IsDelete() { return m_isDelete; }

	void SetOwner(Object* owner) {
		m_pOwner = owner;
	}
	Object* GetOwner() { return m_pOwner; }

	// 有効フラグセット
	virtual void SetEnable(bool enable) {
		m_enable = enable;
	}
	// 有効フラグ取得
	bool GetEnable() const { return m_enable; }

protected:
	// 有効/無効フラグ
	bool	m_enable = true;
	// 削除フラグ
	bool	m_isDelete = false;
	// 持ち主へのアドレス
	Object*	m_pOwner = nullptr;
	// 初回？
	bool	m_isStart = true;
};
