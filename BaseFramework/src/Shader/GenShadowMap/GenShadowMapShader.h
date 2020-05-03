#pragma once

class GenShadowMapShader
{
public:
	// �擾
	const SPtr<KdTexture>& GenDepthMap() { return m_depthMap; }

	// ���[���h�s����Z�b�g
	void SetWorld(const KdMatrix& m)
	{
		m_cb0.GetWork().mW = m;
	}

	// GameMpdel��`��
	void DrawGameModelDepth(const KdGameModel& gmodel, const KdBoneController* bc = nullptr)
	{
		if (bc)
		{
			bc->GetBoneConstantBuffer().VSSetConstantBuffer(1);
		}
		// �S���f���f�[�^��`��
		for (auto& model : gmodel.GetModels())
		{
			DrawModelDepth(*model);
		}
	}

	// �[�x�`��
	void DrawModelDepth(const KdModel& model);

	// �[�x�}�b�v����
	void Generate(std::function<void()> drawProc);

	// ������
	void Init();

	// ���
	void Release()
	{
		KdSafeRelease(m_VS);
		KdSafeRelease(m_VLayout);
		KdSafeRelease(m_PS);

		KdSafeRelease(m_SkinVS);
		KdSafeRelease(m_SkinVLayout);

		m_cb0.Release();
	}

	~GenShadowMapShader()
	{
		Release();
	}

private:
	// �f�[�^
	SPtr<KdTexture>		m_depthMap;		// �[�x�}�b�v
	SPtr<KdTexture>		m_zBuffer;		// Z�o�b�t�@

	// �e�V�F�[�_
	ID3D11VertexShader*		m_VS = nullptr;		// ���_�V�F�[�_
	ID3D11InputLayout*		m_VLayout = nullptr;		// ���_���C�A�E�g
	ID3D11PixelShader*		m_PS = nullptr;		// �s�N�Z���V�F�[�_
	// ��SkinMesh�p���_�V�F�[�_�Ɠ��̓��C�A�E�g
	ID3D11VertexShader*		m_SkinVS = nullptr;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_SkinVLayout = nullptr;	// ���_���̓��C�A�E�g

	// �萔�o�b�t�@(�I�u�W�F�N�g�P��)
	struct cbObject
	{
		KdMatrix mW;		// ���[���h�s��
	};
	KdConstantBuffer<cbObject> m_cb0;
};