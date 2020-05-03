#pragma once

#include "BaseComponent.h"

// **************************
// �K�v�Ȏ�
// �E����炷�^�C�~���O�Ń\�[�X���ŔC�ӂ�Play��Stop�����o���鎖
// �E����炷���̊֐��̈����͖炵�������̃t�@�C����(�^�O�ɂȂ邩��)
// �EImGui��ŉ���炷�t�@�C�����w��o���鎖
// �E

class AudioComponent : public BaseComponent
{
public:
	//----------------------------
	// �ݒ�E�擾
	//----------------------------
	// �X�V����
	virtual void Update()override;

	// (Editor)ImGui����
	virtual void ImGuiUpdate() override;

	// (Editor)Editor�`��
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;

	// 
	// b3D�E�E�E3D�T�E���h�ɂ��邩�H
	void SetSound(const std::string & filename,bool b3D = false);
	void Play(bool loop);
	void Play3D(const KdVec3& pos,bool loop);
	void Stop();
	void Pause();
	void Resume();

private:
	// �t�@�C����
	std::string m_soundFilename;
	
	SPtr<KdSoundEffect> m_soundEffect;
	SPtr<KdSoundInstance> m_soundInstance;

	// �T�E���h�G�t�F�N�g�̐ݒ胁���o
	bool m_b3D = false;

	// �^�O
	std::string m_tag;

	// 3D�T�E���h�̏ꍇ�̃��[�J�����W
	KdVec3 m_pos;

	// �T�E���h�̋����������n�܂鋗���̐ݒ胁���o(���̐ݒ肵�������𒴂���ƌ������n�܂�)
	float m_CurveDistanceScale = FLT_MAX;

};

class AudioListenerComponent : public BaseComponent
{
public:
	//----------------------------
	// �ݒ�E�擾
	//----------------------------
	// �X�V����
	virtual void Update()override;

	// (Editor)ImGui����
	virtual void ImGuiUpdate() override;

	// (Editor)Editor�`��
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) override;

private:
};