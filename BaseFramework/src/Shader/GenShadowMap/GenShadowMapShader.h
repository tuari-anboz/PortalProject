#pragma once

class GenShadowMapShader
{
public:
	// 取得
	const SPtr<KdTexture>& GenDepthMap() { return m_depthMap; }

	// ワールド行列をセット
	void SetWorld(const KdMatrix& m)
	{
		m_cb0.GetWork().mW = m;
	}

	// GameMpdelを描画
	void DrawGameModelDepth(const KdGameModel& gmodel, const KdBoneController* bc = nullptr)
	{
		if (bc)
		{
			bc->GetBoneConstantBuffer().VSSetConstantBuffer(1);
		}
		// 全モデルデータを描画
		for (auto& model : gmodel.GetModels())
		{
			DrawModelDepth(*model);
		}
	}

	// 深度描画
	void DrawModelDepth(const KdModel& model);

	// 深度マップ生成
	void Generate(std::function<void()> drawProc);

	// 初期化
	void Init();

	// 解放
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
	// データ
	SPtr<KdTexture>		m_depthMap;		// 深度マップ
	SPtr<KdTexture>		m_zBuffer;		// Zバッファ

	// 各シェーダ
	ID3D11VertexShader*		m_VS = nullptr;		// 頂点シェーダ
	ID3D11InputLayout*		m_VLayout = nullptr;		// 頂点レイアウト
	ID3D11PixelShader*		m_PS = nullptr;		// ピクセルシェーダ
	// ※SkinMesh用頂点シェーダと入力レイアウト
	ID3D11VertexShader*		m_SkinVS = nullptr;	// 頂点シェーダ
	ID3D11InputLayout*		m_SkinVLayout = nullptr;	// 頂点入力レイアウト

	// 定数バッファ(オブジェクト単位)
	struct cbObject
	{
		KdMatrix mW;		// ワールド行列
	};
	KdConstantBuffer<cbObject> m_cb0;
};