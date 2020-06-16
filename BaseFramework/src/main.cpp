#include "main.h"

#include "System/GameManager.h"

//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{

	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// ウィンドウ作成、Direct3D初期化など
	//===================================================================
	if (APP.Init(hInstance, 1280, 720) == false) {
		return 0;
	}

	//===================================================================
	// ループ
	//===================================================================
	APP.Loop();

	//===================================================================
	// 解放
	//===================================================================
	APP.Release();

	// COM解放
	CoUninitialize();

	return 0;
}

// アプリケーション初期設定
bool Applicatoin::Init(HINSTANCE hInst, int w, int h)
{
	// インスタンスハンドル記憶
	APP.m_hInst = hInst;

	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_Window.Create(hInst, w, h, "Window") == false) {
		MessageBox(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
	if (MessageBox(m_Window.GetHWnd(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}


	//===================================================================
	// Direct3D初期化
	//===================================================================

	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	std::string errorMsg;
	if (KD3D.Init(m_Window.GetHWnd(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBox(m_Window.GetHWnd(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	if (bFullScreen) {
		KD3D.GetSwapChain()->SetFullscreenState(TRUE, 0);
	}

	// シェーダ管理クラス初期設定
	ShMgr.Init();

	// フォント管理システム初期設定
	KDFontMgr.Init(m_Window.GetHWnd());

	// フォント追加
	KDFontMgr.AddFont(0, "ＭＳ ゴシック", 12);
	KDFontMgr.AddFont(1, "ＭＳ ゴシック", 24);
	KDFontMgr.AddFont(2, "ＭＳ ゴシック", 36);


	//===================================================================
	// サウンド初期化
	//===================================================================
	if (KDAudioMgr.Init() == false) {

	}

	//===================================================================
	// 乱数シード初期化
	//===================================================================
	KdMTRand::s_Rnd.InitSeed(timeGetTime());

	//===================================================================
	// imgui
	//===================================================================
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(m_Window.GetHWnd());
	ImGui_ImplDX11_Init(KD3D.GetDev(), KD3D.GetDevContext());

	#include "ja_glyph_ranges.h"
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	//===================================================================
	// その他
	//===================================================================



	return true;
}

// アプリケーション終了
void Applicatoin::Release()
{
	// imgui解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// シェーダ管理クラス解放
	ShMgr.Release();

	// リソース管理クラス解放
	KDResFactory.Release();

	// フォント管理システム解放
	KDFontMgr.Release();

	// Direct3D解放
	KD3D.Release();

	// サウンド解放
	KDAudioMgr.Release();

	// ウィンドウ削除
	m_Window.Release();

}

/*
// (仮)クラス名からObject生成する関数
SPtr<Object> CreateObject(const std::string& className)
{
	SPtr<Object> p;
	if (className == "StageObject") {
		p = std::make_shared<StageObject>();
	}
	else if (className == "Chara") {
		p = std::make_shared<Chara>();
	}

	return p;
}
*/

// メインループ
void Applicatoin::Loop()
{

	//================================
	// ゲーム系のコンポーネントのクラスを登録
	//================================
	REGISTER_COMP_CLASS(CharaAIInputComponent);
	REGISTER_COMP_CLASS(PortalGunComponent);
	REGISTER_COMP_CLASS(PortalComponent);
	REGISTER_COMP_CLASS(BoxComponent);
	REGISTER_COMP_CLASS(GettingBoxComponent);
	REGISTER_COMP_CLASS(AutomaticDoorComponent);
	REGISTER_COMP_CLASS(SimpleCalculatorComponent);

	//================================
	// クラス登録
	//================================
	REGISTER_OBJECT_CLASS(Object);

	// ゲームマネージャ初期化
	GameMgr.Init();

	//

	DWORD baseTime = timeGetTime();
	int count = 0;

	// ゲームループ
	while (1) {

		// 処理開始時間Get
		DWORD st = timeGetTime();

		// ゲーム終了指定があるとき
		if (m_EndFlag) {
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_Window.ProcessMessage();

		// ウィンドウが破棄されてるなら、ループ終了
		if (m_Window.IsCreated() == false)break;


		//=========================================
		//
		// サウンド処理
		//
		//=========================================
		KDAudioMgr.Update();

		//=========================================
		//
		// ゲーム処理
		//
		//=========================================
		GameMgr.Run();


		//=========================================
		// 60 Fps制御
		//=========================================
		// 処理終了時間Get
		DWORD et = timeGetTime();
		// Fps制御
		int ms = 1000 / m_maxFps;
		if (et - st < ms) {
			Sleep(ms - (et - st));	// 速すぎたら待つ
		}

		// FPS計測
		count++;
		if (st - baseTime >= 1000) {
			m_Fps = (count * 1000) / (st - baseTime);
			baseTime = st;
			count = 0;
		}

	}

}