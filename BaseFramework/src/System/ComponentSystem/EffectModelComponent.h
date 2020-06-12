#pragma once

#include "BaseComponent.h"

//==============================================
//
// �G�t�F�N�g���f���R���|�[�l���g
//  �G�t�F�N�g�Ƃ���3D���f���\�����s��
//  �EDrawEffect�֐��ł̕`��(Z��������OFF)
//  �E���f���ƃe�N�X�`����ʎw��
//  �E�`�掞�Ƀr���{�[�f�B���O���s��
//
//==============================================
class EffectModelComponent : public BaseComponent {
public:

	//----------------------------
	// �ݒ�E�擾
	//----------------------------
	// ���f���ǂݍ���
	void LoadModel(const std::string& modelFilename, const std::string& texFilename) {
		m_modelFilename = modelFilename;
		m_texFilename = texFilename;
		m_model = KDResFactory.GetGameModel(m_modelFilename);
		m_tex = KDResFactory.GetTexture(m_texFilename);
	}

	// GameModel�擾
	const SPtr<KdGameModel>& GetGameModel() const { return m_model; }

	//----------------------------
	// �`�揈��
	//----------------------------
	// 3D�`��
	virtual void DrawEffect() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseComponent::Deserialize(jsonObj);
		m_modelFilename = jsonObj["ModelFilename"].string_value();
		m_texFilename = jsonObj["TexFilename"].string_value();
		m_billboardType = jsonObj["BillboardType"].int_value();
		m_blendType = jsonObj["BlendType"].int_value();
		m_color.Deserialize(jsonObj["Color"]);

		LoadModel(m_modelFilename, m_texFilename);
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["ModelFilename"] = m_modelFilename;
		outJsonObj["TexFilename"] = m_texFilename;
		outJsonObj["BillboardType"] = m_billboardType;
		outJsonObj["BlendType"] = m_blendType;
		outJsonObj["Color"] = m_color.Serialize();
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;

private:

	//--------------------------
	// �`��p���f���f�[�^
	//--------------------------
	std::string			m_modelFilename;		// ���f���t�@�C����
	SPtr<KdGameModel>	m_model;				// ���f���f�[�^

	std::string			m_texFilename;			// �e�N�X�`���t�@�C����
	SPtr<KdTexture>		m_tex;					// �e�N�X�`���f�[�^

	KdVec4				m_color = { 1,1,1,1 };	// ��Z�F

	int					m_billboardType = 0;	// �r���{�[�h�̎��
	int					m_blendType = 0;		// �������[�h
};

