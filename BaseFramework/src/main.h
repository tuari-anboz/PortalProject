#pragma once

#define NOMINMAX
#include <windows.h>
#include <stdio.h>

//===========================
// KdLibrary
//===========================
#include "KdLibrary.h"

//===========================
// ���̑�
//===========================
#include "Shader/ShaderManager.h"
#include "System/GameWindow.h"
#include "System/DebugDrawSystem.h"
#include "System/EditorCamera.h"

#include "System/ImGuiHelper.h"


//============================================================
// �A�v���P�[�V�����N���X
//	�O���[�o���ϐ��Ȃǂ��W�߂��悤�ȃN���X
//	APP.�` �łǂ�����ł��A�N�Z�X�\
//============================================================
class Applicatoin{
// �����o
public:

	// �A�v���P�[�V����������
	bool Init(HINSTANCE hInst, int w, int h);

	// �Q�[�����[�v
	void Loop();

	// �A�v���P�[�V�������
	void Release();

	// �Q�[���I��
	void EndGame() {
		m_EndFlag = true;
	}

	//=================================================
	// ���J�f�[�^
	//=================================================
	
	// �Q�[���E�B���h�E�N���X
	GameWindow	m_Window;


	//=====================================================
	// �O���[�o���f�[�^
	//=====================================================

	HINSTANCE	m_hInst = nullptr;		// �C���X�^���X�n���h��
	
	int			m_Fps = 0;					// ���݂�FPS�l
	int			m_maxFps = 60;				// �ő�FPS


private:

	// �Q�[���I���t���O true�ŏI������
	bool	m_EndFlag = false;


//=====================================================
// �V���O���g���p�^�[��
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

// �ȒP�ɃA�N�Z�X�ł���悤�ɂ��邽�߂̃}�N��
#define APP Applicatoin::GetInstance()
