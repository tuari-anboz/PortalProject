#pragma once

#include "BaseComponent.h"

// キャラAI入力
class CharaAIInputComponent : public InputComponent {
public:
	virtual void Update() override {
		// 無効の場合は何もしない
		if (m_enable == false) {
			// ボタンをリセットだけ行う
			m_axis = { 0,0 };
			m_buttons = 0;
			return;
		}

		// 適当にキーを押す(0～100の乱数)
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			if (KdMTRand::s_Rnd.GetInt(0, 1) == 0)m_axis.y = 1;
			else m_axis.y = -1;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			m_axis.y = 0;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			if (KdMTRand::s_Rnd.GetInt(0, 1) == 0)m_axis.x = 1;
			else m_axis.x = -1;
		}
		if (KdMTRand::s_Rnd.GetInt(0, 100) == 0) {
			m_axis.x = 0;
		}
	}

	// ImGui処理
	virtual void ImGuiUpdate() override {
		// 継承元の関数も実行
		InputComponent::ImGuiUpdate();
		// 自分の内容
		ImGui::InputInt("AI Level", &m_aiLevel);
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// 継承元の関数も実行する
		InputComponent::Deserialize(jsonObj);

		m_aiLevel = jsonObj["AILevel"].int_value();
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJson) override {
		// 継承元の関数も実行する
		InputComponent::Serialize(outJson);

		outJson["AILevel"] = m_aiLevel;
	}


private:
	int		m_aiLevel = 1;	// AIの思考レベル
};

// 人間キャラ
class PlayerScript : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// Hpセット
	void SetHp(int hp) { m_hp = hp; }

	// 移動ベクトルゲット
	KdVec3& GetForce() { return m_vForce; }

	// ポータル衝突セット
	bool& SetIsPortalCollide() { return m_isPortalCollide; }

	// 更新処理
	virtual void Update() override;

	// 
	virtual void DrawEffect() override;

	// ダメージを食らったとき
	//virtual void OnDamage(const DamageArg& dmg, DamageReply& rep) override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	//===============================
	// Serialize / Deserialize
	//===============================
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// 継承元のほうのDeserialize関数も実行する
		BaseComponent::Deserialize(jsonObj);

		// このキャラ固有のデータ設定

		// Hp
		m_hp = jsonObj["Hp"].int_value();
		// 移動速度
		m_ms = (float)jsonObj["MoveSpeed"].number_value();
		// プレハブファイルネーム
		m_editorPrefabName = jsonObj["PrefabName"].string_value();
	}
	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJson) override {
		// 継承元のSerializeも実行する
		BaseComponent::Serialize(outJson);
		// 自分のデータもSerialize
		outJson["Hp"] = m_hp;
		outJson["MoveSpeed"] = m_ms;
		outJson["PrefabName"] = m_editorPrefabName;
	}

	// 状態管理
	enum State
	{
		STAND_STATE,
		WALK_STATE,
		SHOT_STATE,
	};

private:

	// Hp
	int					m_hp = 0;

	// プレイヤーの移動速度
	float				m_ms = 1.0f;

	// 移動ベクトル(キャラにかかっている力ベクトル)
	KdVec3				m_vForce;

	// ヒットストップ時間(カウンター)
	int					m_hitStop = 0;

	// 空中？
	bool				m_isSky = false;

	// Test
	KdLaser				m_trail;

	// ポータルに触れているか
	bool				 m_isPortalCollide = false;

	// エディタープレハブファイルネーム
	std::string			m_editorPrefabName = "";

	//===========================
	// アクションステートクラス
	//===========================
	class BaseAction : public std::enable_shared_from_this<BaseAction> {
	public:
		virtual ~BaseAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseAction> Update() = 0;
		// キャラへのアドレス
		PlayerScript*	m_pChara = nullptr;
	};
	// 「立ち」行動
	class Stand : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// 「歩き」行動
	class Walk : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// 「射撃行動」
	class Shot : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	class Shot2 : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	class GetBox : public BaseAction
	{
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// 汎用行動
	class Generic : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};
	// 死亡行動
	class Die : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;

		int m_cnt = 0;
	};

	// ジャンプ開始
	class JumpStart : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;

		KdVec3	m_vForce;
	};

	// ジャンプ上昇
	class JumpUp : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// ジャンプ下降
	class JumpDown : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// 着地
	class Landing : public BaseAction {
	public:
		virtual SPtr<BaseAction> Update() override;
	};

	// 現在の行動
	SPtr<BaseAction>	m_as;

	// 部分特殊行動
	SPtr<BaseAction>	m_asExt;
	KdAnimator			m_animExt;
};