#include "main.h"
#include "../GameManager.h"
#include "EffectModelComponent.h"


void EffectModelComponent::DrawEffect()
{
	if (m_enable == false)return;

	if (m_model) {
		// 行列
		KdMatrix m = GetOwner()->GetMatrix();

		// ビルボード
		if (m_billboardType == 1) {
			KdMatrix mCam;
			ShMgr.m_mView.Inverse(mCam);

			m.SetRotation(mCam);
			m.Scale_Local(GetOwner()->GetMatrix().GetScale());
		}
		// Y軸固定ビルボード
		else if (m_billboardType == 2) {
			// カメラへの方向
			KdMatrix mCam;
			ShMgr.m_mView.Inverse(mCam);

			KdVec3 vCam = mCam.GetPos() - m.GetPos();
			KdVec3 vY = m.GetYAxis();
			KdVec3 vX = vCam.Cross(vY);
			KdVec3 vZ = vY.Cross(vX);
			vX.Normalize();
			vY.Normalize();
			vZ.Normalize();
			m._11 = vX.x;
			m._12 = vX.y;
			m._13 = vX.z;
			m._21 = vY.x;
			m._22 = vY.y;
			m._23 = vY.z;
			m._31 = vZ.x;
			m._32 = vZ.y;
			m._33 = vZ.z;
			m.Scale_Local(GetOwner()->GetMatrix().GetScale());
		}

		// 半透明合成
		if (m_blendType == 0) {
		}
		else if (m_blendType == 1) {
			float bf[4] = { 0,0,0,0 };
			KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAdd, bf, 0xFFFFFFFF);
		}

		// 描画
		ShMgr.m_KdPrimSh.SetTexture(m_tex.get());
		ShMgr.m_KdPrimSh.DrawMesh(*m_model->GetModels()[0]->GetMesh(), m_color, &m);
		ShMgr.m_KdPrimSh.SetTexture(nullptr);

		// 半透明合成にもどしておく
		float bf[4] = { 0,0,0,0 };
		KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAlpha, bf, 0xFFFFFFFF);
	}
}

void EffectModelComponent::ImGuiUpdate()
{
	//
	BaseComponent::ImGuiUpdate();

	// ビルボード方法
	std::vector<char*> bbTypes = { u8"なし", u8"通常ビルボード", u8"Y軸固定ビルボード" };
	ImGui::Combo("Billboard Type", &m_billboardType, &bbTypes[0], bbTypes.size());

	// ブレンド方法
	std::vector<char*> blendtypes = { u8"半透明合成", u8"加算合成" };
	ImGui::Combo("Blend Type", &m_blendType, &blendtypes[0], blendtypes.size());

	// 色
	ImGui::ColorEdit4(u8"色", m_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);

	bool bChanged = false;

	// 描画Modelファイル
	if (ImGuiResourceButton(u8"モデル", m_modelFilename, { ".xed" })) {
		bChanged = true;
	}
	ImGuiShowHelp(u8"描画用の3Dモデルを選択", false);

	// 描画Modelファイル
	if (ImGuiResourceButton(u8"テクスチャ", m_texFilename, { ".png", ".jpg", ".tga", ".hdr", ".bmp" })) {
		bChanged = true;
	}
	ImGuiShowHelp(u8"張り付ける画像ファイルを選択", false);

	// 再設定
	if (bChanged) {
		LoadModel(m_modelFilename, m_texFilename);
	}
}
