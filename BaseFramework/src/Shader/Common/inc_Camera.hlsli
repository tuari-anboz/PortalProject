// 定数バッファのb7以降は固定(共通)バッファとする
// 定数バッファ(カメラ)
cbuffer cbCamera : register(b7)
{
	row_major float4x4	g_mV;		// ビュー行列
	row_major float4x4	g_mP;		// 射影行列
	float3				g_CamPos;	// カメラ座標

	// 距離フォグ
	float g_DistanceFogDensity;		// フォグ密度
	float3 g_DistanceFogColor;		// フォグ色
}