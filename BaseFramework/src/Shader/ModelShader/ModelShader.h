#pragma once

//------------------------
// ���f���V�F�[�_�N���X
//------------------------
class ModelShader
{
public:

	// ���[���h�s����Z�b�g
	void SetWorld(const KdMatrix& m)
	{
		m_cb0.GetWork().mW = m;
	}

	// GameModel��`��
	void DrawGameModel(const KdGameModel& gmodel,const KdBoneController* bc = nullptr)
	{
		// �{�[���s����Z�b�g
		if (bc)
		{
			bc->GetBoneConstantBuffer().VSSetConstantBuffer(2);
		}
		// �S���f���f�[�^��`��
		for (auto& model : gmodel.GetModels())
		{
			DrawModel(*model);
		}
	}

	// ���C�gOn/Off
	void SetLightEnable(bool enable)
	{
		m_cb0.GetWork().LightEnable = enable ? 1 : 0;
	}
	// �e���ݒ�(0�`1)
	void SetRoughness(float roughness)
	{
		m_cb0.GetWork().Roughness = roughness;
	}
	// �����x�ݒ�(0�`1)
	void SetMetallic(float metallic)
	{
		m_cb0.GetWork().Metallic = metallic;
	}
	// ���ʃm�[�}���}�b�v�Z�b�g
	void SetForceUseNormalMap(const SPtr<KdTexture>& forceUseNormalMap)
	{
		m_forceUseNormalMap = forceUseNormalMap;
	}
	// �����F�Z�b�g
	void SetMulColor(const KdVec4& mulcolor)
	{
		m_cb0.GetWork().MulColor = mulcolor;
	}
	// ���ܕ\��On/Off
	void SetRefractEnable(bool enable)
	{
		m_cb0.GetWork().RefractEnable = enable;
	}
	// �t�H�O��On/Off
	void SetFogEnable(bool enable)
	{
		m_cb0.GetWork().FogEnable = enable;
	}

	// KdModel��`��
	void DrawModel(const KdModel& model);

	// ������
	void Init();
	// ���
	void Release();
	~ModelShader() { Release(); }

private:
	ID3D11VertexShader*		m_VS = nullptr;	// ���_�V�F�[�_
	ID3D11PixelShader*		m_PS = nullptr;	// �s�N�Z���V�F�[�_
	ID3D11InputLayout*		m_VLayout = nullptr;	// ���_���̓��C�A�E�g
	// ��SkinMesh�p���_�V�F�[�_�Ɠ��̓��C�A�E�g
	ID3D11VertexShader*		m_SkinVS = nullptr;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_SkinVLayout = nullptr;	// ���_���̓��C�A�E�g
	
	// �����I�Ɏg�p����@���}�b�v
	// �����Ƀe�N�X�`�������Ă����ƁA�}�e���A���̖@���}�b�v��
	// �g�p���ꂸ�A�������g�p����
	SPtr<KdTexture>		m_forceUseNormalMap;


	// �萔�o�b�t�@(�I�u�W�F�N�g)
	struct cbObject
	{
		KdMatrix mW; // ���[���h�s��
		KdVec4 MulColor = { 1,1,1,1 };		// �����F
		int LightEnable = 1;	// ���C�gOn/Off
		float Roughness = 1.0f;	// �e��
		float Metallic = 0;		// �����x
		int RefractEnable = 0;	// ���ܕ\��On/Off
		int FogEnable = true;	// �t�H�OOn/Off
		float tmp[3];			// �p�b�L���O�K���p�̃S�~
	};
	KdConstantBuffer<cbObject> m_cb0;

	// �萔�o�b�t�@(�}�e���A��)
	struct cbMaterial
	{
		KdVec4 Diffuse;		// �g�U�F
		KdVec3 Specular;	// ���ːF
		float  SpePower;	// ���˂̉s��
		KdVec3 Emissive;	// �����F
		float  tmp;
	};
	KdConstantBuffer<cbMaterial> m_cb1;

	// �萔�o�b�t�@(���C�g)
};