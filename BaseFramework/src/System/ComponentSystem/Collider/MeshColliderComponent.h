#pragma once



//==============================================
//
// メッシュColliderコンポーネント
//
//==============================================
class MeshColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーの関数
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 種類
	virtual Types GetType() const override { return Types::Mesh; }

	// あたり判定
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// デバッグ描画
	virtual void DebugDraw(const KdVec4& color) override;

	// 取得
	const SPtr<KdGameModel>& GetModel() { return m_model; }
	const KdMatrix& GetMatrix() const { return m_matrix; }
	const KdMatrix& GetInvMatrix() const { return m_invMatrix; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const std::string& filename, const KdMatrix& mat, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_filename = filename;
		m_matrix = mat;
		m_matrix.Inverse(m_invMatrix);
		LoadModel(m_filename);
	}

	//
	void LoadModel(const std::string& filename);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ここから通常コンポーネントの関数オーバーライド
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//----------------------------
	// 初回実行処理
	//----------------------------
	virtual void Start() override;

	//----------------------------
	// 更新処理
	//----------------------------
	virtual void Update() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseColliderComponent::Deserialize(jsonObj);

		m_filename = jsonObj["ModelFilename"].string_value();

		// 読み込み
		LoadModel(m_filename);
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_filename;
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーのデータ
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ファイル名
	std::string			m_filename;

	// 判定用データ
	SPtr<KdGameModel>	m_model;		// メッシュ
	KdMatrix			m_matrix;		// Transformの行列
	KdMatrix			m_invMatrix;	// Transformの逆行列
};

