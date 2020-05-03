#include "main.h"

#include "../../GameManager.h"
#include "BaseColliderComponent.h"
#include "CollisionManager.h"

// ヒット状態フラグ
namespace HitStates {
	enum {
		ENTER = 0x00000001,	// HITした1回目
		STAY = 0x00000002,	// HIT中ずっと
		EXIT = 0x00000004,	// HITしなくなった1回目
	};
}

void CollisionManager::Run()
{
	//======================================================
	//
	// 全Atkリストから、全Defリストへ判定を行う
	//
	//======================================================

	//++++++++++++++++++++++++++
	// シングルスレッドでの判定
	//++++++++++++++++++++++++++
	if(m_isMultiThread == false){

		//=================================
		// 判定実行
		//=================================
		for (auto&& atk : m_atkList) {

			HitTestToDefList(atk.get());

		}

	}
	//++++++++++++++++++++++++++
	// マルチスレッドで並列判定実行
	// Atkタイプがスレッドで同時実行され、Defタイプリストと判定する
	//++++++++++++++++++++++++++
	else {

		//=================================
		// マルチスレッドで並列判定実行
		// 攻タイプがスレッドで動作し、防タイプリストと判定する
		//=================================
		struct ThreadData {
			int listNum = 0;
			std::atomic<int> nowIdx = 0;	// カウンタ(アトミック変数)
		};
		ThreadData thData;

		// １スレッドでの処理関数(Atk側の処理)
		auto execThread = [this, &thData]() {
			// リストの最後までループし処理する
			while (1) {
				// nowIdxを1加算し、加算「前」の値を返す atomic型の変数は複数スレッドから同時操作しても安全
				int idx = thData.nowIdx.fetch_add(1);
				if (idx >= thData.listNum)break;

				// atkObj vs 全m_defList
				HitTestToDefList(m_atkList[idx].get());
			}
		};

		// CPU数 取得
		SYSTEM_INFO sys;
		GetSystemInfo(&sys);
		int cpuCnt = (int)sys.dwNumberOfProcessors;

		std::vector<std::future<void>> fut(cpuCnt);

		// スレッドデータ設定
		thData.nowIdx = 0;
		thData.listNum = m_atkList.size();

		// CPU数ぶんスレッドを起動し、並列で判定を実行する
		for (DWORD i = 0; i < fut.size(); i++) {
			fut[i] = std::async(std::launch::async | std::launch::deferred, execThread);
		}
		// 全スレッドの終了待機
		for (DWORD i = 0; i < fut.size(); i++) {
			fut[i].wait();
		}

	}



	//======================================================
	//
	// 結果通知関数実行
	//
	//======================================================
	for (auto&& atk : m_atkList) {

		// 初回ヒット時
		if (atk->m_hitState & HitStates::ENTER) {
			if (atk->m_onHitEnter)atk->m_onHitEnter(atk.get());
		}

		// ヒット中ずっと
		if (atk->m_hitState & HitStates::STAY) {
			if (atk->m_onHitStay)atk->m_onHitStay(atk.get());
		}

		// ヒットしなくなった最初の一回目のみ
		if (atk->m_hitState & HitStates::EXIT) {
			if (atk->m_onHitExit)atk->m_onHitExit(atk.get());
		}

	}
}

void CollisionManager::HitTestToDefList(BaseColliderComponent * atk)
{
	if (atk->IsAtk() == false)return;

	// 判定結果データをクリアする
	atk->m_results.clear();

	// Def側と判定
	for (auto&& def : m_defList) {

		//------------------------
		// 事前チェック
		//------------------------

		// 自分 vs 相手 フィルタ判定
		if ( (atk->m_groupFilter & def->m_maskFilter) == 0 ||
			 (atk->m_maskFilter & def->m_groupFilter) == 0
		) {
			continue;
		}

		// 同じ持ち主は無視
		if (atk->GetOwner() == def->GetOwner())continue;

		// 無視リストに登録されているGameObjectは無視
//		if (atk->m_ignoreList.count(def->GetGameObjectRaw()) >= 1)continue;
		if (atk->m_ignoreMap.count(def->GetOwner()) >= 1)continue;

		//------------------------
		// 判定
		//------------------------
		atk->HitTest(def.get());
	}

	//-------------------------
	// ヒット状態フラグ操作
	//-------------------------

	// 初回HITフラグとHIT終了フラグをOFF
	atk->m_hitState &= ~HitStates::ENTER;	// OFF
	atk->m_hitState &= ~HitStates::EXIT;	// OFF

	// ヒットしている
	if (atk->m_results.size() > 0) {
		if (!(atk->m_hitState & HitStates::STAY)) {	// 前回HIT中ではない
			atk->m_hitState |= HitStates::ENTER;	// 初回HIT ON
			atk->m_hitState |= HitStates::STAY;		// HIT中 ON
		}
	}
	// ヒットしていない
	else {
		// HIT中ならHIT中フラグOFFして、HIT終了フラグON
		if (atk->m_hitState & HitStates::STAY) {
			atk->m_hitState |= HitStates::EXIT;		// ON
			atk->m_hitState &= ~HitStates::STAY;	// OFF
		}
	}
}
