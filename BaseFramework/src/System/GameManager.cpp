#include "main.h"

#include "GameManager.h"


void Object::UpdateObject()
{
	// 有効なら所持しているコンポーネントを処理
	if (m_isActive)
	{
		// 各コンポーネントの初回実行処理
		for (auto && comp : m_components)
		{
			comp->CallStart();
		}

		if (GameMgr.m_Editor_IsPlay)
		{
			// 全コンポーネントの処理を実行
			for (auto&& comp : m_components)
			{
				// 有効なコンポーネントのみ処理を実行
				if (comp->GetEnable())
				{
					comp->Update();
				}
			}
		}
	}

	// 削除フラグがOnのコンポーネントを消す
	{
		auto it = m_components.begin();
		while (it != m_components.end()) {
			// 削除フラグOn?
			if ((*it)->IsDelete()) {
				// リストから削除して、次のやつを受け取る
				it = m_components.erase(it);
				continue;
			}
			// 次のやつへ
			++it;
		}
	}

	// 親ボーンに追従させる
	if (m_followBoneName.size() > 0)
	{
		// 親オブジェクト
		if (auto parent = GetParent())
		{
			// 親のModelComponent
			if (auto model = parent->GetComponent<ModelComponent>())
			{
				// BoneController取得
				auto & bc = model->GetBc();
				// 指定名のボーンを検索
				auto bone = bc.SearchBone(m_followBoneName);
				if (bone)
				{
					// ボーンのワールド行列を求める
					KdMatrix mBone = bone->LocalMat * parent->GetMatrix();
					// 自分にセット
					SetMatrix(mBone, true);
				}
			}
		}
	}


	// 更新処理
	Update();

	// 有効なら子を処理
	if (m_isActive)
	{
		// 子を処理
		for (auto&& child : m_childs)
		{
			child->UpdateObject();
		}
	}

	// 削除フラグがOnの子を消す
	{
		auto it = m_childs.begin();
		while (it != m_childs.end())
		{
			// 削除フラグOn?
			if ((*it)->IsDelete())
			{
				// リストから削除して、次のやつをうけとる
				it = m_childs.erase(it);
				continue;
			}
			// 次のやつへ
			++it;
		}
	}

}

void Object::PreDrawObject()
{
	// 全コンポーネントの処理を実行
	for (auto&& comp : m_components) {
		comp->PreDraw();
	}

	// 描画前処理
	PreDraw();

	// 子を処理
	for (auto&& child : m_childs)
	{
		child->PreDrawObject();
	}
}

void Object::DrawObject()
{
	// 有効なら描画処理を実行
	if (m_isActive)
	{
		// 全コンポーネントの処理を実行
		for (auto&& comp : m_components) {
			comp->Draw();
		}

		// 描画処理
		Draw();

		// 子を処理
		for (auto&& child : m_childs)
		{
			child->DrawObject();
		}
	}
}

void Object::DrawTransparencyObject()
{
	// 有効なら描画処理を実行
	if (m_isActive)
	{
		// 全コンポーネントの処理を実行
		for (auto&& comp : m_components) {
			comp->DrawTransparency();
		}

		// 描画処理
		DrawTransparency();

		// 子を処理
		for (auto&& child : m_childs) {
			child->DrawTransparencyObject();
		}
	}
}

void Object::DrawShadowMapObject()
{
	// 有効なら描画処理を実行
	if (m_isActive)
	{
		// 全コンポーネントの処理を実行
		for (auto&& comp : m_components) {
			comp->DrawShadowMap();
		}

		// 描画処理
		DrawTransparency();

		// 子を処理
		for (auto&& child : m_childs) {
			child->DrawShadowMapObject();
		}
	}
}


void Object::DrawEffectObject()
{
	// 有効なら描画処理を実行
	if (m_isActive)
	{
		// 全コンポーネントの処理を実行
		for (auto&& comp : m_components) {
			comp->DrawEffect();
		}

		// 描画
		DrawEffect();

		// 子を処理
		for (auto&& child : m_childs) {
			child->DrawEffectObject();
		}
	}
}


void Object::ImGuiUpdate()
{
	// 当たり判定のマルチスレッドをON
	ColMgr.SetMultiThread(true);

	// クラス名を表示
	ImGui::Separator();
	ImGui::Text((GetObjectClassName() + " Class").c_str());
	ImGui::Separator();
	ImGui::Spacing();

	// コンポーネント追加ボタン
	ImGui::Button("Add Component");
	// 左クリックでコンテキストメニュー表示
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// 登録されている全クラス
		for (auto&& cls : GameMgr.m_componentClassMaker) {
			if (ImGui::Selectable(cls.first.c_str())) {
				// そのコンポーネントを生成
				auto comp = cls.second();
				// 追加
				AddComponent(comp);
			}
		}

		ImGui::EndPopup();
	}


	// 名前
	ImGuiInputString("Name", m_name);
	// 質量
	ImGui::DragFloat(u8"質量", &m_mass,0.01f,0,1000);
	// 行列
	KdMatrix m = GetMatrix();
	ImGuizmoEditTransform(m, ShMgr.m_mView, ShMgr.m_mProj, nullptr);
	SetMatrix(m, true);


	// 全コンポーネントのImGui処理
	for (auto&& comp : m_components) {

		ImGui::PushID( (int)comp.get() );

		bool bOpen = ImGui::CollapsingHeader(comp->ClassName().c_str(),
			ImGuiTreeNodeFlags_DefaultOpen);

		// 右クリックで操作ポップアップ
		if (ImGui::BeginPopupContextItem("ComponentOperation")) {
			// 削除
			if (ImGui::Selectable("Delete")) {
				comp->Delete();	// 削除フラグOn
			}
			if (ImGui::Selectable("MoveUp"))
			{
				comp->MoveUp();
			}
			if (ImGui::Selectable("MoveDown"))
			{
				comp->MoveDown();
			}

			ImGui::EndPopup();
		}

		// CollapsingHeaderを開いているときは内容を表示する
		if (bOpen) {
			comp->ImGuiUpdate();
		}


		ImGui::PopID();
	}
}

void Object::Deserialize(const json11::Json & jsonObj)
{
	// 名前
	m_name = jsonObj["Name"].string_value();
	// 行列セット
	m_mWorld.Deserialize(jsonObj["Matrix"]);
	// 質量
	m_mass = static_cast<float>(jsonObj["Mass"].number_value());		// (float)従来のキャストは非推奨
	// 有効
	if (jsonObj["Active"].is_bool())
	{
		m_isActive = jsonObj["Active"].bool_value();
	}
	// コンポーネント
	auto& compAry = jsonObj["Components"].array_items();
	for (auto&& compJson : compAry) {
		// コンポーネントのクラス名
		std::string className = compJson["ClassName"].string_value();
		// classNameのクラスを生成
		auto comp = GameMgr.m_componentClassMaker[className]();
		// コンポーネントをデシリアライズ
		comp->Deserialize(compJson);
		// コンポーネントリストへ追加
		AddComponent(comp);
	}
	// 子
	auto& childAry = jsonObj["Childs"].array_items();
	for (auto&& childJson : childAry)
	{
		// オブジェクトのクラス名
		std::string className = childJson["ClassName"].string_value();
		// classNameのクラスを生成
		auto obj = GameMgr.m_objectClassMaker[className]();
		// 親を設定(親の子リストに追加される
		obj->SetParent(shared_from_this());
		// コンポーネントをデシリアライズ
		obj->Deserialize(childJson);
	}
}

void GameManager::Init()
{
	//================================
	// システム系のコンポーネントのクラスを登録
	//================================
	REGISTER_COMP_CLASS(PlayerInputComponent);
	REGISTER_COMP_CLASS(BasicInputComponent);
	REGISTER_COMP_CLASS(ModelComponent);
	REGISTER_COMP_CLASS(EffectModelComponent);
	REGISTER_COMP_CLASS(SpriteComponent);
	REGISTER_COMP_CLASS(CameraComponent);
	REGISTER_COMP_CLASS(LightComponent);
	REGISTER_COMP_CLASS(AudioListenerComponent);
	REGISTER_COMP_CLASS(AudioComponent);
	REGISTER_COMP_CLASS(PortalComponent);
	REGISTER_COMP_CLASS(TitleScript);
	REGISTER_COMP_CLASS(PlayerScript);
	REGISTER_COMP_CLASS(FpsCameraComponent);
	REGISTER_COMP_CLASS(LevelEditorScript);
	REGISTER_COMP_CLASS(UIButtonComponent);
	REGISTER_COMP_CLASS(RigidBodyComponent);
	REGISTER_COMP_CLASS(RideButtonComponent);
	REGISTER_COMP_CLASS(BoxGeneratorComponent);
	REGISTER_COMP_CLASS(DoorComponent);
	REGISTER_COMP_CLASS(ButtonPushComponent);
	REGISTER_COMP_CLASS(FadeComponent);
	REGISTER_COMP_CLASS(LoadLevelComponent);

	// コライダーコンポーネントクラス登録
	REGISTER_COMP_CLASS(BoxColliderComponent);
	REGISTER_COMP_CLASS(SphereColliderComponent);
	REGISTER_COMP_CLASS(RayColliderComponent);
	REGISTER_COMP_CLASS(MeshColliderComponent);

	//================================
	// 物理ワールド作成
	//================================
	m_physicsWorld = m_physicsWorld->CreateWorld();
	m_physicsWorld->Init();


	//================================
	// レベルの生成
	//================================
	m_level = std::make_shared<Level>();
	m_level->Load("data/StageDataLevelEditor.json");

	m_Editor_Log.AddLog(u8"Editor起動");

	// (仮) 波テクスチャ作成
	m_wave.Create(512, 512);

	// フェード初期化
	m_fade = false;
}


void GameManager::Run()
{
	//============================
	// レベルの変更処理
	//============================
	if (m_changeLevelFile.size() > 0) 	// 予約がある？
	{
		// Levelを変更する
		m_level = std::make_shared<Level>();
		m_level->Load(m_changeLevelFile);

		m_changeLevelFile = "";	// 予約解消
	}


	//============================
	// 更新処理
	//============================
	if (m_Editor_IsPlay)
	{
		// 物理世界の時間をすすめる(計算する)。
		m_physicsWorld->StepSimulation(1 / 60.0f);
	}

	// レベル更新処理
	m_level->Update();

	KDAudioMgr.Update();

	// (Editor)カメラ処理
	m_Editor_Camera.Update();

	// 波進行
	ShMgr.m_postProcessSh.AdvanceWave(m_wave);

	//============================
	// 描画処理
	//============================

	// ポイントライトリセット
	ShMgr.m_KdModelSh.ResetPointLight();
	// 並行ライトリセット(真っ黒にしただけ)
	ShMgr.m_KdModelSh.SetDirectionalLight({ 0,0,0 }, { 0,0,0,0 });
	// スポットライトリセット
	ShMgr.m_KdModelSh.ResetSpotLight();

	// ライトリセット
	ShMgr.m_cb8_Light.GetWork().DL_Cnt = 0;
	ShMgr.m_cb8_Light.GetWork().PL_Cnt = 0;

	//--------------
	// カメラ設定
	//--------------
	m_Editor_Camera.SetToShader();

	// バックバッファをクリアする
	KD3D.GetBackBuffer()->ClearRT({ 0.2f,0.2f,0.2f, 1 });
	// Zバッファをクリアする
	KD3D.GetZBuffer()->ClearDepth();

	// レベル描画処理
	m_level->Draw();

	if (m_Editor_showDebug)
	{
		// 当たり判定デバッグ表示(最終的には消す)
		ColMgr.DebugDraw();

		// 選択物のエディター描画を実行
		{
			auto select = m_Editor_SelectObj.lock();
			if (select)
			{
				// 子のエディター描画も実行
				std::function<void(const SPtr<Object>&)> rec = [this, &rec](const SPtr<Object>& obj)
				{
					obj->EditorDrawObject();

					// 子を再帰
					for (auto&& child : obj->GetChilds())
					{
						rec(child);
					}
				};
				// 関数を実行
				rec(select);
			}
		}
	}

	//===========================
	// ImGui描画
	//===========================
	if (m_Editor_showUI)
	{
		// ImGui開始
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGuizmo
		ImGuizmo::BeginFrame();

		// 実行中はGUIウィンドウを赤くする
		bool isPlay = m_Editor_IsPlay;
		if(isPlay)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0, 0, 0.6f));
		}

		// ImGui Demo ウィンドウ表示 ※すごく参考になるウィンドウです。imgui_demo.cpp参照。
		ImGui::ShowDemoWindow(nullptr);
		ImGuiUpdate();

		ImGui::ShowMetricsWindow(nullptr);

		if (isPlay)
		{
			ImGui::PopStyleColor(1);
		}

		// GUI描画実行
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	if (KdIsKeyDown(VK_F1, false))
	{
		m_Editor_showUI = !m_Editor_showUI;
	}

	if (KdIsKeyDown(VK_F2, false))
	{
		m_Editor_showDebug = !m_Editor_showDebug;
	}

	// バックバッファの内容を画面に表示
	KD3D.GetSwapChain()->Present(0, 0);

}

void GameManager::ImGuiUpdate()
{

	// レベルのImGui処理
	m_level->ImGuiUpdate();

	//=============================
	// GraphicsDebugウィンドウ
	//=============================
	if (ImGui::Begin("Graphics Debug", nullptr)) {
		// 波用 高さマップ
		ImGui::Image(m_wave.m_rtHeight[0]->GetSRV(),
			ImVec2(200, 200));
		// 波用 法線マップ
		ImGui::Image(m_wave.m_rtNormal->GetSRV(),
			ImVec2(200, 200));
		// シャドウマップデバッグ表示
		for (int i = 0; i < 4; i++)
		{
			// GetSRV_Array(i) … テクスチャ配列のi番目のSRVを指定
			ImGui::Image(ShMgr.m_genShadowMapSh.GenDepthMap()->GetSRV_Array(i), ImVec2(100, 100));
		}
	}
	ImGui::End();

	//=============================
	// Inspectorウィンドウ
	//=============================
	if (ImGui::Begin("Inspector", nullptr)) {
		SPtr<Object> select = m_Editor_SelectObj.lock();
		if (select) {
			// 
			select->ImGuiUpdate();

		}
	}
	ImGui::End();

	//==================================
	// GameManagerウィンドウ
	//==================================
	if (ImGui::Begin("Game Manager", nullptr))
	{
		// 実行
		if (ImGui::Checkbox(u8"実行", &m_Editor_IsPlay)) {
			static json11::Json::object saveLevel;

			// 実行
			if (m_Editor_IsPlay) {
				// 現在のレベルをシリアライズ
				m_level->Serialize(saveLevel);
			}
			// 停止
			else {
				m_level->Deserialize(saveLevel);
				saveLevel.clear();
			}
		}
		ImGuiShowHelp(u8"実行中はUpdateが実行される。", false);

		// カメラコンポーネントを使用する
		ImGui::Checkbox(u8"カメラコンポーネント使用", &m_Editor_UseCameraComponent);
		ImGuiShowHelp(u8"カメラコンポーネントが存在する場合は、それを使用する", false);
	}
	ImGui::End();

	//==================================
	// ログウィンドウ
	//==================================
	m_Editor_Log.ImGuiUpdate("Log Window");
}

SPtr<Object> GameManager::Instantiate(const json11::Json & jsonObj)
{
	if (jsonObj.is_null()) return nullptr;

	std::string className = jsonObj["ClassName"].string_value();
	// 存在チェック 
	if (m_objectClassMaker.count(jsonObj["ClassName"].string_value()) == 0) return nullptr;
	// 生成   
	SPtr<Object> newObj = m_objectClassMaker[className]();

	// デシリアライズ 
	newObj->Deserialize(jsonObj);

	return newObj;
}

SPtr<Object> GameManager::Duplicate(const SPtr<Object> obj)
{

	// objをシリアライズ(JSONデータ化)
	json11::Json::object serial;
	obj->Serialize(serial);
	// ↓↓↓
	// 新規オブジェクトを作成
	std::string className = serial["ClassName"].string_value();
	SPtr<Object> newObj = GameMgr.m_objectClassMaker[className]();
	// デシリアライズ
	newObj->Deserialize(serial);
	// Rootの子にする
	newObj->SetParent(obj->GetParent());
	// こいつを選択状態にする
	GameMgr.m_Editor_SelectObj = newObj;
	return newObj;
}



void Object::SetMatrix(const KdMatrix & m, bool moveChild)
{
	// 変化前の行列をバックアップ
	m_mPrevWorld = m_mWorld;

	// 行列セット
	m_mWorld = m;

	// 変化前と今回の行列から、1フレームでの変化量算出
	KdMatrix mInvPrev;
	m_mPrevWorld.Inverse(mInvPrev);
	KdMatrix mDelta = mInvPrev * m_mWorld;

	// 子も連動させる
	if (moveChild)
	{
		// 子以下のすべてのGameObjectのTransformにmDeltaを合成していく関数
		std::function<void(const SPtr<Object>&)> proc =  [this, &proc,&mDelta](const SPtr<Object>& obj)
		{
			// 変化前の行列をバックアップ
			obj->m_mPrevWorld = obj->m_mWorld;
			// 適用
			obj->m_mWorld *= mDelta;

			// 子再帰
			for (auto&& child : obj->GetChilds())
			{
				proc(child);
			}
		};

		// 子を処理する
		for (auto&& child : GetChilds())
		{
			proc(child);
		}
	}
}

