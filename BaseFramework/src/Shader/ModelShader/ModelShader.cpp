#include "main.h"

#include "ModelShader.h"

void ModelShader::DrawModel(const KdModel & model)
{
	// スキンメッシュの時
	if(model.GetMesh()->IsSkinMesh())
	{
		// 頂点シェーダをデバイスコンテキストへセット
		KD3D.GetDevContext()->VSSetShader(m_SkinVS, 0, 0);
		// 入力レイアウトをセット
		KD3D.GetDevContext()->IASetInputLayout(m_SkinVLayout);
	}
	else
	{
		// 頂点シェーダをデバイスコンテキストへセット
		KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// 入力レイアウトをセット
		KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	}
	// ピクセルシェーダをセット
	KD3D.GetDevContext()->PSSetShader(m_PS, 0, 0);
	// メッシュ情報をセット(頂点バッファやインデックスバッファなど)
	model.GetMesh()->SetToDevice();

	// 定数バッファ書き込み(GPU側へ転送)
	m_cb0.Write();
	// 定数バッファセット
	m_cb0.GetBuffer().VSSetConstantBuffer(0);
	m_cb0.GetBuffer().PSSetConstantBuffer(0);
 
	m_cb1.GetBuffer().VSSetConstantBuffer(1);
	m_cb1.GetBuffer().PSSetConstantBuffer(1);

	// マテリアル単位で描画
	for (UINT mti = 0; mti < model.GetMaterials().size(); mti++)
	{
		const KdMaterial& mate = model.GetMaterials()[mti];

		// マテリアルデータ書き込み
		m_cb1.GetWork().Diffuse = mate.Diffuse;		// 拡散色
		m_cb1.GetWork().Specular = mate.Specular.ToVec3();	// 反射色
		m_cb1.GetWork().SpePower = mate.SpePower;	// 反射の鋭さ
		m_cb1.GetWork().Emissive = mate.Emissive.ToVec3();	// 発光色
		m_cb1.Write();

		// テクスチャセット
		if (mate.texDiffuse)
		{
			// ピクセルシェーダの0番スロットにテクスチャをセット
			mate.texDiffuse->PSSetShaderResource(0);

		}
		else
		{
			// テクスチャが無い時
			// 白のテクスチャをセット
			KD3D.GetWhiteTex()->PSSetShaderResource(0);
		}

		// Toonテクスチャをセット
		if (mate.texToon)
		{
			mate.texToon->PSSetShaderResource(1);
		}
		else 
		{
			KD3D.GetDefaultToonTex()->PSSetShaderResource(1);
		}

		// 法線テクスチャをセット
		if (m_forceUseNormalMap == nullptr)
		{

			if (mate.texNormal)
			{
				mate.texNormal->PSSetShaderResource(2);
			}
			else
			{
				// 1x1のZ方向のベクトルテクスチャ(青っぽい)
				KD3D.GetNormalMap()->PSSetShaderResource(2);
			}
		}
		// 指定法線マップをセット
		else
		{
			m_forceUseNormalMap->PSSetShaderResource(2);
		}

		// 粗さ・金属マップ
		if (mate.texRM)
		{
			mate.texRM->PSSetShaderResource(3);
		}
		else
		{
			// 1x1の白テクスチャをセット
			KD3D.GetWhiteTex()->PSSetShaderResource(3);
		}
		// エミッシブマップ(発光色マップ)
		if (mate.texEmissive)
		{
			mate.texEmissive->PSSetShaderResource(4);
		}
		else
		{
			// nullをセット
			//KdTexture::s_emptyTex.PSSetShaderResource(4);
			ID3D11ShaderResourceView* srv[] = { nullptr };
			KD3D.GetDevContext()->PSSetShaderResources(4, 1, srv);
		}

		// mti番目のマテリアルに属する頂点を描画する
		model.GetMesh()->DrawSubset(mti);
	}

	// 法線マップ解除
	if (m_forceUseNormalMap)
	{
		KdTexture::s_emptyTex.PSSetShaderResource(2);		// nullをセット
	}
}

void ModelShader::Init()
{
	Release();

	// 頂点シェーダ読み込み
	{
		#include "model_VS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// シェーダデータの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			nullptr,					
			&m_VS))						// 作成されたシェーダが入る
			){
			assert(0 && "頂点シェーダー作成失敗");
		}

		// 頂点入力レイアウトの作成
		auto verDecl = KdVertex_Pos_UV_TBN::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout( 
			&verDecl.m_Layout[0],		// 頂点レイアウトデータの先頭address
			verDecl.m_Layout.size(),			// レイアウトの項目数
			compiledBuffer,				// 頂点シェーダの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			&m_VLayout))				// ここに作成されたものが入る
			){
			assert(0 && "頂点レイアウト作成失敗");
		} 
	}

	// SkinMesh頂点シェーダ読み込み
	{
		#include "model_SkinVS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// シェーダデータの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			nullptr,
			&m_SkinVS))						// 作成されたシェーダが入る
			) {
			assert(0 && "頂点シェーダー作成失敗");
		}

		// 頂点入力レイアウトの作成
		auto verDecl = KdVertex_Pos_UV_TBN_Skin::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&verDecl.m_Layout[0],		// 頂点レイアウトデータの先頭address
			verDecl.m_Layout.size(),			// レイアウトの項目数
			compiledBuffer,				// 頂点シェーダの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			&m_SkinVLayout))				// ここに作成されたものが入る
			) {
			assert(0 && "頂点レイアウト作成失敗");
		}
	}

	// ピクセルシェーダ読み込み
	{
		#include "model_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_PS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}

	// 定数バッファ作成
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
