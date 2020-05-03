#pragma once

#include "BaseComponent.h"


//==============================================
//
// 2D�摜�R���|�[�l���g�N���X
//
//==============================================
class SpriteComponent : public BaseComponent {
public:

	//----------------------------
	// �ݒ�E�擾
	//----------------------------

	// �����ݒ�
	void LoadTex(const std::string& texFilename);

	// 2D�`�揈��
	virtual void DrawSprite() override;

	// (Editor)ImGui����
	virtual void ImGuiUpdate() override;

	// �e�N�X�`���擾
	SPtr<KdTexture> GetTexture() { return m_tex; }

	// �e�N�X�`���ύX
	void ChangeTex(const SPtr<KdTexture>& tex) { if (tex == nullptr)return; m_tex = tex; }

	// �e�N�X�`����̍��W���ǂ����𔻒肵�܂�
	bool IsOnTexture(const KdVec2& pos);

	// �e�N�X�`���̓����x�ݒ�
	void SetTexAlpha(const float alpha) { m_texAlpha = alpha; }

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override;


private:
	// �摜�t�@�C����
	std::string			m_texFilename;

	// �e�N�X�`���f�[�^
	SPtr<KdTexture>		m_tex;

	// �e�N�X�`���̓����x
	float m_texAlpha = 1.0f;

};

