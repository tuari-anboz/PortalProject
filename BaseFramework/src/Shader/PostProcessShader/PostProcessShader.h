// PostProcessShader.h
#pragma once

// �u���[�e�N�X�`��
struct BlurTexture
{
	// �e�N�X�`���쐬
	void Create(int w, int h)
	{
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			// �����_�[�^�[�Q�b�g�e�N�X�`��(RT)�Ƃ��č쐬
			m_rt[i][0].CreateRT(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			m_rt[i][1].CreateRT(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
		//m_rt[0].CreateRT(w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM);
		//m_rt[1].CreateRT(w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	// ��Ɨp�����_�[�^�[�Q�b�g�e�N�X�`��
	KdTexture m_rt[5][2];
	//KdTexture m_rt[2];
};

// �g�e�N�X�`��
struct WaveTexture
{
	// �����}�b�v
	SPtr<KdTexture> m_rtHeight[2];
	// �@���}�b�v
	SPtr<KdTexture> m_rtNormal;

	void Create(int w, int h)
	{
		m_rtHeight[0] = std::make_shared<KdTexture>();
		m_rtHeight[1] = std::make_shared<KdTexture>();
		m_rtNormal = std::make_shared<KdTexture>();

		m_rtHeight[0]->CreateRT(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtHeight[1]->CreateRT(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtNormal->CreateRT(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		// �����}�b�v�N���A
		m_rtHeight[0]->ClearRT({ 0,0,0,0 });
		m_rtHeight[1]->ClearRT({ 0,0,0,0 });
	}
};

class PostProcessShader
{
public:
	// �e�N�X�`����S��ʂ�2D�R�s�[�`��
	// �Etex	�E�E�E�\�[�X�摜
	void CopyDraw(const KdTexture& tex, const KdVec4& color = { 1,1,1,1 });

	void ToneMappingDraw(const KdTexture& tex);

	// �e�N�X�`���̖��邳1�ȏ�𒊏o����`��
	// �Etex	�E�E�E�\�[�X�摜
	void BrightFilteringDraw(const KdTexture& tex);

	// �e�N�X�`�����ڂ����`��
	// �Edir	�c�ڂ��������x�N�g��
	void BlurDraw(const KdTexture& tex,const KdVec2& dir);

	// �ڂ����摜����
	// �Edest	�c�ڂ������摜������e�N�X�`��
	// �Etex	�c�ڂ������摜
	void GenerateBlur(BlurTexture& dest, const KdTexture& tex);

	// �g�i�s
	// wave�̔g�e�N�X�`���̎��Ԃ�i�߂�
	void AdvanceWave(WaveTexture& wave);

	// �������֐�
	void Init();
	// ���
	void Release()
	{
		KdSafeRelease(m_VS);
		KdSafeRelease(m_VLayout);
		KdSafeRelease(m_CopyPS);
		KdSafeRelease(m_BlurPS);
		KdSafeRelease(m_BrightFilteringPS);
		KdSafeRelease(m_WaveHeightPS);
		KdSafeRelease(m_WaveNormalPS);
		m_cb0_Blur.Release();
	}

	~PostProcessShader()
	{
		Release();
	}

private:
	// ���_�V�F�[�_
	ID3D11VertexShader* m_VS = nullptr;
	ID3D11InputLayout* m_VLayout = nullptr;

	// �s�N�Z���V�F�[�_(2D�R�s�[)
	ID3D11PixelShader* m_CopyPS = nullptr;

	// �s�N�Z���V�F�[�_(�u���[)
	ID3D11PixelShader* m_BlurPS = nullptr;
	// �s�N�Z���V�F�[�_(���P�x���o)
	ID3D11PixelShader* m_BrightFilteringPS = nullptr;
	// �s�N�Z���V�F�[�_(ToneMapping)
	ID3D11PixelShader* m_ToneMappingPS = nullptr;
	// �g�p �����}�b�v�X�V�V�F�[�_
	ID3D11PixelShader* m_WaveHeightPS = nullptr;
	// �g�p �@���}�b�v�X�V�V�F�[�_
	ID3D11PixelShader* m_WaveNormalPS = nullptr;

	// �萔�o�b�t�@(Copy)
	struct cbCopy
	{
		KdVec4 Color = { 1,1,1,1 };
	};
	KdConstantBuffer<cbCopy>	m_cb0_Copy;

	// �萔�o�b�t�@(Blur)
	struct cbBlur
	{
		KdVec4 Offset[31];
	};
	KdConstantBuffer<cbBlur>		m_cb0_Blur;

	// �萔�o�b�t�@(Wave)
	struct cbWave
	{
		KdVec2	AddPowerUV;		// �͂�������UV���W
		float	AddPowerRadius;	// ���̔��a
		float	AddPower;		// �З�
	};
	KdConstantBuffer<cbWave>		m_cb0_Wave;
};