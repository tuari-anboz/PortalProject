#include "main.h"

#include "GenShadowMapShader.h"

//====================================================================
// カメラの情報から、ぴったり収まるビュー行列と正射影行列を求める
// mView	: ビュー行列
// mProj	: 射影行列
// lightDir	: ライトの方向
// outView	: [出力]算出したビュー行列
// outProj	: [出力]算出した射影行列
//====================================================================
static void CreateLightViewProj(const KdMatrix& mView, const KdMatrix& mProj, const KdVec3& lightDir, KdMatrix& outView, KdMatrix& outProj)
{

	//-------------------------------------------
	// 現在の視錐台の各点(8座標)を求める
	// 射影座標系からワールド座標系へ逆変換
	//-------------------------------------------
	KdMatrix camInvProj = mProj;
	camInvProj.Inverse();
	KdMatrix mCam = mView;
	mCam.Inverse();
	KdMatrix camInvPV = camInvProj * mCam;

	// -1～1(Zは0～1)の射影座標系のBOXデータ
	std::vector<KdVec3> PtTbl;
	PtTbl.push_back(KdVec3(1, 1, 0));
	PtTbl.push_back(KdVec3(-1, 1, 0));
	PtTbl.push_back(KdVec3(-1, -1, 0));
	PtTbl.push_back(KdVec3(1, -1, 0));
	PtTbl.push_back(KdVec3(1, 1, 1));
	PtTbl.push_back(KdVec3(-1, 1, 1));
	PtTbl.push_back(KdVec3(-1, -1, 1));
	PtTbl.push_back(KdVec3(1, -1, 1));
	// 射影座標系の視錐台８座標をワールド座標系へ変換
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].Transform(camInvPV);
	}
	// 仮のビュー行列作成(座標はまだ未定なので方向のみ)
	KdMatrix lightView;
	lightView.LookTo(lightDir, { 0, 1, 0 });
	lightView.Inverse();

	// 視錐台の８点の座標を仮のビュー座標系へ変換
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].Transform(lightView);
	}
	// 視錐台のAABBを算出
	KdVec3 vMin, vMax;
	vMin = PtTbl[0];
	vMax = PtTbl[0];
	for (UINT i = 1; i < PtTbl.size(); i++) {
		if (vMin.x > PtTbl[i].x)vMin.x = PtTbl[i].x;
		if (vMin.y > PtTbl[i].y)vMin.y = PtTbl[i].y;
		if (vMin.z > PtTbl[i].z)vMin.z = PtTbl[i].z;
		if (vMax.x < PtTbl[i].x)vMax.x = PtTbl[i].x;
		if (vMax.y < PtTbl[i].y)vMax.y = PtTbl[i].y;
		if (vMax.z < PtTbl[i].z)vMax.z = PtTbl[i].z;
	}
	// AABBから、ぴったり収まる大きさの正射影行列作成
	outProj.CreateOrthoLH(vMax.x - vMin.x, vMax.y - vMin.y, 0, vMax.z - vMin.z);

	// AABBの中心(XとY)のビュー行列を作成(ただしZ=0にする)
	KdVec3 vCenter = vMin + (vMax - vMin)*0.5f;
	vCenter.z = vMin.z;
	lightView.Inverse();
	vCenter.Transform(lightView);

	outView.CreateMove(vCenter);
	outView.LookTo(lightDir, { 0, 1, 0 });
	outView.Inverse();
}

void GenShadowMapShader::DrawModelDepth(const KdModel & model)
{
	//-------------------
	// シェーダをセット
	//-------------------
		// スキンメッシュの時
	if (model.GetMesh()->IsSkinMesh())
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

	// 定数バッファをセット
	m_cb0.GetBuffer().VSSetConstantBuffer(0);
	m_cb0.GetBuffer().PSSetConstantBuffer(0);
	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュの情報(頂点バッファ、インデックスバッファ、プリミティブ・トポロジーの3つ)をデバイスへセット
	model.GetMesh()->SetToDevice();

	//--------------------
	// マテリアルループ
	//--------------------
	for (UINT mi = 0; mi < model.GetMaterials().size(); mi++)
	{
		// 面数が0ならスキップ
		if (model.GetMesh()->GetSubset()[mi].FaceCount == 0)continue;

		// マテリアルデータの参照
		auto& mate = model.GetMaterials()[mi];

		// Diffuseテクスチャ
		if (mate.texDiffuse)
		{
			mate.texDiffuse->PSSetShaderResource(0);
		}
		else
		{
			KD3D.GetWhiteTex()->PSSetShaderResource(0);
		}

		// 描画
		model.GetMesh()->DrawSubset(mi);
	}
}

void GenShadowMapShader::Generate(std::function<void()> drawProc)
{
	// 平行光源が無いなら、何もしない
	if (ShMgr.m_cb8_Light.GetWork_Read().DL_Cnt == 0)return;

	// 深度マップを解除
	KdTexture::s_emptyTex.PSSetShaderResource(21);

	// RTをクリア
	m_depthMap->ClearRT({ 1,1,1,1 });
	m_zBuffer->ClearDepth();

	for(int ci = 0; ci < ShMgr.m_cb8_Light.GetWork_Read().CascadeNum; ci++)
	{
		// ライトカメラのビュー・射影行列作成
		KdMatrix mLV;
		KdMatrix mLP;

		// 仮に現在の射影行列を作成(距離を50mに制限するため)
		float ang;
		float aspect;
		float fNear, fFar;
		// 現在の射影行列のデータを逆算する
		ShMgr.m_mProj.ComputePerspectiveInfo(ang, aspect, fNear, fFar);
		// Farを50mに制限した射影行列を作成
		KdMatrix		mTmpProj;
		mTmpProj.CreatePerspectiveFovLH(ang, aspect,
			ci == 0 ? fNear : ShMgr.m_cb8_Light.GetWork().CascadeDist[ci-1],
			ShMgr.m_cb8_Light.GetWork().CascadeDist[ci]);
		// カメラのビュー行列と射影行列から、ライトカメラを算出
		CreateLightViewProj(
			ShMgr.m_mView,		// カメラのビュー行列
			mTmpProj,			// カメラの射影行列
			ShMgr.m_cb8_Light.GetWork().DL[0].Dir,		// ライトの方向
			mLV,				// ライトの方向
			mLP					// 視錘台の範囲にぴったり収まった行列が入る
		);

		// 適当にビュー・射影を作成
		//mLV.LookTo(ShMgr.m_cb8_Light.GetWork().DL[0].Dir, { 0,1,0 });
		//mLV.Move_Local(0, 0, -50);
		//mLV.Inverse();

		//mLP.CreateOrthoLH(30, 30, 0.1f, 100);		// 正射影行列(ポイントライトの影なら透視投影)

		ShMgr.m_cb8_Light.GetWork().DirLightVP[ci] = mLV * mLP;		// ライトカメラのビュー＊射影
		ShMgr.m_cb8_Light.GetWork().CascadeNowIdx = ci;
		ShMgr.m_cb8_Light.Write();

		{
			// 現在のRTとZバッファを記憶する
			KdRTSaver rtSaver;

			// ラスタライザステート
			KdStateSaver sSaver;
			sSaver.SaveRS();
			KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Default_ZClipOff);

			{
				// RTとZバッファを変更
				KdRTChanger rtc;
				rtc.RT(0, m_depthMap->GetRTV_Array(ci));
				rtc.Depth(m_zBuffer->GetDSV_Array(ci));
				rtc.SetToDevice();

				// 描画関数実行
				drawProc();
			}
		}

	}

	// 深度マップをセットしておく
	m_zBuffer->PSSetShaderResource(21);
}

void GenShadowMapShader::Init()
{
	Release();

	//------------------
	// 頂点シェーダ
	//------------------
	{
		// コンパイル済みのシェーダーへヘッダーファイルをインクルード
#include "GenShadowMap_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(KD3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS)))
		{
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return;
		}

		// 頂点インプットレイアウト作成
		auto VDecl = KdVertex_Pos_UV_TBN::GetDeclaration();
		if (FAILED(KD3D.GetDev()->CreateInputLayout(
			&VDecl.m_Layout[0],
			VDecl.m_Layout.size(),
			compiledBuffer,
			sizeof(compiledBuffer),
			&m_VLayout
		))
			) {
			assert(0 && "頂点レイアウト作成失敗");
			Release();
			return;
		}
	}

	// SkinMesh頂点シェーダ読み込み
	{
#include "GenShadowMap_SkinVS.inc"

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


	//---------------------------
	// ピクセルシェーダ
	//---------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
#include "GenShadowMap_PS.inc"

		if (FAILED(KD3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS)))
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return;
		}
	}

	//--------------------------------
	// 定数バッファ
	//--------------------------------
	m_cb0.Create();

	//--------------------------------
	//--------------------------------
	m_depthMap = KdMakeSPtr<KdTexture>();
	m_depthMap->CreateRT(1024, 1024, DXGI_FORMAT_R32_FLOAT, 4);
	m_zBuffer = KdMakeSPtr<KdTexture>();
	m_zBuffer->CreateDepth(1024, 1024, DXGI_FORMAT_R32_TYPELESS, 4);
}
