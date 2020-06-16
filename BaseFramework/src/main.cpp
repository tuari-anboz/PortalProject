#include "main.h"

#include "System/GameManager.h"

//===================================================================
// ���C��
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{

	// ���������[�N��m�点��
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM������
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// mbstowcs_s�֐��œ��{��Ή��ɂ��邽�߂ɌĂ�
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// �E�B���h�E�쐬�ADirect3D�������Ȃ�
	//===================================================================
	if (APP.Init(hInstance, 1280, 720) == false) {
		return 0;
	}

	//===================================================================
	// ���[�v
	//===================================================================
	APP.Loop();

	//===================================================================
	// ���
	//===================================================================
	APP.Release();

	// COM���
	CoUninitialize();

	return 0;
}

// �A�v���P�[�V���������ݒ�
bool Applicatoin::Init(HINSTANCE hInst, int w, int h)
{
	// �C���X�^���X�n���h���L��
	APP.m_hInst = hInst;

	//===================================================================
	// �E�B���h�E�쐬
	//===================================================================
	if (m_Window.Create(hInst, w, h, "Window") == false) {
		MessageBox(nullptr, "�E�B���h�E�쐬�Ɏ��s", "�G���[", MB_OK);
		return false;
	}

	//===================================================================
	// �t���X�N���[���m�F
	//===================================================================
	bool bFullScreen = false;
	if (MessageBox(m_Window.GetHWnd(), "�t���X�N���[���ɂ��܂����H", "�m�F", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}


	//===================================================================
	// Direct3D������
	//===================================================================

	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	std::string errorMsg;
	if (KD3D.Init(m_Window.GetHWnd(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBox(m_Window.GetHWnd(), errorMsg.c_str(), "Direct3D���������s", MB_OK | MB_ICONSTOP);
		return false;
	}

	if (bFullScreen) {
		KD3D.GetSwapChain()->SetFullscreenState(TRUE, 0);
	}

	// �V�F�[�_�Ǘ��N���X�����ݒ�
	ShMgr.Init();

	// �t�H���g�Ǘ��V�X�e�������ݒ�
	KDFontMgr.Init(m_Window.GetHWnd());

	// �t�H���g�ǉ�
	KDFontMgr.AddFont(0, "�l�r �S�V�b�N", 12);
	KDFontMgr.AddFont(1, "�l�r �S�V�b�N", 24);
	KDFontMgr.AddFont(2, "�l�r �S�V�b�N", 36);


	//===================================================================
	// �T�E���h������
	//===================================================================
	if (KDAudioMgr.Init() == false) {

	}

	//===================================================================
	// �����V�[�h������
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
	// ���̑�
	//===================================================================



	return true;
}

// �A�v���P�[�V�����I��
void Applicatoin::Release()
{
	// imgui���
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// �V�F�[�_�Ǘ��N���X���
	ShMgr.Release();

	// ���\�[�X�Ǘ��N���X���
	KDResFactory.Release();

	// �t�H���g�Ǘ��V�X�e�����
	KDFontMgr.Release();

	// Direct3D���
	KD3D.Release();

	// �T�E���h���
	KDAudioMgr.Release();

	// �E�B���h�E�폜
	m_Window.Release();

}

/*
// (��)�N���X������Object��������֐�
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

// ���C�����[�v
void Applicatoin::Loop()
{

	//================================
	// �Q�[���n�̃R���|�[�l���g�̃N���X��o�^
	//================================
	REGISTER_COMP_CLASS(CharaAIInputComponent);
	REGISTER_COMP_CLASS(PortalGunComponent);
	REGISTER_COMP_CLASS(PortalComponent);
	REGISTER_COMP_CLASS(BoxComponent);
	REGISTER_COMP_CLASS(GettingBoxComponent);
	REGISTER_COMP_CLASS(AutomaticDoorComponent);
	REGISTER_COMP_CLASS(SimpleCalculatorComponent);

	//================================
	// �N���X�o�^
	//================================
	REGISTER_OBJECT_CLASS(Object);

	// �Q�[���}�l�[�W��������
	GameMgr.Init();

	//

	DWORD baseTime = timeGetTime();
	int count = 0;

	// �Q�[�����[�v
	while (1) {

		// �����J�n����Get
		DWORD st = timeGetTime();

		// �Q�[���I���w�肪����Ƃ�
		if (m_EndFlag) {
			break;
		}

		//=========================================
		//
		// �E�B���h�E�֌W�̏���
		//
		//=========================================

		// �E�B���h�E�̃��b�Z�[�W����������
		m_Window.ProcessMessage();

		// �E�B���h�E���j������Ă�Ȃ�A���[�v�I��
		if (m_Window.IsCreated() == false)break;


		//=========================================
		//
		// �T�E���h����
		//
		//=========================================
		KDAudioMgr.Update();

		//=========================================
		//
		// �Q�[������
		//
		//=========================================
		GameMgr.Run();


		//=========================================
		// 60 Fps����
		//=========================================
		// �����I������Get
		DWORD et = timeGetTime();
		// Fps����
		int ms = 1000 / m_maxFps;
		if (et - st < ms) {
			Sleep(ms - (et - st));	// ����������҂�
		}

		// FPS�v��
		count++;
		if (st - baseTime >= 1000) {
			m_Fps = (count * 1000) / (st - baseTime);
			baseTime = st;
			count = 0;
		}

	}

}