// 
#include "inc_Model.hlsli"

// �J����
#include "../Common/inc_Camera.hlsli"
// ���C�g
#include "../Common/inc_Light.hlsli"

// �e�N�X�`��
Texture2D g_DiffuseTex : register(t0);		// 
Texture2D g_ToonTex : register(t1);			// �g�D�[���}�b�v
Texture2D g_NormalTex : register(t2);		// �@���}�b�v
Texture2D g_RMTex : register(t3);			// �e���E�����}�b�v
Texture2D g_EmissiveTex : register(t4);		// �����}�b�v

Texture2D g_SceneTex : register(t10);		// �����_�����O���ꂽ�V�[�����
TextureCube g_EnvTex : register(t11);			// IBL�e�N�X�`��
Texture2DArray g_ShadowTex : register(t21);		// �V���h�E�}�b�v

// �T���v��
SamplerState	g_WrapSmp : register(s0);
SamplerState	g_ClampSmp : register(s1);
// �V���h�E�}�b�v�p ��r�@�\�t���T���v��
SamplerComparisonState g_CompSmp : register(s10);

// ���˗͌v�Z(Specular)
// lightDir�c���̕����AwN�c�@���̕����x�N�g���AvCam�c�J�����̕����x�N�g���Apower�c���˂̉s��
// �߂�l�c�܂Ԃ���
float CalcSpecular(float3 lightDir, float3 wN, float3 vCam, float power)
{
	// Phong���f��
	//float3 vRef = reflect(lightDir, wN);
	//float spec = dot(vRef, vCam);		// -1�`1
	// Blinn-Phong���f��
	float3 vHalf = normalize(vCam - lightDir);
	float spec = dot(wN, vHalf);
	spec = saturate(spec);				// (0�`1)
	spec = pow(spec, power + 0.0001);		// �܂Ԃ���

	// ���K��(�G�l���M�[�ۑ��̖@�������)
	spec *= (power + 2) / (2 * 3.14159265);

	return spec;
}


// �s�N�Z���V�F�[�_
float4 main(VSOutput In) : SV_Target0
{
	// ���̃s�N�Z������J�����ւ̕���
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);		// �J�����̋���
	vCam /= camDist;		// ���K��(normalize)

	// �r���[���W�n�ł̋���
	float viewDist = mul(float4(In.wPos, 1), g_mV).z;

	// �@���𐳋K��
	//float3 wN = normalize(In.wN);
	// �@���s��
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};
	// �@���}�b�v����@�����擾
	float3 wN = g_NormalTex.Sample(g_WrapSmp, In.UV).xyz;
	wN = wN * 2.0 - 1.0;
	wN = normalize(wN);
	// �@���s��ŕϊ�
	wN = mul(wN, mTBN);


	// �ŏI�I�ȐF
	float3 color = 0;

	//=============================
	// �ގ��̐F
	//=============================
	// �e�N�X�`������F���擾
	// �F = �e�N�X�`��.sample(�g�p����T���v��.UV���W)
	float4 texColor = g_DiffuseTex.Sample(g_WrapSmp, In.UV);
	// ***************************************
	// ���j�A���[�N�t���[
	// SRGB�����j�A��Ԃ֕ϊ�����
	// IBL�e�N�X�`���̎d�l�Ɉˑ��H�H�H
	texColor.rgb = pow(texColor.rgb, 2.2);
	//****************************************
	// �ގ��̐F
	float4 mateColor = texColor * g_Diffuse * g_MulColor;
	// �A���t�@�e�X�g
	if (mateColor.a < 0.01f)
	{
		discard;	// ���̃s�N�Z�����̂Ă�
	}

	// �e���E�����}�b�v
	float2 texRM = g_RMTex.Sample(g_WrapSmp, In.UV).rg;
	float roughness = g_Roughness * texRM.r;
	float metallic = g_Metallic * texRM.g;

	// �g�U���́A�������قǍގ��̐F���o�Ȃ��Ȃ�
	const float3 c_diff = mateColor.rgb * (1.0 - metallic);
	// ���ˌ��́A�������قǍގ��̐F���o��
	const float3 c_spec = lerp(0.04, mateColor.rgb, metallic);

	//=============================
	// ���C�e�B���O
	//=============================
	if (g_LightEnable)
	{
		// --- ���s�� ---
		for (int di = 0; di < g_DL_Cnt; di++)
		{
			//// CascadeShadowMap��Index�����߂�
			//int csmIdx = 0;
			//for (int ci = 0; ci < g_CascadeNum; ci++)
			//{
			//	csmIdx += viewDist > g_CascadeDist[ci];
			//}
			//csmIdx = min(g_CascadeNum - 1, csmIdx);

			// ����for���̍œK��
			int4 comparison = viewDist.xxxx > g_CascadeDist;
			int csmIdx = dot(comparison, 1);
			csmIdx = min(g_CascadeNum - 1, csmIdx);

			// �V���h�E�}�b�v����
			float shadow = 1;		// 0:�e 1:�e����Ȃ�
			if (di == 0)
			{
				// ���C�g�J�����̕��Ɏˉe�ϊ�
				float4 liPos = mul(float4(In.wPos, 1), g_DirLightVP[csmIdx]);
				// �ˉe���W�ϊ������ꍇ�͕K��w�Ŋ��邱��
				liPos.xyz /= liPos.w;
				// �[�x�}�b�v�͈͓̔��H
				if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
				{
					// �ˉe���W -> UV���W�֕ϊ�
					float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;
					// ���C�g�J��������̋���
					float z = liPos.z - 0.005;		// �V���h�E�A�N�l�΍�

					// �[�x�}�b�v�̒l
					//float r = g_ShadowTex.Sample(g_WrapSmp, uv).r;
					// �e����
					//shadow = r > z;

					// ��r�T���v���ɂ��PCF
					// �E�w��UV�̈ʒu��r������z���r���A����(0�`1)��Ԃ�
					// �EPCF(Percentage Closer Filtering)�ɂ��A�ڂ�����
					//   �l���Ԃ��Ă���
					//shadow = g_ShadowTex.SampleCmpLevelZero(
					//	g_CompSmp, uv, z
					//);

					// ������PCF���s���A�傫���ڂ���
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

			// �g�U��(Lambert)
			float lightDiffuse = dot(-g_DL[di].Dir, wN);		// -1�`1
			lightDiffuse = saturate(lightDiffuse);			// 0�`1

			// �g�D�[��(lightDiffuse��U���W(UV���W��X���W)�Ƃ��ĐF�擾)
			float3 toonColor = g_ToonTex.Sample(g_ClampSmp, float2(lightDiffuse, 0.5)).rgb;

			// ���K��Lambert(�G�l���M�[�ۑ��̖@�������I)
			lightDiffuse /= 3.14;
			toonColor /= 3.14;

			// �����̐F * �ގ��̐F
			color += (g_DL[di].Color * toonColor) * c_diff.rgb * shadow;


			// ���ˌ�(Phong)
			//float3 vRef = reflect(g_DL[di].Dir, wN);
			//float spec = dot(vRef, vCam);		// -1�`1
			//spec = saturate(spec);				// (0�`1)
			//spec = pow(spec, g_SpePower + 0.0001);		// �܂Ԃ���

			// ���ˌ�(Bllin - Phong)
			float spec = CalcSpecular(g_DL[di].Dir, wN, vCam, g_SpePower);
			spec *= (1 - g_Roughness);

			// �ގ��̔��ːF(��) * (���̐F * �܂Ԃ���)
			color += g_Specular * (g_DL[di].Color * spec) * c_spec * shadow;
		}

		// --- �_�� ---
		for (int pi = 0; pi < g_PL_Cnt; pi++)
		{
			float3 lightDir = In.wPos - g_PL[pi].Pos;
			// ����
			float dist = length(lightDir);
			if (dist < g_PL[pi].Radius)
			{
				// ���C�g�������K��
				lightDir /= dist;
				// lightDir = normalize(lightDir);

				// ������
				float atte = 1.0 - (dist / g_PL[pi].Radius);
				atte *= atte;

				// �g�U��(Lambert)
				float lightDiffuse = dot(-lightDir, wN);		// -1�`1
				lightDiffuse = saturate(lightDiffuse);			// 0�`1
				lightDiffuse /= 3.14159265;		// ���K��
				lightDiffuse *= atte;		// ���̌���

				// �����̐F * �ގ��̐F
				color += (g_PL[pi].Color * lightDiffuse) * c_diff.rgb;

				// ���ˌ�(Blinn-Phong)
				float spec = CalcSpecular(lightDir, wN, vCam, g_SpePower);
				spec *= (1 - roughness);
				spec *= atte;

				// �ގ��̔��ːF(��) * (���̐F * �܂Ԃ���)
				color += g_Specular * (g_PL[pi].Color * spec) * c_spec;

			}
		}

		// --- ���� ---
		color += g_AmbientLight * c_diff.rgb;

		// --- ���Ȕ��� ---
		float3 emiColor = g_EmissiveTex.Sample(g_WrapSmp, In.UV).rgb;
		color += g_Emissive + emiColor;

		// --- IBL(Image Based Lighting) ---

		// �g�U��
		float3 envDiff = g_EnvTex.SampleLevel(g_WrapSmp,wN,7).rgb / 3.14159265;		// ���K��
		// SRGB��Ԃ̃e�N�X�`���̏ꍇ�� pow(envDiff, 2.2) ������
		//envDiff = pow(envDiff, 1 / 2.2);		// ��
		color += envDiff * c_diff.rgb * g_IBLIntensity;

		// ���ˌ�
		float3 vRef = reflect(-vCam, wN);
		float3 envSpec = g_EnvTex.SampleLevel(g_WrapSmp, vRef,roughness * 8).rgb;
		envSpec = pow(envSpec, 1 / 2.2);		// ��
		color += envSpec * c_spec * g_IBLIntensity;
	}
	else    // ���C�g��OFF�̎��̓}�e���A���̐F���̂܂�
	{
		color = mateColor.rgb;
	}

	// ���ܕ\��
	if (g_RefractEnable)
	{
		// �J�����������g�p���ċ��܃x�N�g�������߂�
		float3 vRefract = refract(-vCam, wN, 0.7);
		float3 v = vRefract - (-vCam);
		vRefract = mul(v, (float3x3)g_mV);

		// ���̃s�N�Z���̉�ʏ�ł�UV���W�����߂�
		// 3D���W���ˉe���W�֕ϊ�
		float4 posProj = mul(float4(In.wPos, 1), g_mV);
		posProj.xy += v * 0.3;
		posProj = mul(posProj, g_mP);
		posProj.xyz /= posProj.w;
		// ���܃x�N�g�������ɏ������炷
		posProj.xy += vRefract.xy * 0.01;
		// �ˉe���W����UV���W�֕ϊ�
		float2 uv = posProj.xy * float2(1, -1);
		uv = uv * 0.5 + 0.5;

		// �V�[���摜����uv�̈ʒu�̐F���擾
		float4 sceneColor = g_SceneTex.Sample(g_ClampSmp, uv);

		// �����̐F�Ɣw�i�̐F���A���t�@�u�����h
		// �� n = lerp(a,b,t) �c a�`b�Ԃ�t�̈ʒu�̒l�����߂�
		color = lerp(sceneColor.rgb,color.rgb,mateColor.a);
		mateColor.a = 1;	// �s�����ɂ���(DirectX���ŃA���t�@�u�����h�����Ȃ��悤�ɂ����)
	}

	if (g_FogEnable)
	{
		// �����t�H�O
		float f = 1.0 / exp(camDist * g_DistanceFogDensity);
		f = saturate(f);	// 0:���S�ɖ����������Ă���
		color = lerp(g_DistanceFogColor, color, f);
	}

	// RGBA
	return float4(color,mateColor.a);
}