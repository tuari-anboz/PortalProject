#pragma once

#include "BaseColliderComponent.h"

//===================================================
//
// あたり判定管理クラス
// ・コライダーを登録し、お互い判定を行い、結果を通知する(通知のAtk属性のコライダーのみ)。
// ・マルチスレッドでの並列判定も可能。
//
//===================================================
class CollisionManager {
public:

	//-------------------------------------------------------
	// 判定オブジェクトを登録
	//-------------------------------------------------------
	void Register(const SPtr<BaseColliderComponent>& obj) {
		if (obj == nullptr)return;

		// 「判定する側」へ登録
		if (obj->IsAtk()) {
			m_atkList.push_back(obj);
		}
		// 「判定される側」へ登録
		else {
			m_defList.push_back(obj);
		}
	}

	//-------------------------------------------------------
	// 登録されている判定オブジェクトリストをクリアする
	//-------------------------------------------------------
	void ClearList() {
		for (auto&& atk : m_atkList)
		{
			atk->m_onHitEnter = nullptr;
			atk->m_onHitStay = nullptr;
			atk->m_onHitExit = nullptr;
		}
		m_atkList.clear();
		m_defList.clear();
	}

	//-------------------------------------------------------
	// 判定実行＆通知
	//-------------------------------------------------------
	void Run();

	//-------------------------------------------------------
	// デバッグ描画
	// ・登録しているコライダーを視覚的に確認できるように、
	// 　コライダー形状やヒット情報を線などで描画する。
	//-------------------------------------------------------
	void DebugDraw() {
		KdVec4 color;

		color = { 1.0f, 0.1f, 0.1f, 0.6f };
		for (auto&& obj : m_atkList) {

			obj->DebugDraw(color);
		}

		color = { 0.1f, 0.1f, 1.0f, 0.3f };
		for (auto&& obj : m_defList) {

			obj->DebugDraw(color);
		}
	}

	//-------------------------------------------------------
	// 動作設定・取得
	//-------------------------------------------------------
	// マルチスレッドでの判定 On/Off
	void SetMultiThread(bool enable) {
		m_isMultiThread = enable;
	}

	// マルチスレッド動作か？
	bool IsMultiThread() const { return m_isMultiThread; }

	// フィルターリストの１項目のデータ
	struct FilterNode {
		std::string name;	// フィルター名
		int flag = 0;		// ビットフラグ
		bool atk = false;	// 判定する側
	};
	// フィルターリストの設定
	void InitFilter(const std::vector<CollisionManager::FilterNode>& atkFilters,
					const std::vector<CollisionManager::FilterNode>& defFilters
	) {
		m_atkFilterList = atkFilters;
		m_defFilterList = defFilters;
	}
	// フィルターリスト取得
	const std::vector<CollisionManager::FilterNode>& GetAtkFilterList() const { return m_atkFilterList; }
	const std::vector<CollisionManager::FilterNode>& GetDefFilterList() const { return m_defFilterList; }

private:

	// atk vs m_defList の判定を実行
	void HitTestToDefList(BaseColliderComponent* atk);

	// 判定する側 リスト
	std::vector<SPtr<BaseColliderComponent>>	m_atkList;

	// 判定される側 リスト
	std::vector<SPtr<BaseColliderComponent>>	m_defList;

	// Run関数のあたり判定をマルチスレッドで行う
	bool m_isMultiThread = true;

	// フィルターリスト
	std::vector<CollisionManager::FilterNode>	m_atkFilterList;
	std::vector<CollisionManager::FilterNode>	m_defFilterList;

//=======================================
// シングルトン
//=======================================
private:
	CollisionManager() {
		InitFilter(
			{
				{u8"ぶつかり",	0x00000001},
				{u8"攻撃",		0x00000002},
			},
			{
				{u8"背景物",	0x00010000},
				{u8"味方",		0x00020000},
				{u8"敵",		0x00040000},
				{u8"プレイヤー",0x00080000},
				{u8"ポータル",	0x00100000},
				{u8"サーチ"	   ,0x00200000},
				{u8"エディター",0x00400000},
				{u8"ボタン",    0x00800000},
				{u8"ボックス"  ,0x01000000},
			}
		);
	}
public:
	static CollisionManager& GetInstance() {
		static CollisionManager instance;
		return instance;
	}
};

#define ColMgr CollisionManager::GetInstance()
