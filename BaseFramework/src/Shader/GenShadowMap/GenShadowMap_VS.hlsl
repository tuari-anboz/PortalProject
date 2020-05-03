#include "inc_GenShadowMap.hlsli"

// カメラデータ
#include "../Common/inc_Camera.hlsli"
// ライトデータ
#include "../Common/inc_Light.hlsli"

// 定数バッファ
cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;
};

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, // 頂点座標(ローカル)
	float2 uv : TEXCOORD0 // UV
)
{
	VSOutput result;

	// 射影変換
	result.Pos = mul(pos, g_mW);
	result.Pos = mul(result.Pos, g_DirLightVP[g_CascadeNowIdx]);
	// 射影座標をコピーしてPSに持っていく
	// ※result.PosはPS前にさらに変換され、射影座標にはなっていないため(セマンティクスがPOSITIONのは魔改造される)
	result.wvpPos = result.Pos;

	// UVはそのまま入れる
	result.UV = uv;

	return result;
}