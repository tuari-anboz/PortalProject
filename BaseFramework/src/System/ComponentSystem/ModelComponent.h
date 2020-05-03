#pragma once

#include "BaseComponent.h"

//====================================
//
// 3Dモデル描画、アニメーション、ボーン計算を行うコンポーネント
//
// 今後必要な物、現在動作中のアニメーションの(名前とアニメーション位置)保存
//
//====================================
class ModelComponent : public BaseComponent {
public:
	//=============================
	// 設定・取得
	//=============================
	// モデルデータ取得
	SPtr<KdGameModel> GetModel() { return m_model; }
	// モデルデータ設定
	void SetModel(const std::string& filename);
	// ボーンコントローラ取得
	KdBoneController& GetBc() { return m_bc; }
	// アニメータ取得
	KdAnimator& GetAnimator() { return m_anim; }
	// スクリプトlキーコールバック関数セット
	void SetScriptProc(std::function<void(KdAnimeKey_Script*)> proc)
	{
		m_onScriptProc = proc;
	}
	// モデルの発光色マップを変更
	void ChangeEmissiveMap();

	//=============================
	// 処理
	//=============================

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw() override;
	// 半透明描画処理
	virtual void DrawTransparency() override;
	// シャドウマップ描画処理
	virtual void DrawShadowMap() override;
	// (Editor)GUI処理
	virtual void ImGuiUpdate() override;

	// (Editor用)描画
	virtual void EditorDraw() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// 継承元のDeserializeを実行
		BaseComponent::Deserialize(jsonObj);

		// モデルファイル
		std::string filename = jsonObj["ModelFilename"].string_value();
		SetModel(filename);

		// アニメ関係
		m_playAnimename = jsonObj["PlayAnimename"].string_value();
		m_loopAnime = jsonObj["LoopAnime"].bool_value();
		m_anim.ChangeAnime(m_playAnimename, m_loopAnime);

		//
		m_roughness = (float)jsonObj["Roughness"].number_value();
		m_lightEnable = jsonObj["LightEnable"].bool_value();
		m_metallic = (float)jsonObj["Metallic"].number_value();
		m_enableWater = jsonObj["EnableWater"].bool_value();
		m_enableRefract = jsonObj["EnableRefract"].bool_value();
		if (jsonObj["EnableFog"].is_bool())
		{
			m_enableFog = jsonObj["EnableFog"].bool_value();
		}
	}

	// 自分のクラスデータをoutJsonへ格納する
	virtual void Serialize(json11::Json::object& outJson) override {
		// 継承元のSerializeを実行
		BaseComponent::Serialize(outJson);
		// 自分のデータもJSON化
		outJson["ModelFilename"] = m_modelFilename;
		outJson["PlayAnimename"] = m_playAnimename;
		outJson["LoopAnime"] = m_loopAnime;
		outJson["Roughness"] = m_roughness;
		outJson["LightEnable"] = m_lightEnable;
		outJson["Metallic"] = m_metallic;
		outJson["EnableWater"] = m_enableWater;
		outJson["EnableRefract"] = m_enableRefract;
		outJson["EnableFog"] = m_enableFog;
	}

private:
	// 3Dメッシュモデル
	SPtr<KdGameModel>	m_model;
	std::string			m_modelFilename;
	// ボーン操作用
	KdBoneController	m_bc;
	// アニメータ
	KdAnimator			m_anim;
	std::string			m_playAnimename;	// 再生するアニメ名
	bool				m_loopAnime = true;	// ループ再生

	//=======================================
	// グラフィックス設定
	//=======================================
	// ライトのOn/Off
	bool				m_lightEnable = true;
	// 発光色マップを使用するかしないか		
	// 粗さ
	float				m_roughness = 1;
	// 金属度
	float				m_metallic = 0;
	// 水面表現
	bool				m_enableWater = false;
	// 屈折表現
	bool	m_enableRefract = false;
	// フォグのOn/Off
	bool	m_enableFog = true;
	// 合成色指定
	KdVec4 m_mulColor = { 1,1,1,1 };

	

	// スクリプトキー用コールバック関数
	std::function<void(KdAnimeKey_Script*)>	m_onScriptProc;

};
