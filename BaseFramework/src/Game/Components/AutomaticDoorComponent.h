#pragma once

#include "System/ComponentSystem/BaseComponent.h"

// 自動ドアコンポーネント
// 当たり判定の制御や挙動を担当する
namespace AUTODOOR
{
	enum
	{
		CLOSE = 0,
		OPEN = 1
	};
};


class AutomaticDoorComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	// ドアの開閉
	void Open();
	void Close();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// ドアの状態
	int m_state = AUTODOOR::CLOSE;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseAutomaticDoorAction : public std::enable_shared_from_this<BaseAutomaticDoorAction> {
	public:
		virtual ~BaseAutomaticDoorAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseAutomaticDoorAction> Update() = 0;
		// キャラへのアドレス
		AutomaticDoorComponent*	m_pComp = nullptr;
	};
	// 「凡用」行動
	class Generic : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};
	// 「開く」行動
	class OpenAction : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};
	// 「閉まる」行動
	class CloseAction : public BaseAutomaticDoorAction {
	public:
		virtual SPtr<BaseAutomaticDoorAction> Update() override;
	};


	// 現在の行動
	SPtr<BaseAutomaticDoorAction>	m_as = nullptr;
};