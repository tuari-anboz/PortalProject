#pragma once

#include "BaseComponent.h"


class BoxGeneratorComponent : public BaseComponent
{
public:
	// 初期設定
	virtual void Start() override;

	// 処理
	virtual void Update() override;

	// ImGui処理
	virtual void ImGuiUpdate() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// 生成中
	bool m_isGenerating = false;

	//===========================
	// アクションステートクラス
	//===========================
	class BaseBoxGeneratorAction : public std::enable_shared_from_this<BaseBoxGeneratorAction> {
	public:
		virtual ~BaseBoxGeneratorAction() {}
		// 行動処理
		// 戻り値：次の行動クラス
		virtual SPtr<BaseBoxGeneratorAction> Update() = 0;
		// キャラへのアドレス
		BoxGeneratorComponent*	m_pComp = nullptr;
	};
	// 「凡用」行動
	class Generic : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};
	// 「オン」行動
	class ON : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};
	// 「オフ」行動
	class OFF : public BaseBoxGeneratorAction {
	public:
		virtual SPtr<BaseBoxGeneratorAction> Update() override;
	};


	// 現在の行動
	SPtr<BaseBoxGeneratorAction>	m_as = nullptr;
};
