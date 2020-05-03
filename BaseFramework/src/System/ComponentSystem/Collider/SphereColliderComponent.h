#pragma once

//==============================================
//
// 球Collider コンポーネント
//
//==============================================
class SphereColliderComponent : public BaseColliderComponent {
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーの関数
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 種類
	virtual Types GetType() const override { return Types::Sphere; }

	// あたり判定実行　自分 vs obj
	virtual bool HitTest(BaseColliderComponent* obj) override;

	// デバッグ描画
	virtual void DebugDraw(const KdVec4& color) override;

	// 球判定データ取得
	const DirectX::BoundingSphere& GetSphere() const { return m_sphere; }

	const KdVec3& GetLocalPos() const { return m_localPos; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, float radius, bool isAtk, int groupFilter, int maskFilter) {
		m_isAtk = isAtk;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localPos = pos;
		m_sphere.Center = pos;
		m_radius = radius;
		m_sphere.Radius = radius;
	}


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	//  ここから通常コンポーネントの関数オーバーライド
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//----------------------------
	// 更新処理
	//----------------------------
	virtual void Update() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseColliderComponent::Deserialize(jsonObj);

		m_localPos.Deserialize(jsonObj["LocalPos"]);
		m_radius = (float)jsonObj["Radius"].number_value();
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseColliderComponent::Serialize(outJsonObj);

		outJsonObj["LocalPos"] = m_localPos.Serialize();
		outJsonObj["Radius"] = m_radius;
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

	// 球設定データ
	KdVec3	m_localPos;		// ローカル座標
	float	m_radius = 1;	// 半径

	// 判定用球データ　実際のあたり判定で使用されるデータ(m_localPosとm_radiusをTransformの行列で変換したもの)
	DirectX::BoundingSphere			m_sphere;

};