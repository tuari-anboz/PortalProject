#pragma once

#include "BaseComponent.h"

//==============================================
//
// Light�R���|�[�l���g�N���X
//
//==============================================
class LightComponent : public BaseComponent {
public:

	//----------------------------
	// �ݒ�E�擾
	//----------------------------

	// �f�B���N�V���i�����C�g�Ƃ��Đݒ�
	void SetDirectionalLight(const KdVec4& color, float intensity) {
		m_lightType = LightTypes::Directional;
		m_color = color;
		m_range = 0;
		m_intensity = intensity;
	}

	// �|�C���g���C�g�Ƃ��Đݒ�
	void SetPointLight(const KdVec4& color, float radius, float intensity) {
		m_lightType = LightTypes::Point;
		m_color = color;
		m_range = radius;
		m_intensity = intensity;
	}

	// �`��O����
	virtual void PreDraw() override;

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

	// ���C�g�̎�ޒ萔
	enum class LightTypes {
		Directional	= 0,
		Point		= 1,
		Spot		= 2,
	};
	// ���C�g�̎�ށ@0:���s���C�g 1:�|�C���g���C�g 2:�X�|�b�g���C�g
	LightTypes	m_lightType = LightTypes::Point;

	//
	KdVec4	m_color = {1, 1, 1, 0};		// �F
	float	m_range = 10;				// �_���͈̔�
	float	m_intensity = 1.0f;			// ���x

	float	m_minAngle = 10;			// �X�|�b�g���C�g�̍ŏ����a
	float	m_maxAngle = 15;			// �X�|�b�g���C�g�̍ő唼�a

};

