// WaveHeight PS

#include "inc_PostProcess.hlsli"

// 定数バッファ
cbuffer cbWave : register(b0)
{
	float2 g_AddPowerUV;		// 意図的に力を加える座標
	float	g_AddPowerRadius;	// その半径	
	float	g_AddPower;			// 威力
}

// 入力テクスチャ(高さマップ)
Texture2D g_HeightTex : register(t0);

// サンプラ(Linear補間 Wrappモード)
SamplerState g_LinearWrapSmp : register(s2);

float4 main(VSOutput In) : SV_Target0
{
	// 画像の幅と高さをゲット(ピクセル)
	float w,h;
	g_HeightTex.GetDimensions(w, h);
	// ピクセル -> テクセル変換
	float tw = 1.0 / w;
	float th = 1.0 / h;
	// 中心点と隣接する4点をサンプリング 
	float4 t0 = g_HeightTex.Sample(g_LinearWrapSmp, In.UV); 
	float4 t1 = g_HeightTex.Sample(g_LinearWrapSmp, In.UV + float2(-tw, 0)); 
	float4 t2 = g_HeightTex.Sample(g_LinearWrapSmp, In.UV + float2(tw, 0));  
	float4 t3 = g_HeightTex.Sample(g_LinearWrapSmp, In.UV + float2(0, th));   
	float4 t4 = g_HeightTex.Sample(g_LinearWrapSmp, In.UV + float2(0, -th)); 

	// このピクセルにかかっている力 
	float F = (t1.r + t2.r + t3.r + t4.r - t0.r * 4) * 0.01;
	
	// 力を元に、速さを更新
	t0.g += F;

	// 速さをもとに高さを更新
	t0.r += t0.g;

	// 意図的に力を加える
	// ※distance(p1,p2) … p1～p2の距離を返す
	if (distance(In.UV, g_AddPowerUV) < g_AddPowerRadius)
	{
		t0.g += g_AddPower;
	}

	// R:高さ G:速さ
	return float4(t0.r, t0.g, 0, 1);
}