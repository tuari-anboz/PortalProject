// 
#include "inc_Model.hlsli"

// カメラ
#include "../Common/inc_Camera.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, // 座標
	float2 uv : TEXCOORD0, // UV座標
	float3 tangent : TANGENT,	// 接線
	float3 binormal : BINORMAL, // 従法線 
	float3 normal : NORMAL, // 法線ベクトル
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
	tangent = mul(tangent, (float3x3)mBones);
	binormal = mul(binormal, (float3x3)mBones);
	normal = mul(normal, (float3x3)mBones);
	//----------------------------------------------------------

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