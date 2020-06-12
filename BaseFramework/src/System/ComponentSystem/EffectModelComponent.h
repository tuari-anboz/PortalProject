#pragma once

#include "BaseComponent.h"

//==============================================
//
// エフェクトモデルコンポーネント
//  エフェクトとして3Dモデル表示を行う
//  ・DrawEffect関数での描画(Z書き込みOFF)
//  ・モデルとテクスチャを別指定
//  ・描画時にビルボーディングを行う
//
//==============================================
class EffectModelComponent : public BaseComponent {
public:

	//----------------------------
	// 設定・取得
	//----------------------------
	// モデル読み込み
	void LoadModel(const std::string& modelFilename, const std::string& texFilename) {
		m_modelFilename = modelFilename;
		m_texFilename = texFilename;
		m_model = KDResFactory.GetGameModel(m_modelFilename);
		m_tex = KDResFactory.GetTexture(m_texFilename);
	}

	// GameModel取得
	const SPtr<KdGameModel>& GetGameModel() const { return m_model; }

	//----------------------------
	// 描画処理
	//----------------------------
	// 3D描画
	virtual void DrawEffect() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseComponent::Deserialize(jsonObj);
		m_modelFilename = jsonObj["ModelFilename"].string_value();
		m_texFilename = jsonObj["TexFilename"].string_value();
		m_billboardType = jsonObj["BillboardType"].int_value();
		m_blendType = jsonObj["BlendType"].int_value();
		m_color.Deserialize(jsonObj["Color"]);

		LoadModel(m_modelFilename, m_texFilename);
	}
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["ModelFilename"] = m_modelFilename;
		outJsonObj["TexFilename"] = m_texFilename;
		outJsonObj["BillboardType"] = m_billboardType;
		outJsonObj["BlendType"] = m_blendType;
		outJsonObj["Color"] = m_color.Serialize();
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:

	//--------------------------
	// 描画用モデルデータ
	//--------------------------
	std::string			m_modelFilename;		// モデルファイル名
	SPtr<KdGameModel>	m_model;				// モデルデータ

	std::string			m_texFilename;			// テクスチャファイル名
	SPtr<KdTexture>		m_tex;					// テクスチャデータ

	KdVec4				m_color = { 1,1,1,1 };	// 乗算色

	int					m_billboardType = 0;	// ビルボードの種類
	int					m_blendType = 0;		// 合成モード
};

