#include "main.h"

#include "../GameManager.h"

#include "AudioComponent.h"


void AudioComponent::Update()
{
}

void AudioComponent::ImGuiUpdate()
{
	// �p�����N���X��ImGuiUpdate�����s
	BaseComponent::ImGuiUpdate();

	ImGui::Checkbox(u8"3D�T�E���h", &m_b3D);

	// �T�E���h�t�@�C��
	if (ImGuiResourceButton("Sound File", m_soundFilename, { ".wav" }))
	{
		SetSound(m_soundFilename);
	}

	// �T�E���h�̐ݒ�
	ImGui::DragFloat(u8"�T�E���h�̋��������J�n", &m_CurveDistanceScale, 0.01f, 0.0f, 1000.0f);
}

void AudioComponent::EditorDraw()
{
	if (m_enable == false)return;
}

void AudioComponent::Deserialize(const json11::Json & jsonObj)
{
	// �p�����N���X��Deserialize�����s
	BaseComponent::Deserialize(jsonObj);

	// 
	// �T�E���h�t�@�C��
	std::string filename = jsonObj["SoundFilename"].string_value();
	bool b3D = jsonObj["3DSound"].bool_value();
	SetSound(filename,b3D);
	m_CurveDistanceScale = (float)jsonObj["CurveDistanceScale"].number_value();
	
	// �^�O
	m_tag = jsonObj["Tag"].string_value();
}

void AudioComponent::Serialize(json11::Json::object & outJsonObj)
{
	// �p������Serialize�����s
	BaseComponent::Serialize(outJsonObj);
	// �����̃f�[�^��JSON��
	outJsonObj["SoundFilename"] = m_soundFilename;
	outJsonObj["3DSound"] = m_b3D;
	outJsonObj["Tag"] = m_tag;
	outJsonObj["CurveDistanceScale"] = m_CurveDistanceScale;
}

void AudioComponent::SetSound(const std::string & filename,bool b3D)
{
	m_soundFilename = filename;
	m_b3D = b3D;

	// ������ǂݍ���
	m_soundEffect = KDResFactory.GetSound(filename);
	m_soundInstance = m_soundEffect->CreateInstance(m_b3D);

}

void AudioComponent::Play(bool loop)
{
	m_soundInstance->Play(loop);
}

void AudioComponent::Play3D(const KdVec3& pos,bool loop)
{
	m_soundInstance->Play3D(pos, loop);
	m_soundInstance->SetCurveDistanceScaler(m_CurveDistanceScale);
}

// ��~
void AudioComponent::Stop()
{
	m_soundInstance->Stop();
}

// �ꎞ��~
void AudioComponent::Pause()
{
	m_soundInstance->Pause();
}

void AudioComponent::Resume()
{
	m_soundInstance->Resume();
}

//---------------------------------------------------
// AudioLisnerComponent
//---------------------------------------------------
void AudioListenerComponent::Update()
{
	if (m_enable == false)return;
	KdVec3 pos = GetOwner()->GetMatrix().GetPos();
	KDAudioMgr.GetListener().SetPosition(pos);

}

void AudioListenerComponent::ImGuiUpdate()
{
}

void AudioListenerComponent::EditorDraw()
{
}

void AudioListenerComponent::Deserialize(const json11::Json & jsonObj)
{
	// �p�����N���X��Deserialize�����s
	BaseComponent::Deserialize(jsonObj);
}

void AudioListenerComponent::Serialize(json11::Json::object & outJsonObj)
{
	// �p������Serialize�����s
	BaseComponent::Serialize(outJsonObj);
}
