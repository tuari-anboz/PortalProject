#pragma once

#include "BaseComponent.h"

// DoorComponent
// ドアを開閉するコンポーネント
// 子のドアオブジェクトのアニメーション管理などをする

namespace DOOR
{
	enum
	{
		CLOSE = 0,
		OPEN = 1
	};
};

class DoorComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	//
	void Open();

	//
	void Close();

	// 状態を取得
	const int GetState() { return m_state; }

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// 
	int m_state = DOOR::CLOSE;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseDoorAction : public std::enable_shared_from_this<BaseDoorAction> {
	public:
		virtual ~BaseDoorAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseDoorAction> Update() = 0;
		// キャラへのアドレス
		DoorComponent*	m_pComp = nullptr;
	};
	// 「凡用」行動
	class Generic : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};
	// 「オン」行動
	class ON : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};
	// 「オフ」行動
	class OFF : public BaseDoorAction {
	public:
		virtual SPtr<BaseDoorAction> Update() override;
	};


	// 現在の行動
	SPtr<BaseDoorAction>	m_as = nullptr;
};