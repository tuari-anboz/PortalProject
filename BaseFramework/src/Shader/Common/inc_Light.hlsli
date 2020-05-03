// 定数バッファ(ライト)
cbuffer cbLight : register(b8)
{
	//-------------------------
	// 環境光
	//-------------------------
	float3 g_AmbientLight;

	//-------------------------
	// 平行(ディレクショナル)光
	//-------------------------
	int g_DL_Cnt;		// 平行光の数
	struct DLData
	{
		float3 Dir;		// 方向
		float3 Color;	// 色
		float tmp;
	};
	DLData  g_DL[3];	// 平行光の配列		

	//-------------------------
	// 点(ポイント)光
	//-------------------------
	int g_PL_Cnt;	// 点光の数

	struct PLData
	{
		float4 Pos;		// 座標
		float3 Color;	// 色
		float  Radius;	// 半径
	};
	PLData g_PL[100];		// 点光の配列

	//-------------------------
	// スポット光
	//-------------------------

	//-------------------------
	// IBL
	//-------------------------
	float g_IBLIntensity;		// IBL輝度

	//-------------------------
	// シャドウマッピング用(平行光の0番目専用)
	//-------------------------
	//// ライトカメラのビュー＋射影行列
	//row_major float4x4 g_DirLightVP;
	// ライトカメラのビュー＋射影行列配列
	row_major float4x4 g_DirLightVP[4];
	int g_CascadeNowIdx;		// 現在何番目のライトカメラを使ってる？
	int g_CascadeNum;			// カスケード数(分割数1～4)
	float4 g_CascadeDist;		// 0～3の距離
	// float g_CascadeDist[4]; じゃダメ
}