#pragma once

//==============================================
//
// レイCollider コンポーネント
//
//==============================================
class RayColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーの関数
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 種類
	virtual Types GetType() const override { return Types::Ray; }

	// あたり判定　自分 vs obj
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// デバッグ描画
	virtual void DebugDraw(const KdVec4& color) override;


	// レイ座標取得
	const KdVec3& GetRayPos() const { return m_rayPos; }
	// レイ方向取得
	const KdVec3& GetRayDir() const { return m_rayDir; }
	// レイの長さ取得
	float GetRayLen() const { return m_rayLen; }

	// レイのローカル座標を取得
	const KdVec3& GetRayLocalPos() const { return m_localPos; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, const KdVec3& dir, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_rayPos = m_localPos = pos;
		m_rayDir = m_localDir = dir;
		m_rayLen = dir.Length();

		m_rayDir.Normalize();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ここから通常コンポーネントの関数オバーライド
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

		m_localPos.Deserialize(jsonObj["LocalPos"]);
		m_localDir.Deserialize(jsonObj["LocalDir"]);
		m_rayLen = (float)jsonObj["Len"].number_value();
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["LocalDir"] = m_localDir.Serialize();
		outJsonObj["RayLen"] = m_rayLen;
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

	// レイ設定データ
	KdVec3	m_localPos;		// ローカル座標
	KdVec3	m_localDir;		// ローカルベクトル(長さも使用)

	// 実際に判定で使用するレイデータ(m_localPosとm_localDirをTransformの行列で変換したもの)
	KdVec3	m_rayPos;		// 位置
	KdVec3	m_rayDir;		// 方向(正規化)
	float	m_rayLen;		// レイの長さ
};
