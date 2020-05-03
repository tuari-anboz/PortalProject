#pragma once

#include "BaseComponent.h"

// **************************
// 必要な事
// ・音を鳴らすタイミングでソース内で任意にPlayやStop等を出来る事
// ・音を鳴らす時の関数の引数は鳴らしたい音のファイル名(タグになるかも)
// ・ImGui上で音を鳴らすファイルを指定出来る事
// ・

class AudioComponent : public BaseComponent
{
public:
	//----------------------------
	// 設定・取得
	//----------------------------
	// 更新処理
	virtual void Update()override;

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() override;

	// (Editor)Editor描画
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;

	// 
	// b3D・・・3Dサウンドにするか？
	void SetSound(const std::string & filename,bool b3D = false);
	void Play(bool loop);
	void Play3D(const KdVec3& pos,bool loop);
	void Stop();
	void Pause();
	void Resume();

private:
	// ファイル名
	std::string m_soundFilename;
	
	SPtr<KdSoundEffect> m_soundEffect;
	SPtr<KdSoundInstance> m_soundInstance;

	// サウンドエフェクトの設定メンバ
	bool m_b3D = false;

	// タグ
	std::string m_tag;

	// 3Dサウンドの場合のローカル座標
	KdVec3 m_pos;

	// サウンドの距離減衰が始まる距離の設定メンバ(この設定した距離を超えると減衰が始まる)
	float m_CurveDistanceScale = FLT_MAX;

};

class AudioListenerComponent : public BaseComponent
{
public:
	//----------------------------
	// 設定・取得
	//----------------------------
	// 更新処理
	virtual void Update()override;

	// (Editor)ImGui処理
	virtual void ImGuiUpdate() override;

	// (Editor)Editor描画
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;

private:
};