#include "main.h"

#include "../GameManager.h"

#include "LevelEditorScript.h"

void LevelEditorScript::Start()
{
	// 初期行動
	m_as = std::make_shared<Basic>();
}

void LevelEditorScript::Update()
{
	if (m_enable == false)return;

	// オーナー取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return;

	// 行動処理
	if (m_as) {
		// 持ち主のキャラのアドレスを入れておく
		m_as->m_pObject = this;
		// 行動処理実行
		auto p = m_as->Update();
		if (p) {
			m_as = p;
		}
	}
}

void LevelEditorScript::ImGuiUpdate()
{
	// ObjectのほうのImGuiUpdateを実行
	BaseComponent::ImGuiUpdate();

	// コピー
	ImGui::Checkbox("IsCopy", &m_isCopy);
	// 生成するプレハブファイル
	ImGuiResourceButton("Prefab File", m_generationPrefabName, { ".pref" });
	// プレイヤープレハブファイル
	ImGuiResourceButton("PlayerPrefab", m_playerPrefabName, { ".pref" });
}

void LevelEditorScript::Deserialize(const json11::Json & jsonObj)
{
	// 継承元の関数を実行しておく
	BaseComponent::Deserialize(jsonObj);

	m_isCopy = jsonObj["IsCopy"].bool_value();
	m_generationPrefabName = jsonObj["PrefabName"].string_value();
	m_playerPrefabName = jsonObj["PlayerPrefabName"].string_value();
}

void LevelEditorScript::Serialize(json11::Json::object & outJsonObj)
{
	// 継承元の関数を実行しておく
	BaseComponent::Serialize(outJsonObj);

	outJsonObj["IsCopy"] = m_isCopy;
	outJsonObj["PrefabName"] = m_generationPrefabName;
	outJsonObj["PlayerPrefabName"] = m_playerPrefabName;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Basic::Update()
{
	// オーナー取得
	auto owner = m_pObject->GetOwner();

	// 入力コンポーネント取得
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return nullptr;
	// カメラオブジェクト取得
	auto cam = owner->FindObject("Cam");
	if (cam == nullptr)return nullptr;

	// エディターモード終了
	if (input->m_keysRepeat[VK_SHIFT] && input->m_keys['P'])
	{
		if (m_pObject->m_playerPrefabName.empty())return nullptr;
		// プレイヤーの初期ポジを取得
		auto startObj = GameMgr.m_level->GetRoot()->FindObject("StartPoint");
		if (startObj == false)return nullptr;
		// prefファイルからプレイヤーを生成
		auto playerPref = KDResFactory.GetJson(m_pObject->m_playerPrefabName);
		if (playerPref.NUL)return nullptr;
		auto playerObj = GameMgr.Instantiate(playerPref);
		if (playerObj == false)return nullptr;
		// プレイヤーを初期ポジにセットしてエディターモードを終了する
		KdMatrix mPos = startObj->GetMatrix();
		playerObj->SetMatrix(mPos, true);
		playerObj->SetParent(GameMgr.m_level->GetRoot());
		owner->Delete();
		GameMgr.m_Editor_Log.AddLog(u8"エディターモード終了");
		return nullptr;
	}

	// 選択中のオブジェクトセットがあれば選択中モードへ移行
	if (m_pObject->m_selectedObjectSet.expired() == false)
	{
		return std::make_shared<Selected>();
	}

	KdVec3 vDir;
	if (cam) 
	{
		//カメラのZ方向
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y成分は潰す
		camZ.Normalize();	// 正規化

		// カメラのX方向
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y成分は潰す
		camX.Normalize();	// 正規化

		// カメラのY方向

		KdVec3 moveDir;

		if (input->m_keysRepeat['W'])
		{
			moveDir.z += 1.0f;
		}
		if (input->m_keysRepeat['A'])
		{
			moveDir.x -= 1.0f;
		}
		if (input->m_keysRepeat['S'])
		{
			moveDir.z -= 1.0f;
		}
		if (input->m_keysRepeat['D'])
		{
			moveDir.x += 1.0f;
		}
		if (input->m_keysRepeat['Q'])
		{
			moveDir.y += 1.0f;
		}
		if (input->m_keysRepeat['Z'])
		{
			moveDir.y -= 1.0f;
		}


		// 方向キーをもとに、移動方向を決定
		vDir = (camX * moveDir.x) + (camZ * moveDir.z);
		vDir.y = moveDir.y;

		// 入力がある時
		if (vDir.LengthSquared() > 0) {
			vDir.Normalize();

			// キャラの行列
			KdMatrix m = owner->GetMatrix();

			// キャラの向き(Z)
			KdVec3 vZ = m.GetZAxis();
			vZ.Normalize();	// 正規化

			// 角度を求める
			const float maxAng = 10;
			float dot = vZ.Dot(vDir);
			dot = std::min(1.0f, dot);// if(dot > 1.0f)dot = 1.0f;
			dot = std::max(-1.0f, dot);
			// 角度制限
			float ang = DirectX::XMConvertToDegrees(acos(dot));
			if (ang > maxAng) {
				ang = maxAng;
			}
			// vZをangだけ回転
			// 外積で回転軸を算出
			KdVec3 crs = vZ.Cross(vDir);
			//float L = crs.Length();
			// 垂直ベクトルが求められないときは、
			// 適当に(0, 1, 0)とする。
			if (crs.LengthSquared() <= 0.0001f) {
				crs.Set(0, 1, 0);
			}
			crs.Normalize();

			// crsを軸の回転行列を求める
			KdMatrix mR;
			mR.CreateRotateAxis(crs, ang);
			// vZをmRで変換
			vZ.TransformNormal(mR);
			// vZの方向に向ける
			m.LookTo(vZ, { 0, 1, 0 });

			// 移動
			m.Move(vDir*0.05f);
			owner->SetMatrix(m, false);
		}
	}
	// マウス左クリックで対象のオブジェクトを選択
	if (input->m_keys[VK_LBUTTON])
	{
		// レイ判定のコンポーネントを取得する
		auto coll = cam->GetComponent<RayColliderComponent>();
		coll->SetEnable(true);
		if (coll) {
			// ヒット時に実行される関数を登録
			coll->m_onHitStay = [this](BaseColliderComponent* collider)
			{
				// 全てのあたった物から、一番近いやつを検出
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults())
				{
					// レベルエディター用のタグのみ
					if (res.Collider->GetTag() == "LevelEditor")
					{
						// より近いものがあれば、それを
						if (nearest > res.Dist) {
							nearestRes = &res;
							nearest = res.Dist;
						}
					}
				}
				//
				if (nearestRes)
				{
					m_pObject->m_selectedObjectSet = nearestRes->Collider->GetOwner()->shared_from_this()->GetParent();
					GameMgr.m_Editor_SelectObj = m_pObject->m_selectedObjectSet;
					m_pObject->m_selectedObject = nearestRes->Collider->GetOwner()->shared_from_this();
					GameMgr.m_Editor_Log.AddLog(u8"選択モードへ移行");
				}
				collider->SetEnable(false);
			};
		}
	}



	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Selected::Update()
{
	// 選択中のオブジェクトセットがない場合基本行動に
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// オーナー取得
	auto owner = m_pObject->GetOwner();

	// 入力コンポーネント取得
	auto input = owner->GetComponent<InputComponent>();

	// Cキーで作成モードへ
	if (input->m_keys['C'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"作成モードへ移行");
		return std::make_shared<Create>();
	}

	// Rキーで編集モードへ
	if (input->m_keys['R'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"編集モードへ移行");
		return std::make_shared<Edit>();
	}

	// Tキーで置換モードへ
	if (input->m_keys['T'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"置換モードへ移行");
		return std::make_shared<Replace>();
	}

	// 右クリックで選択解除
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択を解除しました");
		return std::make_shared<Basic>();
	}

	// DELETEキーでオブジェクトセットを削除
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択したオブジェクトセットを削除しました");
	}

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Create::Update()
{
	// 選択中のオブジェクトセットがない場合基本行動に
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// 選択しているプレハブファイルがない場合
	if (m_pObject->m_isCopy == false)
	{
		if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();
	}

	// オーナー取得
	auto owner = m_pObject->GetOwner();
	if (owner == nullptr) return nullptr;

	// 入力コンポーネント取得
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr) return nullptr;

	// DELETEキーでオブジェクトを削除
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択したオブジェクトセットを削除しました");
	}

	// 右クリックで選択解除
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択を解除しました");
		return std::make_shared<Basic>();
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (input->m_keys[VK_RIGHT])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(2.0f, 0, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (input->m_keys[VK_LEFT])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(-2.0f, 0, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (input->m_keys['Q'])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 2.0f, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	// 選択したオブジェクトセットの上下左右に生成する
	if (input->m_keys['Z'])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, -2.0f, 0);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, -2.0f, 0);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}
	// 選択したオブジェクトセットの前後に生成する
	if (input->m_keys[VK_UP])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 0, 2.0f);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}
	// 選択したオブジェクトセットの前後に生成する
	if (input->m_keys[VK_DOWN])
	{
		SPtr<Object> newObj;
		if (m_pObject->m_isCopy)
		{
			newObj = GameMgr.Duplicate(m_pObject->m_selectedObjectSet.lock()->shared_from_this());
			KdMatrix mNewObj = newObj->GetMatrix();

			mNewObj.Move(0, 0, -2.0f);

			newObj->SetMatrix(mNewObj, true);
		}
		else
		{
			newObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			SPtr<Object> levelroot = GameMgr.m_level->GetRoot()->FindObject("LevelRoot");
			newObj->SetParent(levelroot);

			auto obj = m_pObject->m_selectedObjectSet.lock();
			KdMatrix mSelectedObj = obj->GetMatrix();
			KdMatrix mGeneratedObj = mSelectedObj;

			mGeneratedObj.Move(0, 0, -2.0f);

			newObj->SetMatrix(mGeneratedObj, true);
		}
		m_pObject->m_selectedObjectSet = newObj;
	}

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Edit::Update()
{

	return nullptr;
}

SPtr<LevelEditorScript::BaseLevelEditorAction> LevelEditorScript::Replace::Update()
{
	// 選択中のオブジェクトセットがない場合基本行動に
	if (m_pObject->m_selectedObjectSet.expired())return std::make_shared<Basic>();

	// 選択しているプレハブファイルがない場合
	if (m_pObject->m_generationPrefabName.empty())return std::make_shared<Selected>();

	// オーナー取得
	auto owner = m_pObject->GetOwner();
	if (owner == nullptr) return nullptr;

	// 入力コンポーネント取得
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr) return nullptr;

	// DELETEキーでオブジェクトセットを削除
	if (input->m_keys[VK_DELETE])
	{
		m_pObject->m_selectedObjectSet.lock()->Delete();
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択したオブジェクトセットを削除しました");
	}

	// 右クリックで選択解除
	if (input->m_keys[VK_RBUTTON])
	{
		m_pObject->m_selectedObjectSet.reset();
		GameMgr.m_Editor_Log.AddLog(u8"選択を解除しました");
		return std::make_shared<Basic>();
	}

	// Oキーのみなら選択中のオブジェクトのみ
	// SHIFTと一緒に押すと選択中のオブジェクトと同じ名前のオブジェクトも全て置換する
	if (input->m_keys['O'] && input->m_keysRepeat[VK_LSHIFT])
	{
		auto objects = GameMgr.m_level->GetRoot()->FindObjects(m_pObject->m_selectedObject.lock()->GetName());
		for (auto && obj : objects)
		{
			GameMgr.m_Editor_Log.AddLog(u8"オブジェクトを置換しました");
			auto parent = obj->GetParent();
			KdMatrix mObj = obj->GetMatrix();
			auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
			ReplaceObj->SetMatrix(mObj, true);
			obj->Delete();
			ReplaceObj->SetParent(parent);
		}
	}
	else if (input->m_keys['O'])
	{
		GameMgr.m_Editor_Log.AddLog(u8"オブジェクトを置換しました");
		auto selectedObj = m_pObject->m_selectedObject.lock();
		auto parent = selectedObj->GetParent();
		KdMatrix mObj = selectedObj->GetMatrix();
		auto ReplaceObj = GameMgr.Instantiate(KDResFactory.GetJson(m_pObject->m_generationPrefabName));
		ReplaceObj->SetMatrix(mObj, true);
		selectedObj->Delete();
		ReplaceObj->SetParent(parent);
	}

	return nullptr;
}

