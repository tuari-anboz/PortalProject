#pragma once

//==============================================
//
// 箱Colliderコンポーネント
//
//==============================================
class BoxColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーの関数
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 種類を取得
	virtual Types GetType() const override { return Types::Box; }

	// あたり判定
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// デバッグ描画
	virtual void DebugDraw(const KdVec4& color) override;

	// 箱判定データ取得
	const DirectX::BoundingOrientedBox& GetBox() const { return m_box; }
	// 判定用回転行列取得
	const KdMatrix& GetRotaMat() const { return m_rotaMat; }

	const KdVec3& GetLocalPos() const { return m_localPos; }
	const KdVec3& GetLocalSize() const { return m_localSize; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, const KdVec3& size, const KdMatrix& rotaMat, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localPos = pos;
		m_box.Center = pos;

		m_localSize = size;
		m_box.Extents = size;

		// 回転のみの行列
		m_rotaMat.SetRotation(rotaMat);
		m_rotaMat.NormalizeScale();

		// 回転をクォータニオンに変換
		m_rotaMat.ToQuaternion((KdQuat&)m_box.Orientation, true);
	}

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

		m_localPos.Deserialize(jsonObj["LocalPos"]);
		m_localSize.Deserialize(jsonObj["LocalSize"]);
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["LocalSize"] = m_localSize.Serialize();
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// コライダーのデータ
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 箱設定データ　ここから下記のm_boxとm_rotaMatを毎フレーム計算する
	KdVec3	m_localPos;
	KdVec3	m_localSize = {1, 1, 1};

	// 当たり判定用箱データ　実際のあたり判定で使用されるデータ(m_localPosとm_localSizeをTransformの行列で変換したもの)
	DirectX::BoundingOrientedBox	m_box;
	KdMatrix						m_rotaMat;	// 回転のみの行列(各軸は正規化)

};
