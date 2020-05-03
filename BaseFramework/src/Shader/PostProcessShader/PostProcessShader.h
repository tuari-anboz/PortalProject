// PostProcessShader.h
#pragma once

// ブラーテクスチャ
struct BlurTexture
{
	// テクスチャ作成
	void Create(int w, int h)
	{
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			// レンダーターゲットテクスチャ(RT)として作成
			m_rt[i][0].CreateRT(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			m_rt[i][1].CreateRT(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
		//m_rt[0].CreateRT(w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM);
		//m_rt[1].CreateRT(w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	// 作業用レンダーターゲットテクスチャ
	KdTexture m_rt[5][2];
	//KdTexture m_rt[2];
};

// 波テクスチャ
struct WaveTexture
{
	// 高さマップ
	SPtr<KdTexture> m_rtHeight[2];
	// 法線マップ
	SPtr<KdTexture> m_rtNormal;

	void Create(int w, int h)
	{
		m_rtHeight[0] = std::make_shared<KdTexture>();
		m_rtHeight[1] = std::make_shared<KdTexture>();
		m_rtNormal = std::make_shared<KdTexture>();

		m_rtHeight[0]->CreateRT(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtHeight[1]->CreateRT(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtNormal->CreateRT(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		// 高さマップクリア
		m_rtHeight[0]->ClearRT({ 0,0,0,0 });
		m_rtHeight[1]->ClearRT({ 0,0,0,0 });
	}
};

class PostProcessShader
{
public:
	// テクスチャを全画面に2Dコピー描画
	// ・tex	・・・ソース画像
	void CopyDraw(const KdTexture& tex, const KdVec4& color = { 1,1,1,1 });

	void ToneMappingDraw(const KdTexture& tex);

	// テクスチャの明るさ1以上を抽出する描画
	// ・tex	・・・ソース画像
	void BrightFilteringDraw(const KdTexture& tex);

	// テクスチャをぼかし描画
	// ・dir	…ぼかす方向ベクトル
	void BlurDraw(const KdTexture& tex,const KdVec2& dir);

	// ぼかし画像生成
	// ・dest	…ぼかした画像を入れるテクスチャ
	// ・tex	…ぼかし元画像
	void GenerateBlur(BlurTexture& dest, const KdTexture& tex);

	// 波進行
	// waveの波テクスチャの時間を進める
	void AdvanceWave(WaveTexture& wave);

	// 初期化関数
	void Init();
	// 解放
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
	// 頂点シェーダ
	ID3D11VertexShader* m_VS = nullptr;
	ID3D11InputLayout* m_VLayout = nullptr;

	// ピクセルシェーダ(2Dコピー)
	ID3D11PixelShader* m_CopyPS = nullptr;

	// ピクセルシェーダ(ブラー)
	ID3D11PixelShader* m_BlurPS = nullptr;
	// ピクセルシェーダ(高輝度抽出)
	ID3D11PixelShader* m_BrightFilteringPS = nullptr;
	// ピクセルシェーダ(ToneMapping)
	ID3D11PixelShader* m_ToneMappingPS = nullptr;
	// 波用 高さマップ更新シェーダ
	ID3D11PixelShader* m_WaveHeightPS = nullptr;
	// 波用 法線マップ更新シェーダ
	ID3D11PixelShader* m_WaveNormalPS = nullptr;

	// 定数バッファ(Copy)
	struct cbCopy
	{
		KdVec4 Color = { 1,1,1,1 };
	};
	KdConstantBuffer<cbCopy>	m_cb0_Copy;

	// 定数バッファ(Blur)
	struct cbBlur
	{
		KdVec4 Offset[31];
	};
	KdConstantBuffer<cbBlur>		m_cb0_Blur;

	// 定数バッファ(Wave)
	struct cbWave
	{
		KdVec2	AddPowerUV;		// 力を加えるUV座標
		float	AddPowerRadius;	// その半径
		float	AddPower;		// 威力
	};
	KdConstantBuffer<cbWave>		m_cb0_Wave;
};