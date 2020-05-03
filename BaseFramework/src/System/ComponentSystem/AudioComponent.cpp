#include "main.h"

#include "../GameManager.h"

#include "AudioComponent.h"


void AudioComponent::Update()
{
}

void AudioComponent::ImGuiUpdate()
{
	// 継承元クラスのImGuiUpdateを実行
	BaseComponent::ImGuiUpdate();

	ImGui::Checkbox(u8"3Dサウンド", &m_b3D);

	// サウンドファイル
	if (ImGuiResourceButton("Sound File", m_soundFilename, { ".wav" }))
	{
		SetSound(m_soundFilename);
	}

	// サウンドの設定
	ImGui::DragFloat(u8"サウンドの距離減衰開始", &m_CurveDistanceScale, 0.01f, 0.0f, 1000.0f);
}

void AudioComponent::EditorDraw()
{
	if (m_enable == false)return;
}

void AudioComponent::Deserialize(const json11::Json & jsonObj)
{
	// 継承元クラスのDeserializeを実行
	BaseComponent::Deserialize(jsonObj);

	// 
	// サウンドファイル
	std::string filename = jsonObj["SoundFilename"].string_value();
	bool b3D = jsonObj["3DSound"].bool_value();
	SetSound(filename,b3D);
	m_CurveDistanceScale = (float)jsonObj["CurveDistanceScale"].number_value();
	
	// タグ
	m_tag = jsonObj["Tag"].string_value();
}

void AudioComponent::Serialize(json11::Json::object & outJsonObj)
{
	// 継承元のSerializeを実行
	BaseComponent::Serialize(outJsonObj);
	// 自分のデータもJSON化
	outJsonObj["SoundFilename"] = m_soundFilename;
	outJsonObj["3DSound"] = m_b3D;
	outJsonObj["Tag"] = m_tag;
	outJsonObj["CurveDistanceScale"] = m_CurveDistanceScale;
}

void AudioComponent::SetSound(const std::string & filename,bool b3D)
{
	m_soundFilename = filename;
	m_b3D = b3D;

	// 音声を読み込む
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

// 停止
void AudioComponent::Stop()
{
	m_soundInstance->Stop();
}

// 一時停止
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
	// 継承元クラスのDeserializeを実行
	BaseComponent::Deserialize(jsonObj);
}

void AudioListenerComponent::Serialize(json11::Json::object & outJsonObj)
{
	// 継承元のSerializeを実行
	BaseComponent::Serialize(outJsonObj);
}
