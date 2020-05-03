#pragma once



//=====================================================
//
// �Q�[���E�B���h�E�N���X
//
//  [��ȋ@�\]
//�@�E�E�B���h�E�̍쐬
//
//=====================================================
class GameWindow {
public:

	// �E�B���h�E�n���h���擾
	HWND GetHWnd() const { return m_hWnd; }
	// �E�B���h�E�����݂���H
	bool IsCreated() const { return m_hWnd ? true : false; }
	// �}�E�X�z�C�[���̕ω��ʂ��擾
	int GetMouseWheelVal() const { return m_MouseWheelVal; }

	// �E�B���h�E�쐬
	bool Create(HINSTANCE hInst, int clientWidth, int clientHeight, const std::string& windowClassName);

	// ���
	void Release();

	// �E�B���h�E���b�Z�[�W����������
	//  �߂�l�F�I�����b�Z�[�W��������false���Ԃ�
	bool ProcessMessage();

	// �N���C�A���g�T�C�Y�̐ݒ�
	void SetClientSize(int w, int h);

	//
	~GameWindow() {
		Release();
	}

private:

	// �E�B���h�E�n���h��
	HWND	m_hWnd = nullptr;

	// �E�B���h�E�֐�
	static LRESULT CALLBACK callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// �}�E�X�z�C�[���l
	int		m_MouseWheelVal = 0;

};

