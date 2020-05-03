#pragma once

#include "BaseComponent.h"


//==============================================
//
// 2D画像コンポーネントクラス
//
//==============================================
class SpriteComponent : public BaseComponent {
public:

	//----------------------------
	// 設定・取得
	//----------------------------

	// 初期設定
	void LoadTex(const std::string& texFilename);

	// 2D描画処理
	virtual void DrawSprite() override;

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() override;

	// テクスチャ取得
	SPtr<KdTexture> GetTexture() { return m_tex; }

	// テクスチャ変更
	void ChangeTex(const SPtr<KdTexture>& tex) { if (tex == nullptr)return; m_tex = tex; }

	// テクスチャ上の座標かどうかを判定します
	bool IsOnTexture(const KdVec2& pos);

	// テクスチャの透明度設定
	void SetTexAlpha(const float alpha) { m_texAlpha = alpha; }

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// 画像ファイル名
	std::string			m_texFilename;

	// テクスチャデータ
	SPtr<KdTexture>		m_tex;

	// テクスチャの透明度
	float m_texAlpha = 1.0f;

};

