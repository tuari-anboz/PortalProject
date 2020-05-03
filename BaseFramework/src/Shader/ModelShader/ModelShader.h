#pragma once

//------------------------
// モデルシェーダクラス
//------------------------
class ModelShader
{
public:

	// ワールド行列をセット
	void SetWorld(const KdMatrix& m)
	{
		m_cb0.GetWork().mW = m;
	}

	// GameModelを描画
	void DrawGameModel(const KdGameModel& gmodel,const KdBoneController* bc = nullptr)
	{
		// ボーン行列をセット
		if (bc)
		{
			bc->GetBoneConstantBuffer().VSSetConstantBuffer(2);
		}
		// 全モデルデータを描画
		for (auto& model : gmodel.GetModels())
		{
			DrawModel(*model);
		}
	}

	// ライトOn/Off
	void SetLightEnable(bool enable)
	{
		m_cb0.GetWork().LightEnable = enable ? 1 : 0;
	}
	// 粗さ設定(0～1)
	void SetRoughness(float roughness)
	{
		m_cb0.GetWork().Roughness = roughness;
	}
	// 金属度設定(0～1)
	void SetMetallic(float metallic)
	{
		m_cb0.GetWork().Metallic = metallic;
	}
	// 水面ノーマルマップセット
	void SetForceUseNormalMap(const SPtr<KdTexture>& forceUseNormalMap)
	{
		m_forceUseNormalMap = forceUseNormalMap;
	}
	// 合成色セット
	void SetMulColor(const KdVec4& mulcolor)
	{
		m_cb0.GetWork().MulColor = mulcolor;
	}
	// 屈折表現On/Off
	void SetRefractEnable(bool enable)
	{
		m_cb0.GetWork().RefractEnable = enable;
	}
	// フォグのOn/Off
	void SetFogEnable(bool enable)
	{
		m_cb0.GetWork().FogEnable = enable;
	}

	// KdModelを描画
	void DrawModel(const KdModel& model);

	// 初期化
	void Init();
	// 解放
	void Release();
	~ModelShader() { Release(); }

private:
	ID3D11VertexShader*		m_VS = nullptr;	// 頂点シェーダ
	ID3D11PixelShader*		m_PS = nullptr;	// ピクセルシェーダ
	ID3D11InputLayout*		m_VLayout = nullptr;	// 頂点入力レイアウト
	// ※SkinMesh用頂点シェーダと入力レイアウト
	ID3D11VertexShader*		m_SkinVS = nullptr;	// 頂点シェーダ
	ID3D11InputLayout*		m_SkinVLayout = nullptr;	// 頂点入力レイアウト
	
	// 強制的に使用する法線マップ
	// ここにテクスチャを入れておくと、マテリアルの法線マップは
	// 使用されず、こいつを使用する
	SPtr<KdTexture>		m_forceUseNormalMap;


	// 定数バッファ(オブジェクト)
	struct cbObject
	{
		KdMatrix mW; // ワールド行列
		KdVec4 MulColor = { 1,1,1,1 };		// 合成色
		int LightEnable = 1;	// ライトOn/Off
		float Roughness = 1.0f;	// 粗さ
		float Metallic = 0;		// 金属度
		int RefractEnable = 0;	// 屈折表現On/Off
		int FogEnable = true;	// フォグOn/Off
		float tmp[3];			// パッキング規則用のゴミ
	};
	KdConstantBuffer<cbObject> m_cb0;

	// 定数バッファ(マテリアル)
	struct cbMaterial
	{
		KdVec4 Diffuse;		// 拡散色
		KdVec3 Specular;	// 反射色
		float  SpePower;	// 反射の鋭さ
		KdVec3 Emissive;	// 発光色
		float  tmp;
	};
	KdConstantBuffer<cbMaterial> m_cb1;

	// 定数バッファ(ライト)
};