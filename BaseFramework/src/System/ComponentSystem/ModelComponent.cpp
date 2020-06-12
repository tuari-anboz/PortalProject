#include "main.h"
#include "../GameManager.h"
#include "ModelComponent.h"

void ModelComponent::SetModel(const std::string & filename)
{
	m_modelFilename = filename;

	// まず解放
	m_bc.Release();			// ボーンコントローラ解放
	m_anim.ResetTrack();	// アニメ停止

	// 新しいモデルを読み込む
	m_model = KDResFactory.GetGameModel(filename);
	// その他設定
	if (m_model) {
		// m_model用のボーンに初期化する
		m_bc.Init(*m_model);

		// BoneControllerとアニメータを接続する
		m_bc.BindAnimator(m_anim);

		// アニメーションを登録
		m_anim.SetAnimationList(m_model->GetAnimeList());
		// アニメーション再生
		m_anim.ChangeAnime(m_playAnimename, m_loopAnime);

		// 全ボーンを再計算
		m_bc.CalcBoneMatrix();
		// 全ボーンの行列を定数バッファに書き込む
		m_bc.WriteBoneConstantBuffer();
	}
}

void ModelComponent::ChangeEmissiveMap()
{
}

void ModelComponent::Update()
{
	// 無効時はなにもしない
	if (m_enable == false)return;

	// アニメーションを進行し、mDeltaにRootボーンの
	// 変化量の行列が入ってくる
	KdMatrix mDelta;
	if (m_onScriptProc) {
		m_anim.Animation(1.0, &mDelta, m_onScriptProc);
	}
	else {
		m_anim.Animation(1.0, &mDelta);
	}
	// 変化量行列を合成する
	KdMatrix m = mDelta * GetOwner()->GetMatrix();
	GetOwner()->SetMatrix(m, false);


	// 全ボーンを再計算
	m_bc.CalcBoneMatrix();
	// 全ボーンの行列を定数バッファに書き込む
	m_bc.WriteBoneConstantBuffer();
}

void ModelComponent::Draw()
{
	// 屈折無しの時のみ
	if (m_enableRefract == true)return;

	// 無効時はなにもしない
	if (m_enable == false)return;
	// モデルがないときは描画しない
	if (m_model == nullptr)return;

	// 水面表現
	if (m_enableWater)
	{
		// 波法線マップをセット
		ShMgr.m_ModelSh.SetForceUseNormalMap(GameMgr.GetWaveTex().m_rtNormal);
	}
	// 屈折表現On/Off
	ShMgr.m_ModelSh.SetRefractEnable(false);
	// 合成色セット
	ShMgr.m_ModelSh.SetMulColor(m_mulColor);
	// フォグのOn/Off
	ShMgr.m_ModelSh.SetFogEnable(m_enableFog);
	// ライトのOn/Off
	ShMgr.m_ModelSh.SetLightEnable(m_lightEnable);
	// 粗さ
	ShMgr.m_ModelSh.SetRoughness(m_roughness);
	// 金属度
	ShMgr.m_ModelSh.SetMetallic(m_metallic);

	// シェーダに行列をセット
	ShMgr.m_ModelSh.SetWorld(GetOwner()->GetMatrix());
	// シェーダで描画
	//ShMgr.m_KdModelSh.DrawGameModel(*m_model, &m_bc);
	ShMgr.m_ModelSh.DrawGameModel(*m_model, &m_bc);

	if (m_enableWater)
	{
		// 波法線マップを解除
		ShMgr.m_ModelSh.SetForceUseNormalMap(nullptr);
	}
	// 合成色
	ShMgr.m_ModelSh.SetMulColor(KdVec4{ 1,1,1,1 });
}

void ModelComponent::DrawTransparency()
{
	// 屈折ありの時のみ
	if (m_enableRefract == false)return;

	// 無効時はなにもしない
	if (m_enable == false)return;
	// モデルがないときは描画しない
	if (m_model == nullptr)return;

	// 屈折表現On/Off
	ShMgr.m_ModelSh.SetRefractEnable(true);
	// 水面表現
	if (m_enableWater)
	{
		// 波法線マップをセット
		ShMgr.m_ModelSh.SetForceUseNormalMap(GameMgr.GetWaveTex().m_rtNormal);
	}
	// 合成色セット
	ShMgr.m_ModelSh.SetMulColor(m_mulColor);
	// フォグのOn/Off
	ShMgr.m_ModelSh.SetFogEnable(m_enableFog);
	// ライトのOn/Off
	ShMgr.m_ModelSh.SetLightEnable(m_lightEnable);
	// 粗さ
	ShMgr.m_ModelSh.SetRoughness(m_roughness);
	// 金属度
	ShMgr.m_ModelSh.SetMetallic(m_metallic);

	// シェーダに行列をセット
	ShMgr.m_ModelSh.SetWorld(GetOwner()->GetMatrix());
	// シェーダで描画
	//ShMgr.m_KdModelSh.DrawGameModel(*m_model, &m_bc);
	ShMgr.m_ModelSh.DrawGameModel(*m_model, &m_bc);

	if (m_enableWater)
	{
		// 波法線マップを解除
		ShMgr.m_ModelSh.SetForceUseNormalMap(nullptr);
	}
	// 合成色
	ShMgr.m_ModelSh.SetMulColor(KdVec4{ 1,1,1,1 });
	ShMgr.m_ModelSh.SetRefractEnable(false);
}

void ModelComponent::DrawShadowMap()
{
	// 無効時はなにもしない
	if (m_enable == false)return;
	// モデルがないときは描画しない
	if (m_model == nullptr)return;

	// シェーダに行列をセット
	ShMgr.m_genShadowMapSh.SetWorld(GetOwner()->GetMatrix());
	// シェーダで描画
	ShMgr.m_genShadowMapSh.DrawGameModelDepth(*m_model);
}

void ModelComponent::ImGuiUpdate()
{
	// 継承元クラスのImGuiUpdateを実行
	BaseComponent::ImGuiUpdate();

	// モデルファイル
	if (ImGuiResourceButton("Model File", m_modelFilename, { ".xed" })) {
		SetModel(m_modelFilename);
	}

	//---------------------
	// 再生アニメ名
	//---------------------
	if (ImGui::ListBoxHeader("Play Anime", ImVec2(0, 100)))
	{
		if (ImGui::Selectable(u8"-- 再生しない --")) {
			m_playAnimename = "";
			m_anim.ResetTrack();
		}
		// 全アニメ名
		for (auto&& ani : m_anim.GetAnimeList()) {

			// 選択中？
			bool bSelected = false;
			if (m_playAnimename == ani->m_AnimeName) {
				bSelected = true;
			}

			if (ImGui::Selectable(filesystem::path(ani->m_AnimeName).u8string().c_str(), bSelected)) {
				// 選択されたアニメ名を記憶
				m_playAnimename = ani->m_AnimeName;
				// アニメ変更
				m_anim.ChangeAnime(m_playAnimename, m_loopAnime);
			}
		}

		ImGui::ListBoxFooter();
	}

	// ループ再生
	if (ImGui::Checkbox("Loop Anime", &m_loopAnime)) {
		m_anim.GetTrack()[0]->m_Loop = m_loopAnime;
	}

	//========================================
	// グラフィックス設定
	//========================================
	// ライトのOn/Off
	ImGui::Checkbox(u8"ライトのOn/Off", &m_lightEnable);
	// 
	ImGui::SliderFloat(u8"粗さ", &m_roughness, 0, 1);
	// 金属度
	ImGui::SliderFloat(u8"金属度", &m_metallic, 0, 1);
	// 水面表現
	ImGui::Checkbox(u8"水面表現のOn/Off", &m_enableWater);
	// 屈折表現
	ImGui::Checkbox(u8"屈折表現のOn/Off", &m_enableRefract);
	// 合成色指定
	ImGui::ColorEdit4(u8"色", m_mulColor);
	// フォグOn/Off
	ImGui::Checkbox(u8"フォグ", &m_enableFog);

	// ※filesystem::path(文字列).u8string()　は文字列をutf8の文字コードへ変換している処理です。
}

void ModelComponent::EditorDraw()
{
	if (m_enable == false)return;
	if (m_model == nullptr)return;

	// 現在の描画ステートを保存し、
	// この関数が終わるときに復元される
	KdStateSaver stateSaver;
	stateSaver.SaveDS();
	stateSaver.SaveRS();

	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareOFF_ZWriteOFF, 0);
	// ワイヤーフレームモードにする
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_WireframeBoth);

	ShMgr.m_KdPrimSh.SetTexture(nullptr);

	for (auto&& model : m_model->GetModels()) {
		ShMgr.m_KdPrimSh.DrawMesh(*model->GetMesh(), { 1, 1, 1, 1 }, &GetOwner()->GetMatrix());
	}
}
