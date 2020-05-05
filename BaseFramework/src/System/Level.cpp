#include "main.h"

#include "GameManager.h"

Level::Level()
{
	// バックバッファと同じサイズのRT作成
	for (int i = 0; i < 2; i++)
	{
		m_rtWork[i].CreateRT(
			KD3D.GetBackBuffer()->GetInfo().Width,
			KD3D.GetBackBuffer()->GetInfo().Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT
		);
	}
	m_rtHighBrightness.CreateRT(
		KD3D.GetBackBuffer()->GetInfo().Width,
		KD3D.GetBackBuffer()->GetInfo().Height,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
	m_blurTex.Create(KD3D.GetBackBuffer()->GetInfo().Width,
		KD3D.GetBackBuffer()->GetInfo().Height
	);
}

// Serialize/Deserialize

void Level::Deserialize(const json11::Json & jsonObj)
{
	// ルートを作成
	m_root = std::make_shared<Object>();

	// Rootオブジェクト
	json11::Json jsn = jsonObj["Object"];
	// デシリアライズ
	m_root->Deserialize(jsn);
	// グラフィック設定
	if (jsonObj["GraphicSetting"].is_object())
	{
		auto& graSetting = jsonObj["GraphicSetting"];
		m_fogColor.Deserialize(graSetting["FogColor"]);
		m_fogDensity = (float)graSetting["FogDensity"].number_value();
	}
}

void Level::Serialize(json11::Json::object & outJson)
{
	// 
	json11::Json::object root;
	m_root->Serialize(root);

	outJson["Object"] = root;

	// グラフィック設定
	json11::Json::object graphicSetting;
	graphicSetting["FogColor"] = m_fogColor.Serialize();
	graphicSetting["FogDensity"] = m_fogDensity;
	outJson["GraphicSetting"] = graphicSetting;
}

void Level::Load(const std::string& filename)
{
	// レベルファイル名を記憶しておく
	m_levelFilename = filename;


	// ルートを作成
	m_root = std::make_shared<Object>();

	// JSONファイルの内容を全て文字列として読み込む。
	std::string str = KdLoadStringFile(filename);
	// JSON解析
	std::string err;
	json11::Json jsn = json11::Json::parse(str, err);
	if (err.size() > 0) {
		// 解析エラー　errにエラー内容が文字列で入っている
		assert(0 && "JSON解析エラー！");
	}

	Deserialize(jsn);

	// IBLテクスチャ読み込み
	m_texIBL = KDResFactory.GetTexture("data/Texture/IBL/Park.dds");
	if (m_texIBL)m_texIBL->PSSetShaderResource(11);

	/*
	// Rootオブジェクト
	json11::Json jsonObj = jsn["Object"];
	// デシリアライズ
	m_root->Deserialize(jsonObj);
	*/
}

void Level::Save()
{
	/*
	// 
	json11::Json::object root;
	m_root->Serialize(root);

	// ステージデータ
	json11::Json::object stageData;
	stageData["Object"] = root;
	// 文字列化
	json11::Json jsn = stageData;
	std::string strJson = jsn.dump();	// 文字列へ変換
	*/

	json11::Json::object serial;
	Serialize(serial);

	// ファイルへ保存
	json11::Json jsn = serial;
	std::string strJson = jsn.dump();	// 文字列へ変換
	KdSaveStringFile(m_levelFilename, strJson);
}

void Level::Update()
{
	// ①判定オブジェクトリストをクリアする
	ColMgr.ClearList();

	// ルートオブジェクト実行
	m_root->UpdateObject();

	// ③と④コリジョン処理実行(②で登録されたコライダー達を判定し、結果を通知する)
	ColMgr.Run();


}

void Level::Draw()
{
	//========================
	// グラフィックス設定
	//========================
	// フォグ設定
	ShMgr.m_cb7_Camera.GetWork().DistanceFogColor = m_fogColor;
	ShMgr.m_cb7_Camera.GetWork().DistanceFogDensity = m_fogDensity;
	ShMgr.m_cb7_Camera.Write();

	// オブジェクトの描画前処理
	m_root->PreDrawObject();

	// ライトデータを書き込む
	ShMgr.m_cb8_Light.Write();
	
	// オブジェクトの3D描画処理
	//m_root->DrawObject();

	// (0)シャドウマップ生成
	{
		// 関数を作成
		auto drawProc = [this]()
		{
			m_root->DrawShadowMapObject();
		};
		ShMgr.m_genShadowMapSh.Generate(drawProc);
	}

	// オブジェクトの3D描画処理
	{
		// 現在のRT,Zバッファと保存
		KdRTSaver rtSave;

		// RT,Zバッファ変更用
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[0]);
		rtc.Depth(*KD3D.GetZBuffer());
		rtc.SetToDevice();		// RT,Zバッファを切り替える

		// RTをクリア
		m_rtWork[0].ClearRT({ 0, 0, 0, 1 });
		// Zバッファをクリア
		KD3D.GetZBuffer()->ClearDepth();

		// 全物体を描画
		m_root->DrawObject();
	}

	// ②Work[1]へコピー
	{
		// 現在のRT,Zバッファと保存
		KdRTSaver rtSave;

		// RT,Zバッファ変更用
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[1]);
		rtc.Depth(nullptr);
		rtc.SetToDevice();		// RT,Zバッファを切り替える

		ShMgr.m_postProcessSh.CopyDraw(m_rtWork[0]);
	}

	// ③半透明物体描画
	{
		// 現在のRT,Zバッファと保存
		// 現在のRT,Zバッファと保存
		KdRTSaver rtSave;

		// RT,Zバッファ変更用
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[1]);
		rtc.Depth(*KD3D.GetZBuffer());
		rtc.SetToDevice();		// RT,Zバッファを切り替える

		// Work[0]をシェーダにセットしておく
		m_rtWork[0].PSSetShaderResource(10);

		// 半透明物体を描画
		m_root->DrawTransparencyObject();

		// テクスチャを解除
		KdTexture::s_emptyTex.PSSetShaderResource(10);
	}

	// ④高輝度抽出
	{
		// 現在のRT,Zバッファと保存
		KdRTSaver rtSave;

		// RT,Zバッファ変更用
		KdRTChanger rtc;
		rtc.RT(0, m_rtHighBrightness);
		rtc.Depth(nullptr);
		rtc.SetToDevice();		// RT,Zバッファを切り替える

		// ②Workから高輝度抽出
		ShMgr.m_postProcessSh.BrightFilteringDraw(m_rtWork[1]);
	}

	// ⑤高輝度抽出画像をMGF
	ShMgr.m_postProcessSh.GenerateBlur(m_blurTex, m_rtHighBrightness);

	// ⑥全ぼかし画像を加算合成描画
	{
		KdStateSaver stSave;
		stSave.SaveBS();		// 現在のBlendStateを記憶しておく

		float blendFactor[] = { 0,0,0,0 };
		// 加算合成に切り替え
		KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAdd, blendFactor, 0xFFFFFFF);

		// RTを切り替える
		{
			// 現在のRT,Zバッファと保存
			KdRTSaver rtSave;

			// RT,Zバッファ変更用
			KdRTChanger rtc;
			rtc.RT(0, m_rtWork[1]);
			rtc.Depth(nullptr);
			rtc.SetToDevice();		// RT,Zバッファを切り替える

			//
			for (int i = 0; i < 5; i++)
			{
				ShMgr.m_postProcessSh.CopyDraw(m_blurTex.m_rt[i][0], { 0.2f, 0.2f, 0.2f, 1 });	// 20%
			}
		}
	}

	// ⑦Work -> BackBuffer
	//ShMgr.m_postProcessSh.CopyDraw(m_rtWork);
	ShMgr.m_postProcessSh.ToneMappingDraw(m_rtWork[1]);

	// オブジェクトのエフェクト描画
	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteOFF, 0);	// Z書き込みOFF
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Both);									// 両面描画
	m_root->DrawEffectObject();
	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteON, 0);	// Z書き込みON
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Default);								// 表面描画

	// 全Objectの2D描画
	ShMgr.m_KdSpriteSh.Begin(false);
	{
		KdStateSaver StateSaver;
		StateSaver.SaveDS();
		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteOFF, 0);

		m_root->DrawSpriteObject();

		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteON, 0);
	}
	ShMgr.m_KdSpriteSh.End();
}

void Level::ImGuiUpdate()
{
	//=============================
	// ObjectListウィンドウ
	//=============================
	if (ImGui::Begin("Object List", nullptr, ImGuiWindowFlags_MenuBar))
	{
		// メインメニュー
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// Newメニュー
				if (ImGui::MenuItem("New")) {
					// Rootオブジェクトを作成
					m_root = std::make_shared<Object>();
					m_root->GetName() = "Root";
				}
				// Loadメニュー
//				if (ImGui::MenuItem("Load")) {
//					Load();
//				}
				ImGui::Selectable("Load", false, ImGuiSelectableFlags_DontClosePopups);
				{
					std::string filename;
					if (ImGuiBeginPopup_ResourceBrowser(
						"LoadLevelPopup", filename, { ".json" }
					)) {
						// 現在のPopupウィンドウを消す
						ImGui::CloseCurrentPopup();
						// レベル読み込み
						Load(filename);
					}
				}

				// Saveメニュー
				if (ImGui::MenuItem("Save")) {
					Save();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// 現在のレベルファイル名
		ImGuiInputString("Level File", m_levelFilename);

		if (ImGui::BeginTabBar("LevelTabBar", 0))
		{
			if (ImGui::BeginTabItem("Scene Setting"))
			{
				// オブジェクト追加ボタン
				if (GameMgr.GetSelectObj().expired() == false) {
					ImGui::Button("Add Object");
					ImGuiShowHelp(u8"選択Objectの子として、新規Objectを追加する", false);
					// 左クリックでコンテキストメニュー表示
					if (ImGui::BeginPopupContextItem("AddObjectPopup", 0)) {
						// 登録されている全クラス
						for (auto&& cls : GameMgr.m_objectClassMaker) {
							if (ImGui::Selectable(cls.first.c_str())) {
								// そのクラスを生成
								SPtr<Object> p = cls.second();
								// 追加
								//AddObject(p);
								// 選択物の子として追加
								p->SetParent(GameMgr.GetSelectObj().lock());
								// こいつを選択にする
								GameMgr.SetSelectObj(p);
							}
						}

						ImGui::EndPopup();
					}
				}

				// オブジェクトリスト
		//		for (auto&& obj : m_objectList) {
				// １つのObjectのImGui処理
				std::function<void(const SPtr<Object>&)> rec = [this, &rec](const SPtr<Object>& obj)
				{
					// アドレスを識別IDとして使う
					ImGui::PushID((int)obj.get());

					/*
					// このobjが選択中？
					bool bSelected = false;
					if (GameMgr.m_Editor_SelectObj.lock() == obj) {
						bSelected = true;
					}
					// オブジェクト！
					if (ImGui::Selectable(obj->m_name.c_str(), bSelected))
					{
						GameMgr.m_Editor_SelectObj = obj;
					}
					*/

					// ツリーの動作フラグ
					int treeFlags = ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_OpenOnDoubleClick |
						ImGuiTreeNodeFlags_OpenOnArrow;
					// 選択されている時は、選択状態のフラグを付ける
					if (GameMgr.GetSelectObj().lock() == obj)
					{
						treeFlags |= ImGuiTreeNodeFlags_Selected;
						//-----------------
						// Auto Scroll
						//-----------------
						// オブジェクトが選択されたばかりでかつ見えてないとき
						if (GameMgr.m_Editor_JustSelected && !ImGui::IsItemVisible())
						{
							ImGui::SetScrollHereY();
						}
						GameMgr.m_Editor_JustSelected = false;
					}

					// 選択されたアイテムがツリーの内側に隠れている場合にツリーを開く
					std::function<bool(const SPtr<Object>&)> selectedchild = [this, &selectedchild](const SPtr<Object>& obj)
					{
						if (GameMgr.GetSelectObj().lock() == obj)
						{
							return true;
						}
						else
						{
							// 子を再帰
							for (auto&& child : obj->GetChilds())
							{
								if (selectedchild(child))
								{
									return true;
								}
							}
						}
					};

					// 子を再帰
					for (auto&& child : obj->GetChilds()) {
						if (selectedchild(child))
						{
							ImGui::SetNextTreeNodeOpen(true);
						}
					}

					// ツリーノード
					bool bTreeOpen = ImGui::TreeNodeEx(obj.get(), treeFlags, obj->GetName().c_str());
					
					// ツリーノードがクリックされた時
					if (ImGui::IsItemClicked())
					{
						GameMgr.SetSelectObj(obj);
					}

					//-----------------
					// Drag & Drop
					//-----------------
					// Drag
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Objectのアドレスを数値として記憶する
						intptr_t n = (intptr_t)obj.get();
						ImGui::SetDragDropPayload("ObjectDragDrop", &n, sizeof(n));
						ImGui::Text(u8"親となるObjectを選択してください");
						ImGui::EndDragDropSource();
					}
					// ObjectをDrop
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjectDragDrop"))
						{
							IM_ASSERT(payload->DataSize == sizeof(intptr_t));
							// int型で記憶していたアドレスをObjectに復元
							Object* srcObj = (Object*)(*(intptr_t*)payload->Data);
							// そいつの子になる
							srcObj->SetParent(obj);
						}
						ImGui::EndDragDropTarget();
					}
					//-----------------

					// Ctrl + マウスホイールで順番入れ替え
					if (treeFlags & ImGuiTreeNodeFlags_Selected) {// 選択物
						if (ImGui::IsKeyDown(VK_CONTROL)) {
							if (ImGui::GetIO().MouseWheel > 0) {
								obj->MoveUp();
							}
							else if (ImGui::GetIO().MouseWheel < 0) {
								obj->MoveDown();
							}
						}
					}

					// 右クリックのポップアップメニュー
					if (ImGui::BeginPopupContextItem("ObjectPopupMenu", 1))
					{
						// プレハブファイル化
						ImGui::Selectable("Save Prefab", false, ImGuiSelectableFlags_DontClosePopups);
						{
							std::string filename;
							if (ImGuiBeginPopup_ResourceBrowser(
								"SavePrefabPopup", filename, { ".pref" }, true
							)
								) {
								// 現在のPopupウィンドウを消す
								ImGui::CloseCurrentPopup();

								// フォルダを選択したとき

								// objをシリアライズ(JSONデータ化)
								json11::Json::object serial;
								obj->Serialize(serial);
								// ↓↓↓
								// JSONデータを文字列化
								json11::Json jsn = serial;
								std::string strJson = jsn.dump();
								// ファイルへ保存
		//						KdSaveStringFile(filename + "/test.pref", strJson);
								KdSaveStringFile(filename + "/" + obj->GetName() + ".pref", strJson);

							}

						}

						// プレハブファイルから新規オブジェクト作成
						ImGui::Selectable("Load Prefab", false, ImGuiSelectableFlags_DontClosePopups);
						{
							std::string filename;
							if (ImGuiBeginPopup_ResourceBrowser(
								"LoadPrefabPopup", filename, { ".pref" }
							)
								) {
								// 現在のPopupウィンドウを消す
								ImGui::CloseCurrentPopup();

								// ファイルを選んだ時
								std::string strJson = KdLoadStringFile(filename);

								// JSON解析
								std::string err;
								json11::Json jsn = json11::Json::parse(strJson, err);
								if (err.size() == 0) {
									// 新規オブジェクト作成
									SPtr<Object> p = GameMgr.m_objectClassMaker[jsn["ClassName"].string_value()]();
									if (p) {
										// デシリアライズ
										p->Deserialize(jsn);
										// GameMgrに追加
		//								AddObject(p);
										// Rootの子にする
										p->SetParent(m_root);
										// こいつを選択状態にする
										GameMgr.SetSelectObj(p);
									}
								}

							}
						}

						// 複製
						if (ImGui::Selectable("Duplicate")) {
							// objをシリアライズ(JSONデータ化)
							json11::Json::object serial;
							obj->Serialize(serial);
							// ↓↓↓
							// 新規オブジェクトを作成
							std::string className = serial["ClassName"].string_value();
							SPtr<Object> newObj = GameMgr.m_objectClassMaker[className]();
							// デシリアライズ
							newObj->Deserialize(serial);
							// リストへ追加
		//					AddObject(newObj);
							// Rootの子にする
							newObj->SetParent(obj->GetParent());
							// こいつを選択状態にする
							GameMgr.SetSelectObj(newObj);
						}

						// 削除
						if (ImGui::Selectable("Delete")) {
							obj->Delete();
						}

						// 必ず最後に呼ぶこと
						ImGui::EndPopup();
					}

					// ツリーノードが開かれている
					if (bTreeOpen)
					{
						// 子を再帰
						for (auto&& child : obj->GetChilds()) {
							rec(child);
						}

						// Tree Nodeの終わり
						ImGui::TreePop();
					}

					// 
					ImGui::PopID();	// 識別IDを解除
				};

				// オブジェクトリスト
		//		for (auto&& obj : m_objectList) {
		//			rec(obj);
		//		}
				// ルートからImGui処理実行
				rec(m_root);

				ImGui::EndTabItem();
			}
			// グラフィック設定タブ
			if (ImGui::BeginTabItem("GraphicsSetting"))
			{
				// グラフィック設定
				ImGui::ColorEdit3("Fog Color", m_fogColor);
				ImGui::DragFloat(u8"Fog密度", &m_fogDensity, 0.001f, 0, 10.0f);

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
