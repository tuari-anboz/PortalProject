// 
#include "inc_Model.hlsli"

// カメラ
#include "../Common/inc_Camera.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, // 座標
			  float2 uv : TEXCOORD0, // UV座標
			  float3 tangent : TANGENT,	// 接線
			  float3 binormal : BINORMAL,	// 従法線
			  float3 normal : NORMAL // 法線ベクトル
			)
{
	VSOutput Out;

	// 座標変換
	Out.Pos = mul(pos, g_mW);		// Local座標からWorld座標
	Out.wPos = Out.Pos.xyz;			// ワールド座標を取得
	Out.Pos = mul(Out.Pos, g_mV);	// World座標からView座標
	Out.Pos = mul(Out.Pos, g_mP);	// VIew座標から射影座標

	// ワールド法線を出力
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	Out.wB = normalize(mul(binormal, (float3x3)g_mW));
	Out.wN = normalize(mul(normal, (float3x3)g_mW));

	// UV座標
	Out.UV = uv;
	
	return Out;
}