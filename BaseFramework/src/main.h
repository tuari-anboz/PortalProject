#pragma once

#define NOMINMAX
#include <windows.h>
#include <stdio.h>

//===========================
// KdLibrary
//===========================
#include "KdLibrary.h"

//===========================
// その他
//===========================
#include "Shader/ShaderManager.h"
#include "System/GameWindow.h"
#include "System/DebugDrawSystem.h"
#include "System/EditorCamera.h"

#include "System/ImGuiHelper.h"


//============================================================
// アプリケーションクラス
//	グローバル変数などを集めたようなクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Applicatoin{
// メンバ
public:

	// アプリケーション初期化
	bool Init(HINSTANCE hInst, int w, int h);

	// ゲームループ
	void Loop();

	// アプリケーション解放
	void Release();

	// ゲーム終了
	void EndGame() {
		m_EndFlag = true;
	}

	//=================================================
	// 公開データ
	//=================================================
	
	// ゲームウィンドウクラス
	GameWindow	m_Window;


	//=====================================================
	// グローバルデータ
	//=====================================================

	HINSTANCE	m_hInst = nullptr;		// インスタンスハンドル
	
	int			m_Fps = 0;					// 現在のFPS値
	int			m_maxFps = 60;				// 最大FPS


private:

	// ゲーム終了フラグ trueで終了する
	bool	m_EndFlag = false;


//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Applicatoin(){

	}

public:
	static Applicatoin &GetInstance(){
		static Applicatoin Instance;
		return Instance;
	}
};

// 簡単にアクセスできるようにするためのマクロ
#define APP Applicatoin::GetInstance()
