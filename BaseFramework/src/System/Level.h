#pragma once

//===============================
//
// Levelクラス
// ・１つのシーンとなるクラス
//
//===============================
class Level {
public:
	// コンストラクタ
	Level();

	// Serialize/Deserialize
	void Deserialize(const json11::Json& jsonObj);
	void Serialize(json11::Json::object& outJson);
	
	// ファイル操作
	void Load( const std::string& filename );
	void Save();

	void Update();
	void Draw();

	// Editor処理
	void EditorUpdate();

	// ImGui処理
	void ImGuiUpdate();

	// オブジェクト追加
//	void AddObject(SPtr<Object> obj) {
//		m_objectList.push_back(obj);
//	}

	// Rootオブジェクトの取得
	const SPtr<Object>& GetRoot() { return m_root; }

private:
	// ファイル名
	std::string			m_levelFilename;

	// オブジェクトリスト
//	std::list< SPtr<Object> >	m_objectList;

	// Rootオブジェクト
	SPtr<Object> m_root;

	//-------------------------
	// グラフィック設定
	//-------------------------
	// IBLテクスチャ
	SPtr<KdTexture> m_texIBL;

	// フォグ設定
	KdVec3 m_fogColor = { 1,1,1 };
	float m_fogDensity = 0.02f;

	// Workレンダーターゲット(shared_ptrで作るべき)
	KdTexture		m_rtWork[2];		// 作業テクスチャ
	KdTexture       m_rtHighBrightness;		// 高輝度抽出テクスチャ
	BlurTexture		m_blurTex;		// ブラーテクスチャ
};

