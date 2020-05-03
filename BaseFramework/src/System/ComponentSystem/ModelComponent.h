#pragma once

#include "BaseComponent.h"

//====================================
//
// 3D���f���`��A�A�j���[�V�����A�{�[���v�Z���s���R���|�[�l���g
//
// ����K�v�ȕ��A���ݓ��쒆�̃A�j���[�V������(���O�ƃA�j���[�V�����ʒu)�ۑ�
//
//====================================
class ModelComponent : public BaseComponent {
public:
	//=============================
	// �ݒ�E�擾
	//=============================
	// ���f���f�[�^�擾
	SPtr<KdGameModel> GetModel() { return m_model; }
	// ���f���f�[�^�ݒ�
	void SetModel(const std::string& filename);
	// �{�[���R���g���[���擾
	KdBoneController& GetBc() { return m_bc; }
	// �A�j���[�^�擾
	KdAnimator& GetAnimator() { return m_anim; }
	// �X�N���v�gl�L�[�R�[���o�b�N�֐��Z�b�g
	void SetScriptProc(std::function<void(KdAnimeKey_Script*)> proc)
	{
		m_onScriptProc = proc;
	}
	// ���f���̔����F�}�b�v��ύX
	void ChangeEmissiveMap();

	//=============================
	// ����
	//=============================

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw() override;
	// �������`�揈��
	virtual void DrawTransparency() override;
	// �V���h�E�}�b�v�`�揈��
	virtual void DrawShadowMap() override;
	// (Editor)GUI����
	virtual void ImGuiUpdate() override;

	// (Editor�p)�`��
	virtual void EditorDraw() override;

	//=============================
	// Deserialize / Serialize
	//=============================
	virtual void Deserialize(const json11::Json& jsonObj) override {
		// �p������Deserialize�����s
		BaseComponent::Deserialize(jsonObj);

		// ���f���t�@�C��
		std::string filename = jsonObj["ModelFilename"].string_value();
		SetModel(filename);

		// �A�j���֌W
		m_playAnimename = jsonObj["PlayAnimename"].string_value();
		m_loopAnime = jsonObj["LoopAnime"].bool_value();
		m_anim.ChangeAnime(m_playAnimename, m_loopAnime);

		//
		m_roughness = (float)jsonObj["Roughness"].number_value();
		m_lightEnable = jsonObj["LightEnable"].bool_value();
		m_metallic = (float)jsonObj["Metallic"].number_value();
		m_enableWater = jsonObj["EnableWater"].bool_value();
		m_enableRefract = jsonObj["EnableRefract"].bool_value();
		if (jsonObj["EnableFog"].is_bool())
		{
			m_enableFog = jsonObj["EnableFog"].bool_value();
		}
	}

	// �����̃N���X�f�[�^��outJson�֊i�[����
	virtual void Serialize(json11::Json::object& outJson) override {
		// �p������Serialize�����s
		BaseComponent::Serialize(outJson);
		// �����̃f�[�^��JSON��
		outJson["ModelFilename"] = m_modelFilename;
		outJson["PlayAnimename"] = m_playAnimename;
		outJson["LoopAnime"] = m_loopAnime;
		outJson["Roughness"] = m_roughness;
		outJson["LightEnable"] = m_lightEnable;
		outJson["Metallic"] = m_metallic;
		outJson["EnableWater"] = m_enableWater;
		outJson["EnableRefract"] = m_enableRefract;
		outJson["EnableFog"] = m_enableFog;
	}

private:
	// 3D���b�V�����f��
	SPtr<KdGameModel>	m_model;
	std::string			m_modelFilename;
	// �{�[������p
	KdBoneController	m_bc;
	// �A�j���[�^
	KdAnimator			m_anim;
	std::string			m_playAnimename;	// �Đ�����A�j����
	bool				m_loopAnime = true;	// ���[�v�Đ�

	//=======================================
	// �O���t�B�b�N�X�ݒ�
	//=======================================
	// ���C�g��On/Off
	bool				m_lightEnable = true;
	// �����F�}�b�v���g�p���邩���Ȃ���		
	// �e��
	float				m_roughness = 1;
	// �����x
	float				m_metallic = 0;
	// ���ʕ\��
	bool				m_enableWater = false;
	// ���ܕ\��
	bool	m_enableRefract = false;
	// �t�H�O��On/Off
	bool	m_enableFog = true;
	// �����F�w��
	KdVec4 m_mulColor = { 1,1,1,1 };

	

	// �X�N���v�g�L�[�p�R�[���o�b�N�֐�
	std::function<void(KdAnimeKey_Script*)>	m_onScriptProc;

};
