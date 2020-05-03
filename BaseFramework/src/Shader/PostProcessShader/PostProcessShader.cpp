#include "main.h"

#include "PostProcessShader.h"

void PostProcessShader::CopyDraw(const KdTexture & tex,const KdVec4& color)
{
	// 定数バッファ
	m_cb0_Copy.GetWork().Color = color;
	m_cb0_Copy.Write();
	m_cb0_Copy.GetBuffer().PSSetConstantBuffer(0);

	// 各シェーダをセット
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_CopyPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// テクスチャセット
	tex.PSSetShaderResource(0);
	// 頂点データ
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// プリミティブ・トポロジー
	// (描画時の頂点データの使用方法)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// 頂点描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭アドレス
		sizeof(KdVertex_Pos_UV)		// 1頂点のバイトサイズ
	);

	// テクスチャを解除(ポストプロセス描画の場合は必ず解除しておく)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::ToneMappingDraw(const KdTexture & tex)
{
	// 各シェーダをセット
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_ToneMappingPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// テクスチャセット
	tex.PSSetShaderResource(0);
	// 頂点データ
	KdVertex_Pos_UV v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};
	// プリミティブ・トポロジー
	// (描画時の頂点データの使用方法)
//	KD3D.GetDevContext()->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//	);

	// 頂点描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭アドレス
		sizeof(KdVertex_Pos_UV)	// １頂点のバイトサイズ
	);

	// テクスチャを解除
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::BrightFilteringDraw(const KdTexture & tex)
{
	// 各シェーダをセット
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_BrightFilteringPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// テクスチャセット
	tex.PSSetShaderResource(0);
	// 頂点データ
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// プリミティブ・トポロジー
	// (描画時の頂点データの使用方法)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// 頂点描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭アドレス
		sizeof(KdVertex_Pos_UV)		// 1頂点のバイトサイズ
	);

	// テクスチャを解除(ポストプロセス描画の場合は必ず解除しておく)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::BlurDraw(const KdTexture & tex, const KdVec2& dir)
{
	// 定数バッファ
	m_cb0_Blur.GetBuffer().PSSetConstantBuffer(0);

	// テクセルサイズ算出(1ピクセルのUV座標系でのサイズ)
	float totalW = 0;
	constexpr float dispersion = 0.2f;
	KdVec2 ts;
	ts.x = 1.0f / tex.GetInfo().Width;
	ts.y = 1.0f / tex.GetInfo().Height;

	for (int i = 0; i < 31; i++)
	{
		int pt = i - 15;		// -15 ～ +15
		// オフセット座標
		m_cb0_Blur.GetWork().Offset[i].x = dir.x * (pt * ts.x);
		m_cb0_Blur.GetWork().Offset[i].y = dir.y * (pt * ts.y);
		// 重み
		//m_cb0_Blur.GetWork().Offset[i].z = 1.0f / 31;
		m_cb0_Blur.GetWork().Offset[i].z = exp(-(pt*pt)*dispersion) / sqrt(3.141592f * 2);
		totalW += m_cb0_Blur.GetWork().Offset[i].z;
	}

	// 重みを正規化
	for (int i = 0; i < 31; i++)
	{
		m_cb0_Blur.GetWork().Offset[i].z /= totalW; // 正規化 
	}

	m_cb0_Blur.Write();		// GPU側へ転送

	// 各シェーダをセット
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_BlurPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// テクスチャセット
	tex.PSSetShaderResource(0);
	// 頂点データ
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// プリミティブ・トポロジー
	// (描画時の頂点データの使用方法)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// 頂点描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭アドレス
		sizeof(KdVertex_Pos_UV)		// 1頂点のバイトサイズ
	);

	// テクスチャを解除(ポストプロセス描画の場合は必ず解除しておく)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::GenerateBlur(BlurTexture & dest, const KdTexture & tex)
{
	// 現在のRT、Zバッファを記憶して、後で復元させる
	KdRTSaver rtSave;

	// RT,Zバッファ変更用
	KdRTChanger rtc;

	//// ①ダウンサイジング
	//rtc.RT(0, dest.m_rt[0]);		// 変更したいRT
	//rtc.Depth(nullptr);				// 変更したいZバッファ
	//rtc.SetToDevice();				// 実際に変更を行う

	//CopyDraw(tex);

	//// ②Xブラー
	//rtc.RT(0, dest.m_rt[1]);
	////rtc.Depth(nullptr);
	//rtc.SetToDevice();

	//BlurDraw(dest.m_rt[0], { 1,0 });

	//// ③Yブラー
	//rtc.RT(0, dest.m_rt[0]);
	////rtc.Depth(nullptr);
	//rtc.SetToDevice();

	//BlurDraw(dest.m_rt[1], { 0,1 });

	for (int i = 0; i < 5; i++)
	{
		// ①ダウンサイジング
		rtc.RT(0, dest.m_rt[i][0]);		// 変更したいRT
		rtc.Depth(nullptr);				// 変更したいZバッファ
		rtc.SetToDevice();				// 実際に変更を行う

		if (i == 0)
		{
			CopyDraw(tex);
		}
		else
		{
			CopyDraw(dest.m_rt[i - 1][0]);
		}

		// ②Xブラー [0]→[1]へ
		rtc.RT(0, dest.m_rt[i][1]);
		//rtc.Depth(nullptr);
		rtc.SetToDevice();

		BlurDraw(dest.m_rt[i][0], { 1,0 });

		// ③Yブラー [1]→[0]へ
		rtc.RT(0, dest.m_rt[i][0]);
		//rtc.Depth(nullptr);
		rtc.SetToDevice();

		BlurDraw(dest.m_rt[i][1], { 0,1 });
	}

}

void PostProcessShader::AdvanceWave(WaveTexture & wave)
{
	// RT.Zを記憶用
	KdRTSaver rtSave;
	// RT,Zを変更用
	KdRTChanger rtc;
	rtc.Depth(nullptr);

	// 水面にランダムで力を加える
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
	// ①高さ・速さマップ更新
	//---------------------------------
	rtc.RT(0, *wave.m_rtHeight[0]);
	rtc.SetToDevice();
	// 入力テクスチャ
	wave.m_rtHeight[1]->PSSetShaderResource(0);

	// 各シェーダをセット
	KD3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	KD3D.GetDevContext()->PSSetShader(m_WaveHeightPS, 0, 0);
	KD3D.GetDevContext()->IASetInputLayout(m_VLayout);
	// 頂点データ
	KdVertex_Pos_UV v[4] =
	{
		{{-1,-1,0},{0,1}},	// 0
		{{-1,1,0},{0,0}},	// 1
		{{1,-1,0},{1,1}},	// 2
		{{1,1,0},{1,0}}		// 3
	};
	// プリミティブ・トポロジー
	// (描画時の頂点データの使用方法)
	//KD3D.GetDevContext()->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	// 頂点描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭アドレス
		sizeof(KdVertex_Pos_UV)		// 1頂点のバイトサイズ
	);

	// [0]と[1]を入れ替える
	std::swap(wave.m_rtHeight[0], wave.m_rtHeight[1]);

	//---------------------------------
	// ②法線マップ更新
	//---------------------------------
	rtc.RT(0, *wave.m_rtNormal);
	rtc.SetToDevice();
	// 最新の高さマップをセット
	wave.m_rtHeight[1]->PSSetShaderResource(0);
	// PSセット
	KD3D.GetDevContext()->PSSetShader(m_WaveNormalPS, 0, 0);
	// 描画
	KD3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4,		// 使用する頂点数
		&v[0],	// 頂点配列の先頭address
		sizeof(KdVertex_Pos_UV)	// 1頂点のバイトサイズ
	);

	// テクスチャを解除(ポストプロセス描画の場合は必ず解除しておく)
	KdTexture::s_emptyTex.PSSetShaderResource(0);
}

void PostProcessShader::Init()
{
	Release();

	// 頂点シェーダ
	{
#include "PostProcess_VS.inc"

		if (FAILED(KD3D.GetDev()->CreateVertexShader(
			compiledBuffer,				// シェーダデータの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			nullptr,
			&m_VS))						// 作成されたシェーダが入る
			) {
			assert(0 && "頂点シェーダー作成失敗");
		}

		// 頂点入力レイアウトの作成
		auto verDecl = KdVertex_Pos_UV::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&verDecl.m_Layout[0],		// 頂点レイアウトデータの先頭address
			verDecl.m_Layout.size(),			// レイアウトの項目数
			compiledBuffer,				// 頂点シェーダの先頭address
			sizeof(compiledBuffer),		// そのデータのバイトサイズ
			&m_VLayout))				// ここに作成されたものが入る
			) {
			assert(0 && "頂点レイアウト作成失敗");
		}
	}

	// ピクセルシェーダ(2D Copy)
	{
		#include "PostProcess_Copy_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_CopyPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}
	// ブラーシェーダ(2D Blur)
	{
		#include "PostProcess_Blur_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_BlurPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}
	// 高輝度抽出(2D BrightFiltering)
	{
		#include "PostProcess_BrightFiltering_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_BrightFilteringPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}
	// ピクセルシェーダ(ToneMapping)
	{
#include "PostProcess_ToneMapping_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_ToneMappingPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}
	// ピクセルシェーダ(WaveHeight)
	{
		#include "PostProcess_WaveHeight_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_WaveHeightPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}
	// ピクセルシェーダ(WaveNormal)
	{
#include "PostProcess_WaveNormal_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(
			compiledBuffer,
			sizeof(compiledBuffer),
			nullptr,
			&m_WaveNormalPS))
			) {
			assert(0 && "ピクセルシェーダー作成失敗");
		}
	}

	// 定数バッファ
	m_cb0_Copy.Create();
	m_cb0_Blur.Create();
	m_cb0_Wave.Create();
}
