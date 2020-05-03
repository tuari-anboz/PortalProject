#include "main.h"
#include "ShaderManager.h"

//--------------------
// 初期設定
//--------------------
void ShaderManager::Init()
{
	m_KdPrimSh.Init();
	m_KdModelSh.Init();
	m_KdSpriteSh.Init();

	m_ModelSh.Init();
	m_postProcessSh.Init();
	m_genShadowMapSh.Init();

	//-------------------
	// Sampler State作成
	//-------------------
	m_ss0_AnisoWrap = KdCreateSamplerState(2, 4, 0, false);
	m_ss1_AnisoClamp = KdCreateSamplerState(2, 4, 1, false);

	m_ss2_LinearWrap = KdCreateSamplerState(1, 0, 0, false);
	m_ss3_LinearClamp = KdCreateSamplerState(1, 0, 1, false);

	m_ss4_PointWrap = KdCreateSamplerState(0, 0, 0, false);
	m_ss5_PointClamp = KdCreateSamplerState(0, 0, 1, false);

	m_ss10_Comparison = KdCreateSamplerState(1, 0, 1, true);

	KD3D.GetDevContext()->PSSetSamplers(0, 1, &m_ss0_AnisoWrap);
	KD3D.GetDevContext()->PSSetSamplers(1, 1, &m_ss1_AnisoClamp);
	KD3D.GetDevContext()->PSSetSamplers(2, 1, &m_ss2_LinearWrap);
	KD3D.GetDevContext()->PSSetSamplers(3, 1, &m_ss3_LinearClamp);
	KD3D.GetDevContext()->PSSetSamplers(4, 1, &m_ss4_PointWrap);
	KD3D.GetDevContext()->PSSetSamplers(5, 1, &m_ss5_PointClamp);
	KD3D.GetDevContext()->PSSetSamplers(10, 1, &m_ss10_Comparison);

	//-------------------
	// Blend State作成
	//-------------------
	m_bsDisable = KdCreateBlendState_Disable();
	m_bsNoBlend = KdCreateBlendState_NoBlend();
	m_bsAlpha_AtoC = KdCreateBlendState_Alpha(true);
	m_bsAlpha = KdCreateBlendState_Alpha(false);
	m_bsAdd = KdCreateBlendState_Add(false);

	//-------------------
	// DepthStencil State作成
	//-------------------
	m_ds_ZCompareON_ZWriteON = KdCreateDepthStencilState(true, true);
	m_ds_ZCompareON_ZWriteOFF = KdCreateDepthStencilState(true, false);
	m_ds_ZCompareOFF_ZWriteON = KdCreateDepthStencilState(false, true);
	m_ds_ZCompareOFF_ZWriteOFF = KdCreateDepthStencilState(false, false);


	//-------------------
	// Rasterizer State作成
	//-------------------
	m_rs_Default = KdCreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true);
	m_rs_Reverse = KdCreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, true);
	m_rs_Both = KdCreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true);
	m_rs_Wireframe = KdCreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, true);
	m_rs_WireframeBoth = KdCreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, true);

	m_rs_Default_ZClipOff = KdCreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, false);

	// カメラ定数バッファ
	m_cb7_Camera.Create();
	m_cb7_Camera.GetBuffer().VSSetConstantBuffer(7);
	m_cb7_Camera.GetBuffer().PSSetConstantBuffer(7);

	// ライト定数バッファ
	m_cb8_Light.Create();
	m_cb8_Light.GetBuffer().VSSetConstantBuffer(8);
	m_cb8_Light.GetBuffer().PSSetConstantBuffer(8);
	m_cb8_Light.Write();

	// デバッグ用定数バッファ
	m_cb13_Debug.Create();
	m_cb13_Debug.GetBuffer().VSSetConstantBuffer(13);
	m_cb13_Debug.GetBuffer().PSSetConstantBuffer(13);

}

//--------------------
// 解放
//--------------------
void ShaderManager::Release()
{
	// シェーダクラス解放
	m_KdPrimSh.Release();
	m_KdModelSh.Release();
	m_KdSpriteSh.Release();

	m_ModelSh.Release();
	m_postProcessSh.Release();
	m_genShadowMapSh.Release();

	// 定数バッファ解放
	m_cb7_Camera.Release();


	// ステート解放
	KdSafeRelease(m_ss0_AnisoWrap);
	KdSafeRelease(m_ss1_AnisoClamp);
	KdSafeRelease(m_ss2_LinearWrap);
	KdSafeRelease(m_ss3_LinearClamp);
	KdSafeRelease(m_ss4_PointWrap);
	KdSafeRelease(m_ss5_PointClamp);
	KdSafeRelease(m_ss10_Comparison);

	KdSafeRelease(m_bsDisable);
	KdSafeRelease(m_bsNoBlend);
	KdSafeRelease(m_bsAlpha_AtoC);
	KdSafeRelease(m_bsAlpha);
	KdSafeRelease(m_bsAdd);

	KdSafeRelease(m_ds_ZCompareON_ZWriteON);
	KdSafeRelease(m_ds_ZCompareON_ZWriteOFF);
	KdSafeRelease(m_ds_ZCompareOFF_ZWriteON);
	KdSafeRelease(m_ds_ZCompareOFF_ZWriteOFF);

	KdSafeRelease(m_rs_Default);
	KdSafeRelease(m_rs_Reverse);
	KdSafeRelease(m_rs_Both);
	KdSafeRelease(m_rs_Wireframe);
	KdSafeRelease(m_rs_WireframeBoth);
	KdSafeRelease(m_rs_Default_ZClipOff);

}

// 
void ShaderManager::Convert3Dto2D(KdVec3& pos2D, const KdVec3& pos3D)
{
	// 現在のビューポート取得
	UINT Num = 1;
	D3D11_VIEWPORT vp;
	KD3D.GetDevContext()->RSGetViewports(&Num, &vp);

	// 変換
	KdMatrix mW;
	mW.CreateMove(pos3D);
	KdMatrix m = mW * m_mView * m_mProj;

	float halfW = vp.Width * 0.5f;
	float halfH = vp.Height * 0.5f;

	pos2D.x = (m._41 / m._44) * halfW + halfW;
	pos2D.y = (m._42 / m._44) * -halfH + halfH;
	pos2D.z = m._44;

}

void ShaderManager::Convert2Dto3D(KdVec3& pos3D, const KdVec3& pos2D)
{
	// 現在のビューポート取得
	UINT Num = 1;
	D3D11_VIEWPORT vp;
	KD3D.GetDevContext()->RSGetViewports(&Num, &vp);

	// 変換
	pos3D = DirectX::XMVector3Unproject(pos2D, vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height, vp.MinDepth, vp.MaxDepth, m_mProj, m_mView, KdMatrix::Identity);

}

