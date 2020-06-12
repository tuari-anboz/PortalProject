#pragma once

#include "../BaseComponent.h"

class Object;
class BaseColliderComponent;

//================================
// 結果データ
//================================
struct CollisionResult {
	SPtr<BaseColliderComponent>	Collider;	// ヒット相手のコライダーコンポーネント
	KdVec3 Pos;								// ヒット位置(※現在、一部の形状でのみ正確な値になっています)
	KdVec3 Push;							// 反発ベクトル(※現在、一部の形状でのみ正確な値になっています)
	float Dist = 0;							// 最短距離(※現在、一部の形状での正確な値になっています)

	// vsメッシュ専用
	int MeshIdx = -1;	// (vsメッシュ専用)メッシュ番号
	int FaceIdx = -1;	// (vsメッシュ専用)面番号
};

//==============================================
//
// Collider基本コンポーネント
// ・[ 仕様 ]
// 　m_isAtkフラグにより、「判定する側」と「判定される側」とに分かれる
// 　判定する側		… 「判定される側」に対してあたり判定を行い、その結果データを記憶し、m_onHit系の関数を実行(通知)する。
// 　判定される側	… 自分からは判定しに行かない。「判定する側」からひたすら判定されるのみで、結果データも持たず、通知も行わない。
//
//==============================================
class BaseColliderComponent : public BaseComponent {
	friend class CollisionManager;
	friend class CollisionFunctions;
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーの変数・関数
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// コライダーの種類定数
	enum class Types {
		Sphere = 0,		// 球
		Ray = 1,		// レイ
		Box = 2,		// 箱
		Mesh = 3,		// メッシュ
	};
	// 形状の種類(高速ダウンキャスト用)
	virtual Types GetType() const = 0;

	// あたり判定実行　自分 vs obj
	virtual bool HitTest(BaseColliderComponent* obj) = 0;

	// 判定デバッグ描画
	virtual void DebugDraw(const KdVec4& color) = 0;

	// 判定する側？
	bool IsAtk() const { return m_isAtk; }
	// グループフィルタ設定
	void SetGroupFilter(int filter) { m_groupFilter = filter; }
	// マスクフィルタ設定
	void SetMaskFilter(int filter) { m_maskFilter = filter; }

	// 物理エンジンの形状として使用する？
	bool UsePhysics() const { return m_usePhysics; }

	// タグ設定・取得
	const std::string& GetTag() const { return m_tag; }
	void SetTag(const std::string& tag) { m_tag = tag; }

	//----------------------------------------------
	//
	// 判定する側(m_isAtk == true 時) 専用のデータ
	//
	//----------------------------------------------

	// 当たり判定結果データ取得。ここにあたり判定の結果データが入っている。
	const std::vector<CollisionResult>& GetResults() const { return m_results; }

	// 無視リストへ登録
	void AddIgnore(Object* obj, UINT lifespan) {
		m_ignoreMap[obj] = lifespan;
	}

	// ヒット通知関数オブジェクト
	std::function<void(BaseColliderComponent*)>	m_onHitEnter;	// ヒット時の初回のみ実行される
	std::function<void(BaseColliderComponent*)>	m_onHitStay;	// ヒット中ずっと実行される
	std::function<void(BaseColliderComponent*)>	m_onHitExit;	// ヒットしなくなった初回のみ実行される

	// 無効カウンターをセット
	void SetDisableCounter(int count) {
		m_disableCounter = count;
	}
	// 無効カウンターの値を加算する(時間を延長する)
	void AddDisableCounter(int count) {
		m_disableCounter += count;
	}
	// 無効カウンター進行
	void UpdateDisableCounter() {
		if (m_disableCounter >= 0) {
			m_disableCounter--;
			if (m_disableCounter <= -1) {
				m_enable = false;	// 無効にする
			}
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ここから通常コンポーネントの関数オーバーライド
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// (Editor用)描画
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseComponent::Deserialize(jsonObj);

		m_tag = jsonObj["Tag"].string_value();
		m_isAtk = jsonObj["IsAtk"].bool_value();
		m_groupFilter = jsonObj["GroupFilter"].int_value();
		m_maskFilter = jsonObj["MaskFilter"].int_value();
		m_usePhysics = jsonObj["UsePhysics"].bool_value();
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Tag"] = m_tag;
		outJsonObj["IsAtk"] = m_isAtk;
		outJsonObj["GroupFilter"] = m_groupFilter;
		outJsonObj["MaskFilter"] = m_maskFilter;
		outJsonObj["UsePhysics"] = m_usePhysics;
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;


protected:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// コライダーのデータ
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// 汎用タグ
	std::string			m_tag;

	// 判定する側(結果データの記憶や通知が可能)
	bool				m_isAtk = false;

	// フィルタ
	int					m_groupFilter = 1;			// グループフィルタ　
	int					m_maskFilter = 0;	// マスクフィルタ

	// ヒット状態フラグ(初回ヒット時、ヒット中、ヒット解除時の判定用)
	int					m_hitState = 0;

	// 結果データ　ここにあたり判定の結果データが入ってくる
	std::vector<CollisionResult>	m_results;

	// 無視マップ　ここに登録されているGameObjectは判定対象から除外される。
	// 速度重視のため、生ポインタを使用。実際に内部にはアクセスしていません。
	std::unordered_map<Object*, UINT>	m_ignoreMap;
	void IgnoreMapUpdate();

	// 物理エンジンの形状として使用する
	bool				m_usePhysics = false;

	// 指定時間後にEnableをfalseにするカウンタ
	int		m_disableCounter = -1;

};

// 球コライダーコンポーネント
#include "SphereColliderComponent.h"
// レイコライダーコンポーネント
#include "RayColliderComponent.h"
// 箱コライダーコンポーネント
#include "BoxColliderComponent.h"
// メッシュコライダーコンポーネント
#include "MeshColliderComponent.h"

// Collision管理クラス
#include "CollisionManager.h"
