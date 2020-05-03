#include "main.h"

#include "PostProcessShader.h"

void PostProcessShader::CopyDraw(const KdTexture & tex,const KdVec4& color)
{
	// �萔�o�b�t�@
	m_cb0_Copy.GetWork().Color = color;
	m_cb0_Copy.Write();
	m_cb0_Copy.GetBuffer().PSSetConstantBuffer(0);

	// �e�V�F�[�_���Z�b�g
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_CopyPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// �e�N�X�`���Z�b�g
	tex.PSSetShaderResource(0);
	// ���_�f�[�^
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// �v���~�e�B�u�E�g�|���W�[
	// (�`�掞�̒��_�f�[�^�̎g�p���@)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// ���_�`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪�A�h���X
		sizeof(KdVertex_Pos_UV)		// 1���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������(�|�X�g�v���Z�X�`��̏ꍇ�͕K���������Ă���)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::ToneMappingDraw(const KdTexture & tex)
{
	// �e�V�F�[�_���Z�b�g
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_ToneMappingPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// �e�N�X�`���Z�b�g
	tex.PSSetShaderResource(0);
	// ���_�f�[�^
	KdVertex_Pos_UV v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};
	// �v���~�e�B�u�E�g�|���W�[
	// (�`�掞�̒��_�f�[�^�̎g�p���@)
//	KD3D.GetDevContext()->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//	);

	// ���_�`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪�A�h���X
		sizeof(KdVertex_Pos_UV)	// �P���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::BrightFilteringDraw(const KdTexture & tex)
{
	// �e�V�F�[�_���Z�b�g
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_BrightFilteringPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// �e�N�X�`���Z�b�g
	tex.PSSetShaderResource(0);
	// ���_�f�[�^
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// �v���~�e�B�u�E�g�|���W�[
	// (�`�掞�̒��_�f�[�^�̎g�p���@)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// ���_�`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪�A�h���X
		sizeof(KdVertex_Pos_UV)		// 1���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������(�|�X�g�v���Z�X�`��̏ꍇ�͕K���������Ă���)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::BlurDraw(const KdTexture & tex, const KdVec2& dir)
{
	// �萔�o�b�t�@
	m_cb0_Blur.GetBuffer().PSSetConstantBuffer(0);

	// �e�N�Z���T�C�Y�Z�o(1�s�N�Z����UV���W�n�ł̃T�C�Y)
	float totalW = 0;
	constexpr float dispersion = 0.2f;
	KdVec2 ts;
	ts.x = 1.0f / tex.GetInfo().Width;
	ts.y = 1.0f / tex.GetInfo().Height;

	for (int i = 0; i < 31; i++)
	{
		int pt = i - 15;		// -15 �` +15
		// �I�t�Z�b�g���W
		m_cb0_Blur.GetWork().Offset[i].x = dir.x * (pt * ts.x);
		m_cb0_Blur.GetWork().Offset[i].y = dir.y * (pt * ts.y);
		// �d��
		//m_cb0_Blur.GetWork().Offset[i].z = 1.0f / 31;
		m_cb0_Blur.GetWork().Offset[i].z = exp(-(pt*pt)*dispersion) / sqrt(3.141592f * 2);
		totalW += m_cb0_Blur.GetWork().Offset[i].z;
	}

	// �d�݂𐳋K��
	for (int i = 0; i < 31; i++)
	{
		m_cb0_Blur.GetWork().Offset[i].z /= totalW; // ���K�� 
	}

	m_cb0_Blur.Write();		// GPU���֓]��

	// �e�V�F�[�_���Z�b�g
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_BlurPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// �e�N�X�`���Z�b�g
	tex.PSSetShaderResource(0);
	// ���_�f�[�^
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// �v���~�e�B�u�E�g�|���W�[
	// (�`�掞�̒��_�f�[�^�̎g�p���@)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// ���_�`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪�A�h���X
		sizeof(KdVertex_Pos_UV)		// 1���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������(�|�X�g�v���Z�X�`��̏ꍇ�͕K���������Ă���)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::GenerateBlur(BlurTexture & dest, const KdTexture & tex)
{
	// ���݂�RT�AZ�o�b�t�@���L�����āA��ŕ���������
	KdRTSaver rtSave;

	// RT,Z�o�b�t�@�ύX�p
	KdRTChanger rtc;

	//// �@�_�E���T�C�W���O
	//rtc.RT(0, dest.m_rt[0]);		// �ύX������RT
	//rtc.Depth(nullptr);				// �ύX������Z�o�b�t�@
	//rtc.SetToDevice();				// ���ۂɕύX���s��

	//CopyDraw(tex);

	//// �AX�u���[
	//rtc.RT(0, dest.m_rt[1]);
	////rtc.Depth(nullptr);
	//rtc.SetToDevice();

	//BlurDraw(dest.m_rt[0], { 1,0 });

	//// �BY�u���[
	//rtc.RT(0, dest.m_rt[0]);
	////rtc.Depth(nullptr);
	//rtc.SetToDevice();

	//BlurDraw(dest.m_rt[1], { 0,1 });

	for (int i = 0; i < 5; i++)
	{
		// �@�_�E���T�C�W���O
		rtc.RT(0, dest.m_rt[i][0]);		// �ύX������RT
		rtc.Depth(nullptr);				// �ύX������Z�o�b�t�@
		rtc.SetToDevice();				// ���ۂɕύX���s��

		if (i == 0)
		{
			CopyDraw(tex);
		}
		else
		{
			CopyDraw(dest.m_rt[i - 1][0]);
		}

		// �AX�u���[ [0]��[1]��
		rtc.RT(0, dest.m_rt[i][1]);
		//rtc.Depth(nullptr);
		rtc.SetToDevice();

		BlurDraw(dest.m_rt[i][0], { 1,0 });

		// �BY�u���[ [1]��[0]��
		rtc.RT(0, dest.m_rt[i][0]);
		//rtc.Depth(nullptr);
		rtc.SetToDevice();

		BlurDraw(dest.m_rt[i][1], { 0,1 });
	}

}

void PostProcessShader::AdvanceWave(WaveTexture & wave)
{
	// RT.Z���L���p
	KdRTSaver rtSave;
	// RT,Z��ύX�p
	KdRTChanger rtc;
	rtc.Depth(nullptr);

	// ���ʂɃ����_���ŗ͂�������
	m_cb0_Wave.GetWork().AddPowerUV.x =
		KdMTRand::s_Rnd.GetFloat(0, 1);
	m_cb0_Wave.GetWork().AddPowerUV.y =
		KdMTRand::s_Rnd.GetFloat(0, 1);
	m_cb0_Wave.GetWork().AddPowerRadius =
		KdMTRand::s_Rnd.GetFloat(1.0f / 512, 3.0f / 512);
	m_cb0_Wave.GetWork().AddPower =
		KdMTRand::s_Rnd.GetFloat(-0.3f, 0.3f);
	m_cb0_Wave.Write();
	m_cb0_Wave.GetBuffer().PSSetConstantBuffer(0);

	//---------------------------------
	// �@�����E�����}�b�v�X�V
	//---------------------------------
	rtc.RT(0, *wave.m_rtHeight[0]);
	rtc.SetToDevice();
	// ���̓e�N�X�`��
	wave.m_rtHeight[1]->PSSetShaderResource(0);

	// �e�V�F�[�_���Z�b�g
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_WaveHeightPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// ���_�f�[�^
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// �v���~�e�B�u�E�g�|���W�[
	// (�`�掞�̒��_�f�[�^�̎g�p���@)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// ���_�`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪�A�h���X
		sizeof(KdVertex_Pos_UV)		// 1���_�̃o�C�g�T�C�Y
	);

	// [0]��[1]�����ւ���
	std::swap(wave.m_rtHeight[0], wave.m_rtHeight[1]);

	//---------------------------------
	// �A�@���}�b�v�X�V
	//---------------------------------
	rtc.RT(0, *wave.m_rtNormal);
	rtc.SetToDevice();
	// �ŐV�̍����}�b�v���Z�b�g
	wave.m_rtHeight[1]->PSSetShaderResource(0);
	// PS�Z�b�g
	KD3D.GetDevContext()->PSSetShader(m_WaveNormalPS, 0, 0);
	// �`��
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// �g�p���钸�_��
		&v[0],	// ���_�z��̐擪address
		sizeof(KdVertex_Pos_UV)	// 1���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������(�|�X�g�v���Z�X�`��̏ꍇ�͕K���������Ă���)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::Init()
{
	Release();

	// ���_�V�F�[�_
	{
#include "PostProcess_VS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// �V�F�[�_�f�[�^�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			nullptr,
			&m_VS))						// �쐬���ꂽ�V�F�[�_������
			) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
		}

		// ���_���̓��C�A�E�g�̍쐬
		auto verDecl = KdVertex_Pos_UV::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&verDecl.m_Layout[0],		// ���_���C�A�E�g�f�[�^�̐擪address
			verDecl.m_Layout.size(),			// ���C�A�E�g�̍��ڐ�
			compiledBuffer,				// ���_�V�F�[�_�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			&m_VLayout))				// �����ɍ쐬���ꂽ���̂�����
			) {
			assert(0 && "���_���C�A�E�g�쐬���s");
		}
	}

	// �s�N�Z���V�F�[�_(2D Copy)
	{
		#include "PostProcess_Copy_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_CopyPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}
	// �u���[�V�F�[�_(2D Blur)
	{
		#include "PostProcess_Blur_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_BlurPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}
	// ���P�x���o(2D BrightFiltering)
	{
		#include "PostProcess_BrightFiltering_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_BrightFilteringPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}
	// �s�N�Z���V�F�[�_(ToneMapping)
	{
#include "PostProcess_ToneMapping_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_ToneMappingPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}
	// �s�N�Z���V�F�[�_(WaveHeight)
	{
		#include "PostProcess_WaveHeight_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_WaveHeightPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}
	// �s�N�Z���V�F�[�_(WaveNormal)
	{
#include "PostProcess_WaveNormal_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_WaveNormalPS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}

	// �萔�o�b�t�@
	m_cb0_Copy.Create();
	m_cb0_Blur.Create();
	m_cb0_Wave.Create();
}
