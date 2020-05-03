#pragma once

// インクルード
#include "ModelShader/ModelShader.h"
#include "PostProcessShader/PostProcessShader.h"
#include "GenShadowMap/GenShadowMapShader.h"

//=================================================
//
// シェーダー関係のものを管理するクラス
//
// ・自分の作成したシェーダクラス
// ・よく使用するステート
// ・ライトの情報
// ・カメラの情報
//
//=================================================
class ShaderManager {
public:

	//--------------------
	// シェーダクラス
	//--------------------
	KdPrimitiveShader	m_KdPrimSh;
	KdModelShader		m_KdModelSh;
	KdSpriteShader		m_KdSpriteSh;

	ModelShader			m_ModelSh;
	PostProcessShader	m_postProcessSh;
	GenShadowMapShader	m_genShadowMapSh;




	//--------------------
	// カメラ
	//--------------------
	KdMatrix		m_mView;		// ビュー行列
	KdMatrix		m_mProj;		// 射影行列

	// 定数バッファ(カメラ)
	struct cbCamera
	{
		KdMatrix	mV;		// ビュー行列
		KdMatrix	mP;		// 射影行列
		KdVec3		CamPos;	// カメラ座標

		float		DistanceFogDensity = 0.05f;		// 距離フォグの密度
		KdVec3		DistanceFogColor = { 0,0,0 };		// 距離フォグの色
		float		tmp;
	};
	KdConstantBuffer<cbCamera> m_cb7_Camera ;

	// カメラ情報を各シェーダへセット
	void UpdateCamera() {
		m_KdPrimSh.SetView(m_mView);
		m_KdPrimSh.SetProj(m_mProj);

		m_KdModelSh.SetView(m_mView);
		m_KdModelSh.SetProj(m_mProj);

		// カメラ情報書き込み
		m_cb7_Camera.GetWork().mV = m_mView;
		KdMatrix mCam = m_mView;
		mCam.Inverse();
		m_cb7_Camera.GetWork().CamPos = mCam.GetPos();
		m_cb7_Camera.GetWork().mP = m_mProj;
		m_cb7_Camera.Write();		// GPU側へデータを転送する

	}

	// 3D→2D変換(カメラはmViewとmProjを使用。現在のビューポートを使用。)
	// ・pos2D		… 結果である2D座標が入ってくる
	// ・pos3D		… 処理の基となる3D座標
	void Convert3Dto2D(KdVec3& pos2D, const KdVec3& pos3D);

	// 2D→3D変換(カメラはmViewとmProjを使用。現在のビューポートを使用。)
	// ・pos3D		… 結果である3D座標が入ってくる
	// ・pos2D		… 処理の基となる2D座標
	void Convert2Dto3D(KdVec3& pos3D, const KdVec3& pos2D);

	// カスケード数(分割数)設定
	// 合成色セット
	void SetCascade(const int& cascadenum)
	{
		m_cb8_Light.GetWork().CascadeNum = cascadenum;
	}

	//--------------------
	// ライト
	//--------------------
	// 定数バッファ(ライト)
	struct cbLight
	{
		// 環境光
		KdVec3 AmbientLight = { 0.2f,0.2f,0.2f };
		// 平行光
		int	DL_Cnt = 1;		// 平行光の数
		struct DLData
		{
			KdVec3 Dir = { 0,-1,0 };	// 方向
			float tmp;
			KdVec3 Color = { 1,1,1 };	// 色
			float tmp2;
		};
		DLData  DL[3];	// 平行光の配列	

		// 点光
		int PL_Cnt = 0;	// 点光の数
		float tmp;
		float tmp2;
		float tmp3;

		struct PLData
		{
			KdVec3 Pos;
			float tmp;
			KdVec3 Color = { 1,1,1 };	// 色
			float Radius;
		};
//		PLData PL[100];		//	点光の配列
		std::array<PLData, 100>	PL;		// 点光の配列

		// ポイント光

		// IBL
		float IBLIntensity = 0.6f;
		float tmp4[3];

		//-------------------------
		// シャドウマッピング用(平行光の0番目専用)
		//-------------------------
		KdMatrix	DirLightVP[4];		// ライトカメラのビュー*射影行列配列
		int			CascadeNowIdx = 0;
		int			CascadeNum = 4;		// 分割数
		float		tmp5[2];
		float		CascadeDist[4] = { 5,20,100,300 };		// 0～3の距離
	};

	KdConstantBuffer<cbLight> m_cb8_Light;







	//--------------------
	// Sampler State
	//--------------------
	ID3D11SamplerState*		m_ss0_AnisoWrap = nullptr;	// 異方性フィルタ Wrapモード
	ID3D11SamplerState*		m_ss1_AnisoClamp = nullptr;	// 異方性フィルタ Clampモード

	ID3D11SamplerState*		m_ss2_LinearWrap = nullptr;	// 線形フィルタ Wrapモード
	ID3D11SamplerState*		m_ss3_LinearClamp = nullptr;	// 線形フィルタ Clampモード

	ID3D11SamplerState*		m_ss4_PointWrap = nullptr;	// フィルタなし Wrapモード
	ID3D11SamplerState*		m_ss5_PointClamp = nullptr;	// フィルタなし Clampモード

	ID3D11SamplerState*		m_ss10_Comparison = nullptr;	// ShadowMapping用 比較サンプラ

	//--------------------
	// Blend State
	//--------------------
	ID3D11BlendState*		m_bsDisable = nullptr;		// ブレンド無効
	ID3D11BlendState*		m_bsNoBlend = nullptr;		// ブレンドせずにただのコピーを行う
	ID3D11BlendState*		m_bsAlpha_AtoC = nullptr;	// 半透明合成(Alpha To Coverage付き)
	ID3D11BlendState*		m_bsAlpha = nullptr;		// 半透明合成
	ID3D11BlendState*		m_bsAdd = nullptr;			// 加算合成

	//--------------------
	// DepthStencil State
	//--------------------
	ID3D11DepthStencilState*	m_ds_ZCompareON_ZWriteON = nullptr;		// Z判定On Z書き込みOn
	ID3D11DepthStencilState*	m_ds_ZCompareON_ZWriteOFF = nullptr;	// Z判定On Z書き込みOff 主にエフェクト描画用
	ID3D11DepthStencilState*	m_ds_ZCompareOFF_ZWriteON = nullptr;	// Z判定Off Z書き込みOn
	ID3D11DepthStencilState*	m_ds_ZCompareOFF_ZWriteOFF = nullptr;	// Z判定Off Z書き込みOff 主に2D画像描画用

	//--------------------
	// Rasterizer State
	//--------------------
	ID3D11RasterizerState*		m_rs_Default = nullptr;		// 表面描画
	ID3D11RasterizerState*		m_rs_Reverse = nullptr;		// 裏面描画
	ID3D11RasterizerState*		m_rs_Both = nullptr;		// 両面描画
	ID3D11RasterizerState*		m_rs_Wireframe = nullptr;		// ワイヤーフレーム描画
	ID3D11RasterizerState*		m_rs_WireframeBoth = nullptr;	// ワイヤーフレーム描画 &  両面描画

	ID3D11RasterizerState*		m_rs_Default_ZClipOff = nullptr;		// 両面描画 ZクリップOff

	//--------------------
	// デバッグ用定数バッファ
	//--------------------
	struct cbDebug {
		int DisableMaterial = 0;	// マテリアル色を無視
		int ShowCascadeRange = 0;	// カスケードシャドウマップの範囲を表示

		float tmp[2];
	};
	KdConstantBuffer<cbDebug>	m_cb13_Debug;

	//--------------------
	// 初期設定
	//--------------------
	void Init();

	//--------------------
	// 解放
	//--------------------
	void Release();


//=======================================
// シングルトン
//=======================================
private:
	ShaderManager() {}

public:
	static ShaderManager& GetInstance() {
		static ShaderManager instance;
		return instance;
	}
};

#define ShMgr ShaderManager::GetInstance()
