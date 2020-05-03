// inc_PostProcess.hlsli


// 頂点シェーダ出力
// (ピクセルシェーダの入力データにもなる)
struct VSOutput
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD0;
};