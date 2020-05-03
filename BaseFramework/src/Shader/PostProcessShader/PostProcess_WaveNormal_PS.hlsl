// WaveNormal PS

#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��(�����}�b�v)
Texture2D g_HeightTex : register(t0);

// �T���v��(Linear��� Clamp���[�h)
SamplerState g_Smp : register(s0);

// 
float4 main(VSOutput In) : SV_Target0
{
    // �摜�̕��ƍ������Q�b�g(�s�N�Z��)
    float w, h;
    g_HeightTex.GetDimensions(w, h);
    // �s�N�Z�� -> �e�N�Z���ϊ�
    float tw = 1.0 / w;
    float th = 1.0 / h;

    // ���S�_�Ɨאڂ���4�_���T���v�����O
//    float4 t0 = g_HeightTex.Sample(g_Smp, In.UV);
    float4 t1 = g_HeightTex.Sample(g_Smp, In.UV + float2(-tw, 0));
    float4 t2 = g_HeightTex.Sample(g_Smp, In.UV + float2(tw, 0));
    float4 t3 = g_HeightTex.Sample(g_Smp, In.UV + float2(0, -th));
    float4 t4 = g_HeightTex.Sample(g_Smp, In.UV + float2(0, th));
    
    float3 normal;
    normal.x = t1.r - t2.r;
    normal.y = t3.r - t4.r;
    normal.z = 1;
    normal = normalize(normal);

    // -1�`1����0�`1�֕ϊ�
    normal = normal * 0.5 + 0.5;

    return float4(normal, 1);
}
