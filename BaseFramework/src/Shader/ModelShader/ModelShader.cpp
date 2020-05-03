#include "main.h"

#include "ModelShader.h"

void ModelShader::DrawModel(const KdModel & model)
{
	// �X�L�����b�V���̎�
	if(model.GetMesh()->IsSkinMesh())
	{
		// ���_�V�F�[�_���f�o�C�X�R���e�L�X�g�փZ�b�g
		KD3D.GetDevContext()->VSSetShader(m_SkinVS, 0, 0);
		// ���̓��C�A�E�g���Z�b�g
		KD3D.GetDevContext()->IASetInputLayout(m_SkinVLayout);
	}
	else
	{
		// ���_�V�F�[�_���f�o�C�X�R���e�L�X�g�փZ�b�g
		KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// ���̓��C�A�E�g���Z�b�g
		KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	}
	// �s�N�Z���V�F�[�_���Z�b�g
	KD3D.GetDevContext()->PSSetShader(m_PS, 0, 0);
	// ���b�V�������Z�b�g(���_�o�b�t�@��C���f�b�N�X�o�b�t�@�Ȃ�)
	model.GetMesh()->SetToDevice();

	// �萔�o�b�t�@��������(GPU���֓]��)
	m_cb0.Write();
	// �萔�o�b�t�@�Z�b�g
	m_cb0.GetBuffer().VSSetConstantBuffer(0);
	m_cb0.GetBuffer().PSSetConstantBuffer(0);
 
	m_cb1.GetBuffer().VSSetConstantBuffer(1);
	m_cb1.GetBuffer().PSSetConstantBuffer(1);

	// �}�e���A���P�ʂŕ`��
	for (UINT mti = 0; mti < model.GetMaterials().size(); mti++)
	{
		const KdMaterial& mate = model.GetMaterials()[mti];

		// �}�e���A���f�[�^��������
		m_cb1.GetWork().Diffuse = mate.Diffuse;		// �g�U�F
		m_cb1.GetWork().Specular = mate.Specular.ToVec3();	// ���ːF
		m_cb1.GetWork().SpePower = mate.SpePower;	// ���˂̉s��
		m_cb1.GetWork().Emissive = mate.Emissive.ToVec3();	// �����F
		m_cb1.Write();

		// �e�N�X�`���Z�b�g
		if (mate.texDiffuse)
		{
			// �s�N�Z���V�F�[�_��0�ԃX���b�g�Ƀe�N�X�`�����Z�b�g
			mate.texDiffuse->PSSetShaderResource(0);

		}
		else
		{
			// �e�N�X�`����������
			// ���̃e�N�X�`�����Z�b�g
			KD3D.GetWhiteTex()->PSSetShaderResource(0);
		}

		// Toon�e�N�X�`�����Z�b�g
		if (mate.texToon)
		{
			mate.texToon->PSSetShaderResource(1);
		}
		else 
		{
			KD3D.GetDefaultToonTex()->PSSetShaderResource(1);
		}

		// �@���e�N�X�`�����Z�b�g
		if (m_forceUseNormalMap == nullptr)
		{

			if (mate.texNormal)
			{
				mate.texNormal->PSSetShaderResource(2);
			}
			else
			{
				// 1x1��Z�����̃x�N�g���e�N�X�`��(���ۂ�)
				KD3D.GetNormalMap()->PSSetShaderResource(2);
			}
		}
		// �w��@���}�b�v���Z�b�g
		else
		{
			m_forceUseNormalMap->PSSetShaderResource(2);
		}

		// �e���E�����}�b�v
		if (mate.texRM)
		{
			mate.texRM->PSSetShaderResource(3);
		}
		else
		{
			// 1x1�̔��e�N�X�`�����Z�b�g
			KD3D.GetWhiteTex()->PSSetShaderResource(3);
		}
		// �G�~�b�V�u�}�b�v(�����F�}�b�v)
		if (mate.texEmissive)
		{
			mate.texEmissive->PSSetShaderResource(4);
		}
		else
		{
			// null���Z�b�g
			//KdTexture::s_emptyTex.PSSetShaderResource(4);
			ID3D11ShaderResourceView* srv[] = { nullptr };
			KD3D.GetDevContext()->PSSetShaderResources(4, 1, srv);
		}

		// mti�Ԗڂ̃}�e���A���ɑ����钸�_��`�悷��
		model.GetMesh()->DrawSubset(mti);
	}

	// �@���}�b�v����
	if (m_forceUseNormalMap)
	{
		KdTexture::s_emptyTex.PSSetShaderResource(2);		// null���Z�b�g
	}
}

void ModelShader::Init()
{
	Release();

	// ���_�V�F�[�_�ǂݍ���
	{
		#include "model_VS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// �V�F�[�_�f�[�^�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			nullptr,					
			&m_VS))						// �쐬���ꂽ�V�F�[�_������
			){
			assert(0 && "���_�V�F�[�_�[�쐬���s");
		}

		// ���_���̓��C�A�E�g�̍쐬
		auto verDecl = KdVertex_Pos_UV_TBN::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout( 
			&verDecl.m_Layout[0],		// ���_���C�A�E�g�f�[�^�̐擪address
			verDecl.m_Layout.size(),			// ���C�A�E�g�̍��ڐ�
			compiledBuffer,				// ���_�V�F�[�_�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			&m_VLayout))				// �����ɍ쐬���ꂽ���̂�����
			){
			assert(0 && "���_���C�A�E�g�쐬���s");
		} 
	}

	// SkinMesh���_�V�F�[�_�ǂݍ���
	{
		#include "model_SkinVS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// �V�F�[�_�f�[�^�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			nullptr,
			&m_SkinVS))						// �쐬���ꂽ�V�F�[�_������
			) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
		}

		// ���_���̓��C�A�E�g�̍쐬
		auto verDecl = KdVertex_Pos_UV_TBN_Skin::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&verDecl.m_Layout[0],		// ���_���C�A�E�g�f�[�^�̐擪address
			verDecl.m_Layout.size(),			// ���C�A�E�g�̍��ڐ�
			compiledBuffer,				// ���_�V�F�[�_�̐擪address
			sizeof(compiledBuffer),		// ���̃f�[�^�̃o�C�g�T�C�Y
			&m_SkinVLayout))				// �����ɍ쐬���ꂽ���̂�����
			) {
			assert(0 && "���_���C�A�E�g�쐬���s");
		}
	}

	// �s�N�Z���V�F�[�_�ǂݍ���
	{
		#include "model_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_PS))
			) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		}
	}

	// �萔�o�b�t�@�쐬
	m_cb0.Create();
	m_cb1.Create();

}

void ModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_VLayout);

	KdSafeRelease(m_SkinVS);
	KdSafeRelease(m_SkinVLayout);

	m_cb0.Release();
	m_cb1.Release();
}
