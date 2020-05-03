// 
#include "inc_Model.hlsli"

// カメラ
#include "../Common/inc_Camera.hlsli"
// ライト
#include "../Common/inc_Light.hlsli"

// テクスチャ
Texture2D g_DiffuseTex : register(t0);		// 
Texture2D g_ToonTex : register(t1);			// トゥーンマップ
Texture2D g_NormalTex : register(t2);		// 法線マップ
Texture2D g_RMTex : register(t3);			// 粗さ・金属マップ
Texture2D g_EmissiveTex : register(t4);		// 発光マップ

Texture2D g_SceneTex : register(t10);		// レンダリングされたシーン画面
TextureCube g_EnvTex : register(t11);			// IBLテクスチャ
Texture2DArray g_ShadowTex : register(t21);		// シャドウマップ

// サンプラ
SamplerState	g_WrapSmp : register(s0);
SamplerState	g_ClampSmp : register(s1);
// シャドウマップ用 比較機能付きサンプラ
SamplerComparisonState g_CompSmp : register(s10);

// 反射力計算(Specular)
// lightDir…光の方向、wN…法線の方向ベクトル、vCam…カメラの方向ベクトル、power…反射の鋭さ
// 戻り値…まぶしさ
float CalcSpecular(float3 lightDir, float3 wN, float3 vCam, float power)
{
	// Phongモデル
	//float3 vRef = reflect(lightDir, wN);
	//float spec = dot(vRef, vCam);		// -1～1
	// Blinn-Phongモデル
	float3 vHalf = normalize(vCam - lightDir);
	float spec = dot(wN, vHalf);
	spec = saturate(spec);				// (0～1)
	spec = pow(spec, power + 0.0001);		// まぶしさ

	// 正規化(エネルギー保存の法則を守る)
	spec *= (power + 2) / (2 * 3.14159265);

	return spec;
}


// ピクセルシェーダ
float4 main(VSOutput In) : SV_Target0
{
	// このピクセルからカメラへの方向
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);		// カメラの距離
	vCam /= camDist;		// 正規化(normalize)

	// ビュー座標系での距離
	float viewDist = mul(float4(In.wPos, 1), g_mV).z;

	// 法線を正規化
	//float3 wN = normalize(In.wN);
	// 法線行列
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};
	// 法線マップから法線を取得
	float3 wN = g_NormalTex.Sample(g_WrapSmp, In.UV).xyz;
	wN = wN * 2.0 - 1.0;
	wN = normalize(wN);
	// 法線行列で変換
	wN = mul(wN, mTBN);


	// 最終的な色
	float3 color = 0;

	//=============================
	// 材質の色
	//=============================
	// テクスチャから色を取得
	// 色 = テクスチャ.sample(使用するサンプラ.UV座標)
	float4 texColor = g_DiffuseTex.Sample(g_WrapSmp, In.UV);
	// ***************************************
	// リニアワークフロー
	// SRGBをリニア空間へ変換する
	// IBLテクスチャの仕様に依存？？？
	texColor.rgb = pow(texColor.rgb, 2.2);
	//****************************************
	// 材質の色
	float4 mateColor = texColor * g_Diffuse * g_MulColor;
	// アルファテスト
	if (mateColor.a < 0.01f)
	{
		discard;	// このピクセルを捨てる
	}

	// 粗さ・金属マップ
	float2 texRM = g_RMTex.Sample(g_WrapSmp, In.UV).rg;
	float roughness = g_Roughness * texRM.r;
	float metallic = g_Metallic * texRM.g;

	// 拡散光は、金属質ほど材質の色が出なくなる
	const float3 c_diff = mateColor.rgb * (1.0 - metallic);
	// 反射光は、金属質ほど材質の色が出る
	const float3 c_spec = lerp(0.04, mateColor.rgb, metallic);

	//=============================
	// ライティング
	//=============================
	if (g_LightEnable)
	{
		// --- 平行光 ---
		for (int di = 0; di < g_DL_Cnt; di++)
		{
			//// CascadeShadowMapのIndexを求める
			//int csmIdx = 0;
			//for (int ci = 0; ci < g_CascadeNum; ci++)
			//{
			//	csmIdx += viewDist > g_CascadeDist[ci];
			//}
			//csmIdx = min(g_CascadeNum - 1, csmIdx);

			// ↑のfor文の最適化
			int4 comparison = viewDist.xxxx > g_CascadeDist;
			int csmIdx = dot(comparison, 1);
			csmIdx = min(g_CascadeNum - 1, csmIdx);

			// シャドウマップ判定
			float shadow = 1;		// 0:影 1:影じゃない
			if (di == 0)
			{
				// ライトカメラの方に射影変換
				float4 liPos = mul(float4(In.wPos, 1), g_DirLightVP[csmIdx]);
				// 射影座標変換した場合は必ずwで割ること
				liPos.xyz /= liPos.w;
				// 深度マップの範囲内？
				if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
				{
					// 射影座標 -> UV座標へ変換
					float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;
					// ライトカメラからの距離
					float z = liPos.z - 0.005;		// シャドウアクネ対策

					// 深度マップの値
					//float r = g_ShadowTex.Sample(g_WrapSmp, uv).r;
					// 影判定
					//shadow = r > z;

					// 比較サンプラによるPCF
					// ・指定UVの位置のr成分とzを比較し、結果(0～1)を返す
					// ・PCF(Percentage Closer Filtering)により、ぼかした
					//   値が返ってくる
					//shadow = g_ShadowTex.SampleCmpLevelZero(
					//	g_CompSmp, uv, z
					//);

					// 自分でPCFも行い、大きくぼかす
					float w, h, elements;
					g_ShadowTex.GetDimensions(w, h,elements);
					float ts = 1.0 / w;
					shadow = 0;
					for (int y = -1; y <= 1; y++)
					{
						for (int x = -1; x <= 1; x++)
						{
							float2 uv2 = uv + float2(x*ts, y*ts);
							shadow += g_ShadowTex.SampleCmpLevelZero(
								g_CompSmp, float3(uv2,csmIdx), z
							);
						}
					}
					shadow /= 9;
				}
			}

			// 拡散光(Lambert)
			float lightDiffuse = dot(-g_DL[di].Dir, wN);		// -1～1
			lightDiffuse = saturate(lightDiffuse);			// 0～1

			// トゥーン(lightDiffuseをU座標(UV座標のX座標)として色取得)
			float3 toonColor = g_ToonTex.Sample(g_ClampSmp, float2(lightDiffuse, 0.5)).rgb;

			// 正規化Lambert(エネルギー保存の法則を守る！)
			lightDiffuse /= 3.14;
			toonColor /= 3.14;

			// ※光の色 * 材質の色
			color += (g_DL[di].Color * toonColor) * c_diff.rgb * shadow;


			// 反射光(Phong)
			//float3 vRef = reflect(g_DL[di].Dir, wN);
			//float spec = dot(vRef, vCam);		// -1～1
			//spec = saturate(spec);				// (0～1)
			//spec = pow(spec, g_SpePower + 0.0001);		// まぶしさ

			// 反射光(Bllin - Phong)
			float spec = CalcSpecular(g_DL[di].Dir, wN, vCam, g_SpePower);
			spec *= (1 - g_Roughness);

			// 材質の反射色(力) * (光の色 * まぶしさ)
			color += g_Specular * (g_DL[di].Color * spec) * c_spec * shadow;
		}

		// --- 点光 ---
		for (int pi = 0; pi < g_PL_Cnt; pi++)
		{
			float3 lightDir = In.wPos - g_PL[pi].Pos;
			// 距離
			float dist = length(lightDir);
			if (dist < g_PL[pi].Radius)
			{
				// ライト方向正規化
				lightDir /= dist;
				// lightDir = normalize(lightDir);

				// 減衰力
				float atte = 1.0 - (dist / g_PL[pi].Radius);
				atte *= atte;

				// 拡散光(Lambert)
				float lightDiffuse = dot(-lightDir, wN);		// -1～1
				lightDiffuse = saturate(lightDiffuse);			// 0～1
				lightDiffuse /= 3.14159265;		// 正規化
				lightDiffuse *= atte;		// 光の減衰

				// ※光の色 * 材質の色
				color += (g_PL[pi].Color * lightDiffuse) * c_diff.rgb;

				// 反射光(Blinn-Phong)
				float spec = CalcSpecular(lightDir, wN, vCam, g_SpePower);
				spec *= (1 - roughness);
				spec *= atte;

				// 材質の反射色(力) * (光の色 * まぶしさ)
				color += g_Specular * (g_PL[pi].Color * spec) * c_spec;

			}
		}

		// --- 環境光 ---
		color += g_AmbientLight * c_diff.rgb;

		// --- 自己発光 ---
		float3 emiColor = g_EmissiveTex.Sample(g_WrapSmp, In.UV).rgb;
		color += g_Emissive + emiColor;

		// --- IBL(Image Based Lighting) ---

		// 拡散光
		float3 envDiff = g_EnvTex.SampleLevel(g_WrapSmp,wN,7).rgb / 3.14159265;		// 正規化
		// SRGB空間のテクスチャの場合は pow(envDiff, 2.2) をする
		//envDiff = pow(envDiff, 1 / 2.2);		// 仮
		color += envDiff * c_diff.rgb * g_IBLIntensity;

		// 反射光
		float3 vRef = reflect(-vCam, wN);
		float3 envSpec = g_EnvTex.SampleLevel(g_WrapSmp, vRef,roughness * 8).rgb;
		envSpec = pow(envSpec, 1 / 2.2);		// 仮
		color += envSpec * c_spec * g_IBLIntensity;
	}
	else    // ライトがOFFの時はマテリアルの色そのまま
	{
		color = mateColor.rgb;
	}

	// 屈折表現
	if (g_RefractEnable)
	{
		// カメラ方向を使用して屈折ベクトルを求める
		float3 vRefract = refract(-vCam, wN, 0.7);
		float3 v = vRefract - (-vCam);
		vRefract = mul(v, (float3x3)g_mV);

		// このピクセルの画面上でのUV座標を求める
		// 3D座標を射影座標へ変換
		float4 posProj = mul(float4(In.wPos, 1), g_mV);
		posProj.xy += v * 0.3;
		posProj = mul(posProj, g_mP);
		posProj.xyz /= posProj.w;
		// 屈折ベクトル方向に少しずらす
		posProj.xy += vRefract.xy * 0.01;
		// 射影座標からUV座標へ変換
		float2 uv = posProj.xy * float2(1, -1);
		uv = uv * 0.5 + 0.5;

		// シーン画像からuvの位置の色を取得
		float4 sceneColor = g_SceneTex.Sample(g_ClampSmp, uv);

		// 物質の色と背景の色をアルファブレンド
		// ※ n = lerp(a,b,t) … a～b間のtの位置の値を求める
		color = lerp(sceneColor.rgb,color.rgb,mateColor.a);
		mateColor.a = 1;	// 不透明にする(DirectX側でアルファブレンドさせないようにする為)
	}

	if (g_FogEnable)
	{
		// 距離フォグ
		float f = 1.0 / exp(camDist * g_DistanceFogDensity);
		f = saturate(f);	// 0:完全に霧がかかっている
		color = lerp(g_DistanceFogColor, color, f);
	}

	// RGBA
	return float4(color,mateColor.a);
}