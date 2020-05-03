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

// 定数バッファ(ボーン)
cbuffer cBone : register(b1)
{
	// ボーン行列配列
	row_major float4x4 g_mBones[1024];
}

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, // 頂点座標(ローカル)
	float2 uv : TEXCOORD0, // UV
	float4 blendWeight : BLENDWEIGHT, // ボーンウェイトx4　ここに、 この頂点に影響しているボーンの影響度が入ってくる　最大4つ(x,y,z,w)
	uint4 blendIndex : BLENDINDICES	 // ボーン番号4　ここに、この頂点に影響しているボーン番号が入ってくる  最大４つ(x,y,z,w)
)
{
	//---------------------------------------------------------
	// スキニング処理
	// この頂点に影響している複数のボーンの行列を使用して
	// posやnormalなどを変換する
	//---------------------------------------------------------
	row_major float4x4 mBones = 0; // 最終的なワールド行列
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_mBones[blendIndex[i]] * blendWeight[i];
	}
	// ボーン行列で変換(まだローカル座標系)
	pos = mul(pos, mBones);
	//----------------------------------------------------------

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