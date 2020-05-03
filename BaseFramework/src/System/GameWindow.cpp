#include "main.h"
#include "GameWindow.h"

bool GameWindow::Create(HINSTANCE hInst, int clientWidth, int clientHeight, const std::string& windowClassName)
{
	//===================================================================
	// ���C���E�B���h�E�쐬
	//===================================================================

	//�E�B���h�E�N���X�̒�`
	WNDCLASSEX wc;											// �E�B���h�E�N���X�̒�`�p
	wc.cbSize = sizeof(WNDCLASSEX);							// �\���̂̃T�C�Y
	wc.style = 0;											// �X�^�C��
	wc.lpfnWndProc = &GameWindow::callWindowProc;			// �E�C���h�E�֐�
	wc.cbClsExtra = 0;										// �G�L�X�g���N���X��� 
	wc.cbWndExtra = 0;										// �G�L�X�g���E�B���h�E���
	wc.hInstance = hInst;									// �C���X�^���X�n���h��
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);			// ���[�W�A�C�R��
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);		// �X���[���A�C�R�� 
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);			// �}�E�X�J�[�\��
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// �w�i�F 
	wc.lpszMenuName = nullptr;								// ���C�����j���[��
	wc.lpszClassName = windowClassName.c_str();				// �E�B���h�E�N���X��

	//�E�B���h�E�N���X�̓o�^
	if (!RegisterClassEx(&wc)) {
		return false;
	}

	//�E�B���h�E�̍쐬
	m_hWnd = CreateWindow(
		windowClassName.c_str(),							// �E�B���h�E�N���X��
		"�|���^��",											// �E�B���h�E�̃^�C�g��
		WS_OVERLAPPEDWINDOW - WS_THICKFRAME,				// �E�B���h�E�^�C�v��W���^�C�v��	
		0,													// �E�B���h�E�̈ʒu�i�w���W�j
		0,													// �E�B���h�E�̈ʒu�i�x���W�j						
		clientWidth,										// �E�B���h�E�̕�
		clientHeight,										// �E�B���h�E�̍���			
		nullptr,											// �e�E�B���h�E�̃n���h��
		nullptr,											// ���j���[�̃n���h��
		hInst,												// �C���X�^���X�n���h�� 
		this);												// �ǉ���� 
	if (m_hWnd == nullptr) {
		return false;
	}

	// �E�B���h�E�v���p�e�B�ɂ��̃N���X�̃C���X�^���X�A�h���X���L�����Ă���
//	SetProp(m_hWnd, "GameWindowInstance", this);

	// �N���C�A���g�̃T�C�Y��ݒ�
	SetClientSize(clientWidth, clientHeight);

	//===================================================================
	//�E�B���h�E�̕\��
	//===================================================================
	ShowWindow(m_hWnd, SW_SHOW);
	//�E�B���h�E�̍X�V
	UpdateWindow(m_hWnd);

	// timeGetTime�֐��̐��x��1ms�ɐݒ�
	timeBeginPeriod(1);

	return true;
}

void GameWindow::Release()
{
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

bool GameWindow::ProcessMessage()
{
	m_MouseWheelVal = 0;

	// ���b�Z�[�W�擾
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// �I�����b�Z�[�W������
		if (msg.message == WM_QUIT) {
			return false;
		}

		//���b�Z�[�W����
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

// �E�B���h�E�֐�(Static�֐�)
LRESULT CALLBACK GameWindow::callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// �E�B���h�E�v���p�e�B����AGameWindow�N���X�̃C���X�^���X���擾
	GameWindow* pThis = (GameWindow*)GetProp(hWnd, "GameWindowInstance");

	// nullptr�̏ꍇ�́A�f�t�H���g���������s
	if (pThis == nullptr) {
		switch (message) {
			case WM_CREATE:
			{
				// CreateWindow()�œn�����p�����[�^���擾
				CREATESTRUCT * createStruct = (CREATESTRUCT*)lParam;
				GameWindow* window = (GameWindow*)createStruct->lpCreateParams;

				// �E�B���h�E�v���p�e�B�ɂ��̃N���X�̃C���X�^���X�A�h���X�𖄂ߍ���ł���
				// ���񂩂�ApThis->WindowProc�̕��֏���������Ă���
				SetProp(hWnd, "GameWindowInstance", window);

			}
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	// �C���X�^���X����Window�֐������s����
	return pThis->WindowProc(hWnd, message, wParam, lParam);
}

// imgui
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �E�B���h�E�֐�
LRESULT GameWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGui�ɃC�x���g�ʒm
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	//===================================
	//���b�Z�[�W�ɂ���ď�����I��
	//===================================
	switch (message) {
	// �z�C�[���X�N���[����
	case WM_MOUSEWHEEL:
		{
			m_MouseWheelVal = (short)HIWORD(wParam);
		}
		break;
	// X�{�^���������ꂽ
	case WM_CLOSE:
		// �j��
		Release();
		break;
	// �E�B���h�E�j�����O
	case WM_DESTROY:
		RemoveProp(hWnd, "GameWindowInstance");
		PostQuitMessage(0);
		break;
	default:
		// ���b�Z�[�W�̃f�t�H���g����
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �E�B���h�E�̃N���C�A���g�̃T�C�Y���w��T�C�Y�ɂ���B
void GameWindow::SetClientSize(int w, int h)
{
	RECT rcWnd, rcCli;

	GetWindowRect(m_hWnd, &rcWnd); // �E�B���h�E��RECT�擾
	GetClientRect(m_hWnd, &rcCli); // �N���C�A���g�̈��RECT�擾

	// �E�B���h�E�̗]�����l���āA�N���C�A���g�̃T�C�Y���w��T�C�Y�ɂ���B
	MoveWindow(m_hWnd,
		rcWnd.left,// X���W
		rcWnd.top,// Y���W
		w + (rcWnd.right - rcWnd.left) - (rcCli.right - rcCli.left),
		h + (rcWnd.bottom - rcWnd.top) - (rcCli.bottom - rcCli.top),
		TRUE);
}
