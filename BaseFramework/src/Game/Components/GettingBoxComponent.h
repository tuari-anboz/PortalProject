#pragma once

#include "System/ComponentSystem/BaseComponent.h"

// 重力箱コンポーネント
// 当たり判定の制御や挙動を担当する

class GettingBoxComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	// 
	void GetBox();

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// ボックスを取得しているかどうか
	bool m_GettingBox = false;

	// 取得しているBox
	WPtr<Object> m_pBox;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseGettingBoxAction : public std::enable_shared_from_this<BaseGettingBoxAction> {
	public:
		virtual ~BaseGettingBoxAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseGettingBoxAction> Update() = 0;
		// キャラへのアドレス
		GettingBoxComponent*	m_pComp = nullptr;
	};
	// 「基本」行動
	class Basic : public BaseGettingBoxAction 
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};
	// 「取得」行動
	class Get : public BaseGettingBoxAction
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};
	// 「リリース」行動
	class Release : public BaseGettingBoxAction
	{
	public:
		virtual SPtr<BaseGettingBoxAction> Update() override;
	};

	SPtr<BaseGettingBoxAction> m_as = nullptr;
};