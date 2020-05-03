// 頂点シェーダ出力用構造体

// 定数バッファ(オブジェクト)
cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;	// ワールド行列
	float4 g_MulColor;			// 合成色
	int g_LightEnable;			// ライトOn/Off
	float g_Roughness;			// 粗さ(0～1)
	float g_Metallic;			// 金属度(0～1)
	int g_RefractEnable;		// 屈折表現On/Off
	int g_FogEnable;			// フォグOn/Off
};

// 定数バッファ(マテリアル)
cbuffer cbMaterial : register(b1)
{
	float4 g_Diffuse;	// 拡散色
	float3 g_Specular;	// 反射色
	float  g_SpePower;	// 反射の鋭さ
	float3 g_Emissive;	// 発光色
};

// 定数バッファ(ボーン)
cbuffer cBone : register(b2)
{
	// ボーン行列配列
	row_major float4x4 g_mBones[1024];
}

// 頂点シェーダ出力用構造体
struct VSOutput
{
	float4 Pos : SV_Position;		// 射影座標
	float2 UV : TEXCOORD1;			// UV座標
	float3 wPos : TEXCOORD2;		// ワールド座標

	float3 wT : TEXCOORD3;			// ワールド接線
	float3 wB : TEXCOORD4;			// ワールド従法線
	float3 wN : TEXCOORD0;			// ワールド法線
};