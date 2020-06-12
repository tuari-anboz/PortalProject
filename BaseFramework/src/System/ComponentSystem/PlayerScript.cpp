#include "main.h"

// システム系
#include "../GameManager.h"

// ゲーム系
#include "PlayerScript.h"

//=================================

void PlayerScript::Start()
{
	// オーナー取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// 初期行動
	m_as = std::make_shared<Stand>();
}

void PlayerScript::Update()
{
	// オーナー取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	// モデル取得
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return;

	// 入力取得
	auto input = owner->GetComponent<InputComponent>();
	if (input == nullptr)return;

	// ヒットストップ中でないとき
	if (m_hitStop <= 0) {

		// アニメータをすすめる
		if (model) {
			model->GetAnimator().GetTrack()[0]->m_Enable = true;
		}

		// 行動処理
		if (m_as) {
			// 持ち主のキャラのアドレスを入れておく
			m_as->m_pChara = this;
			// 行動処理実行
			auto p = m_as->Update();
			if (p) {
				m_as = p;
			}
		}

		// [特殊]部分行動処理
		if (m_asExt) {
			m_asExt->m_pChara = this;
			m_asExt = m_asExt->Update();

		}
	}
	// ヒットストップ中
	else {
		m_hitStop--;

		// アニメータを止める
		if (model) {
			// アニメーショントラック[0]を無効にする
			// ※トラック … 現在のアニメーションの進行データ
			model->GetAnimator().GetTrack()[0]->m_Enable = false;
		}
	}

	// Eキーでボックスを取ったり放ったりする
	if (input->m_keys['E'])
	{
		auto cam = owner->FindObject("Cam");
		if (cam)
		{
			auto getbox = cam->GetComponent<GettingBoxComponent>();
			if (getbox)
			{
				getbox->GetBox();
			}
		}
	}

	// 当たり判定
	{
		SetIsPortalCollide() = false;
		// 2つのポータルの存在チェック
		auto portal1 = GameMgr.m_level->GetRoot()->FindObject("Portal1");
		auto portal2 = GameMgr.m_level->GetRoot()->FindObject("Portal2");

		auto colliders = owner->GetComponents<BaseColliderComponent>();
		for (auto&& coll : colliders)
		{
			// サーチ
			if (coll->GetTag() == "Search" && portal1 && portal2)
			{
				// この球がヒットした時に実行される関数を入れておく
				coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
				{
					for (auto&& res : collider->GetResults())
					{
						if (res.Collider->GetTag() == "EnterPortal")
						{
							SetIsPortalCollide() = true;
							// コライダーコンポーネントを「全て」取得する
							auto colliders = owner->GetComponents<BaseColliderComponent>();
							for (auto&& coll : colliders)
							{
								// ぶつかり系の判定
								if (coll->GetTag() == "Collision")
								{
									// この球がヒットした時に実行される関数を入れておく
									coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
									{
										// ヒットしたやつら(Collider)全員
										for (auto&& res : collider->GetResults())
										{
											if (res.Collider->GetTag() == "Portal")
											{
												// 質量比計算
												float myMass = owner->GetMass();	// 自分の質量
												float youMass = res.Collider->GetOwner()->GetMass();// 相手の質量
												float massPower = 0;
												// 自分の質量が0なら、自分は不動
												if (myMass == 0) {
													massPower = 0;
												}
												// 相手の質量が0なら、自分は動く
												else if (youMass == 0) {
													massPower = 1.0f;
												}
												else {
													// 両方質量あり　比率計算(0.0～1.0)
													massPower = youMass / (myMass + youMass);
												}

												KdMatrix m = owner->GetMatrix();
												m.Move(res.Push * massPower);	// 押し戻す
												owner->SetMatrix(m, false);
											}
										}
									};
								}
							}
							// 地面判定
							{
								// レイ判定のコンポーネントを取得する
								auto coll = owner->GetComponent<RayColliderComponent>();
								if (coll) {
									float rayY = coll->GetRayLocalPos().y * owner->GetMatrix().GetYScale() + 1.0f;
									// ヒット時の実行される関数を登録
									coll->m_onHitStay = [this, rayY, owner](BaseColliderComponent* collider) {
										// 全てのあたった物から、一番近いやつを検出
										float nearest = FLT_MAX;
										const CollisionResult* nearestRes = nullptr;
										for (auto&& res : collider->GetResults()) {
											if (res.Collider->GetTag() != "EnterPortal")
											{
												// より近いものがあれば、それを
												if (nearest > res.Dist) {
													nearestRes = &res; 
													nearest = res.Dist;
												}
											}
										}
										// 一番近いやつ
										if (nearestRes) {

											KdMatrix m = owner->GetMatrix();

											// 空中時
											if (m_isSky) {
												if (nearestRes->Collider->GetTag() != "Portal")
												{
													// めり込んでいる
													if (nearestRes->Dist < rayY && m_vForce.y < 0)
													{
														m_isSky = false;
														// めり込んだぶん、移動させる
														m.Move(0, rayY - nearestRes->Dist, 0);
														// 重力リセット
														m_vForce.y = 0;
													}
												}
											}
											// 地上時
											else {
												// 一定以上地面から離れると、空中と判断する
												if (nearestRes->Dist >= rayY + 0.1f || m_vForce.y > 0 || nearestRes->Collider->GetTag() == "Portal" || nearestRes->Collider->GetTag() == "EnterPortal") {
													// 空中に
													m_isSky = true;
												}
												else {
													// めり込んだぶん、移動させる
													m.Move(0, rayY - nearestRes->Dist, 0);
													// 重力リセット
													m_vForce.y = 0;

													//-------------------------------
													// 地面の変化量ぶん、移動させる
													//-------------------------------
													// 地面のやつの変化量を取得
													KdMatrix mDelta;
													nearestRes->Collider->GetOwner()->GetDeltaMatrix(mDelta);
													mDelta.NormalizeScale();	// 拡大は無視する
													m *= mDelta;
													// Z方向は、XZ平面に水平にする
													KdVec3 vZ = m.GetZAxis();
													vZ.y = 0;
													m.LookTo(vZ, { 0,1,0 });
												}
											}
											// 行列セット
											owner->SetMatrix(m, false);
										}
									};
								}
							}
						}
					}
				};
			}
		}
		// ポータルに当たってない場合
		for (auto&& coll : colliders)
		{
			// ぶつかり系の判定
			if (coll->GetTag() == "Collision")
			{
				// この球がヒットした時に実行される関数を入れておく
				coll->m_onHitStay = [this, owner](BaseColliderComponent* collider)
				{
					if (SetIsPortalCollide() == false)
					{
						for (auto&& res : collider->GetResults())
						{
							if (res.Collider->GetTag() == "Collision")
							{
								// 質量比計算
								float myMass = owner->GetMass();	// 自分の質量
								float youMass = res.Collider->GetOwner()->GetMass();// 相手の質量
								float massPower = 0;
								// 自分の質量が0なら、自分は不動
								if (myMass == 0) {
									massPower = 0;
								}
								// 相手の質量が0なら、自分は動く
								else if (youMass == 0) {
									massPower = 1.0f;
								}
								else {
									// 両方質量あり　比率計算(0.0～1.0)
									massPower = youMass / (myMass + youMass);
								}

								KdMatrix m = owner->GetMatrix();
								m.Move(res.Push * massPower);	// 押し戻す
								owner->SetMatrix(m, false);
							}
						}
					}
				};
			}
		}
		// 地面判定
		{
			// レイ判定のコンポーネントを取得する
			auto coll = owner->GetComponent<RayColliderComponent>();
			if (coll) {
				float rayY = coll->GetRayLocalPos().y * owner->GetMatrix().GetYScale() + 1.0f;
				// ヒット時の実行される関数を登録
				coll->m_onHitStay = [this, rayY, owner](BaseColliderComponent* collider)
				{
					if (SetIsPortalCollide() == false)
					{
						// 全てのあたった物から、一番近いやつを検出
						float nearest = FLT_MAX;
						const CollisionResult* nearestRes = nullptr;
						for (auto&& res : collider->GetResults()) {
							if (res.Collider->GetTag() != "EnterPortal" && res.Collider->GetTag() != "Portal")
							{
								// より近いものがあれば、それを
								if (nearest > res.Dist) {
									nearestRes = &res; nearest = res.Dist;
								}
							}
						}
						// 一番近いやつ
						if (nearestRes)
						{
							KdMatrix m = owner->GetMatrix();

							// 空中時
							if (m_isSky) {
								// めり込んでいる
								if (nearestRes->Dist < rayY && m_vForce.y < 0)
								{
									m_isSky = false;
									// めり込んだぶん、移動させる
									m.Move(0, rayY - nearestRes->Dist, 0);
									// 重力リセット
									m_vForce.y = 0;
								}
							}
							// 地上時
							else {
								// 一定以上地面から離れると、空中と判断する
								if (nearestRes->Dist >= rayY + 0.1f || m_vForce.y > 0) {
									// 空中に
									m_isSky = true;
								}
								else {
									// めり込んだぶん、移動させる
									m.Move(0, rayY - nearestRes->Dist, 0);
									// 重力リセット
									m_vForce.y = 0;

									//-------------------------------
									// 地面の変化量ぶん、移動させる
									//-------------------------------
									// 地面のやつの変化量を取得
									KdMatrix mDelta;
									nearestRes->Collider->GetOwner()->GetDeltaMatrix(mDelta);
									mDelta.NormalizeScale();	// 拡大は無視する
									m *= mDelta;
									// Z方向は、XZ平面に水平にする
									KdVec3 vZ = m.GetZAxis();
									vZ.y = 0;
									m.LookTo(vZ, { 0,1,0 });
								}
							}
							// 行列セット
							owner->SetMatrix(m, false);
						}
					}
				};
			}
		}
	}
}

void PlayerScript::DrawEffect()
{

}

void PlayerScript::ImGuiUpdate()
{
	// ObjectのほうのImGuiUpdateを実行
	BaseComponent::ImGuiUpdate();

	// オーナー取得
	auto owner = GetOwner();
	if (owner == nullptr)return;

	if (ImGui::CollapsingHeader(owner->GetObjectClassName().c_str(),
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Hp
		ImGui::InputInt("Hp", &m_hp);

		// 移動速度
		ImGui::InputFloat(u8"移動速度", &m_ms);

		// プレハブファイル
		ImGuiResourceButton("Prefab File", m_editorPrefabName, { ".pref" });
	}
}


SPtr<PlayerScript::BaseAction> PlayerScript::Stand::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// InputComponentを検索する
	auto input = owner->GetComponent<InputComponent>();
	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	if (input)
	{
		// 方向キー押している
		if (input->m_keysRepeat['W'] || input->m_keysRepeat['S'] || input->m_keysRepeat['A'] || input->m_keysRepeat['D']) {
			// アニメ変更
			//model->GetAnimator().ChangeAnimeSmooth("走り", true, 0.05f);
			// 行動切り替え
			return std::make_shared<Walk>();
		}

		// ジャンプ
		if (input->m_keysRepeat[VK_SPACE]) {
			// 行動切り替え
			auto as = std::make_shared<JumpStart>();
			as->m_vForce.Set(0, 0.075f, 0);

			return as;
		}
		// Xキーでポータルの片方を発射
		if (input->m_keys['X'])
		{

			// 行動切替
			return std::make_shared<Shot>();
		}
		// Cキーでポータルのもう片方を発射
		if (input->m_keys['C'])
		{
			// 行動切替
			return std::make_shared<Shot2>();
		}
	}

	// 空中？
	if (m_pChara->m_isSky) {
		// アニメ変更
		model->GetAnimator().ChangeAnimeSmooth("落下", false, 0.1f);
		// 行動切り替え
		return std::make_shared<JumpDown>();
	}

	// 摩擦
	m_pChara->m_vForce *= 0.9f;

	// 重力
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// 力移動
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Walk::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// InputComponentを検索する
	SPtr<InputComponent> input = owner->GetComponent<InputComponent>();
	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;



	KdVec3 vDir;
	// 子からカメラオブジェクトを検索・取得
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//カメラのZ方向
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y成分は潰す
		camZ.Normalize();	// 正規化

		// カメラのX方向
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y成分は潰す
		camX.Normalize();	// 正規化

		// 方向キーをもとに、移動方向を決定
		if(input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

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
			//m_pChara->m_vForce += vDir * 0.05f;
			owner->SetMatrix(m, false);
		}
	}

	if (input) {
		// 方向キー離している
		if (input->m_keysRepeat['W'] == 0 && input->m_keysRepeat['S'] == 0 && input->m_keysRepeat['A'] == 0 && input->m_keysRepeat['D'] == 0) {
			// アニメ変更
			//model->GetAnimator().ChangeAnimeSmooth("立ち", true, 0.05f);
			// 行動切り替え
			return std::make_shared<Stand>();
		}
		// ジャンプ
		if (input->m_keysRepeat[VK_SPACE]) {
			// アニメ変更
			//model->GetAnimator().ChangeAnimeSmooth("ジャンプ開始", false, 1.0f);
			// 行動切り替え
			auto as = std::make_shared<JumpStart>();
			as->m_vForce.Set(0, 0.075f, 0);

			return as;
		}
		// Xキー
		if (input->m_keysRepeat['X'])
		{

			// 行動切替
			return std::make_shared<Shot>();
		}
		// Cキー
		if (input->m_keysRepeat['C'])
		{
			// 行動切替
			return std::make_shared<Shot2>();
		}
	}

	// 空中？
	if (m_pChara->m_isSky) {
		// アニメ変更
		//model->GetAnimator().ChangeAnimeSmooth("落下", false, 0.1f);
		// 行動切り替え
		return std::make_shared<JumpDown>();
	}

	// 摩擦
	m_pChara->m_vForce *= 0.9f;

	// 重力
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// 力移動
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);


	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Shot::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam)
	{
		// ポータル展開
		// レイ判定のコンポーネントを取得する
		auto coll = cam->GetComponent<RayColliderComponent>();
		if (coll) {
			coll->SetEnable(true);
			// ヒット時の実行される関数を登録
			coll->m_onHitStay = [this, rayPos = coll->GetRayPos(), camObj = cam](BaseColliderComponent* collider)
			{
				// 全てのあたった物から、一番近いやつを検出
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults()) {
					if (res.Collider->GetTag() != "Portal")
					{
						// より近いものがあれば、それを
						if (nearest > res.Dist) {
							nearestRes = &res;
							nearest = res.Dist;
						}
					}
				}
				// 
				if (nearestRes) {
					// 一番近い物がポータルが設置可能な場合はポータルを設置し、そうでない場合はエフェクトを変更する
					if (nearestRes->Collider->GetTag() == "SetPortal")
					{
						auto portal = camObj->GetParent()->GetComponent<UsePortalComponent>();
						if (portal)
						{
							// 当たったオブジェクトのモデルコンポーネントを取得
							auto modelcomp = nearestRes->Collider->GetOwner()->GetComponent<ModelComponent>();
							if (!modelcomp) return;
							// モデルコンポーネントからモデルを取得
							auto model = modelcomp->GetModel();
							if (!model) return;
							// モデルの3Dメッシュ配列を取得
							auto models = model->GetModels();
							// 3Dメッシュ配列から対象のメッシュを取得
							auto mesh = models[nearestRes->MeshIdx];
							if (!mesh) return;
							// メッシュの法線を取得
							auto meshNormal = mesh->GetMesh()->GetExtFaceArray()[nearestRes->FaceIdx].vN;
							if (!meshNormal) return;
							// メッシュの法線を行列で変換
							meshNormal.TransformNormal(nearestRes->Collider->GetOwner()->GetMatrix());
							// ポータルを設置
							portal->Set(camObj->GetMatrix().GetPos(), nearestRes->Pos, meshNormal, 1);
						}
					}
					else
					{
						// 弾着エフェクトを生成
						auto obj = GameMgr.Instantiate(KDResFactory.GetJson("data/Texture/Effect/FlashEffect.pref"));
						if (obj)
						{
							KdMatrix m = obj->GetMatrix();
							m.SetPos(nearestRes->Pos);
							obj->SetMatrix(m, false);
							obj->SetParent(GameMgr.m_level->GetRoot());
						}
					}
				}
				collider->SetEnable(false);
			};
		}
	}


	return std::make_shared<Stand>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::Shot2::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam)
	{
		// ポータル展開
		// レイ判定のコンポーネントを取得する
		auto coll = cam->GetComponent<RayColliderComponent>();
		if (coll)
		{
			coll->SetEnable(true);
			// ヒット時の実行される関数を登録
			coll->m_onHitStay = [this, rayY = coll->GetRayLocalPos().y, coll, camObj = cam](BaseColliderComponent* collider)
			{
				// 全てのあたった物から、一番近いやつを検出
				float nearest = FLT_MAX;
				const CollisionResult* nearestRes = nullptr;
				for (auto&& res : collider->GetResults())
				{
					if (res.Collider->GetTag() != "Portal")
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
					if (nearestRes->Collider->GetTag() == "SetPortal")
					{
						auto portal = camObj->GetParent()->GetComponent<UsePortalComponent>();
						if (portal)
						{
							// 当たったオブジェクトのモデルコンポーネントを取得
							auto modelcomp = nearestRes->Collider->GetOwner()->GetComponent<ModelComponent>();
							if (!modelcomp) return;
							// モデルコンポーネントからモデルを取得
							auto model = modelcomp->GetModel();
							if (!model) return;
							// モデルの3Dメッシュ配列を取得
							auto models = model->GetModels();
							// 3Dメッシュ配列から対象のメッシュを取得
							auto mesh = models[nearestRes->MeshIdx];
							if (!mesh) return;
							// メッシュの法線を取得
							auto meshNormal = mesh->GetMesh()->GetExtFaceArray()[nearestRes->FaceIdx].vN;
							if (!meshNormal) return;
							// メッシュの法線を行列で変換
							meshNormal.TransformNormal(nearestRes->Collider->GetOwner()->GetMatrix());
							// ポータルを設置
							portal->Set(camObj->GetMatrix().GetPos(), nearestRes->Pos, meshNormal, 2);
						}
					}
				}
				collider->SetEnable(false);
			};
		}
	}


	return std::make_shared<Stand>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::GetBox::Update()
{
	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Generic::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	// アニメ終了判定
	if (model->GetAnimator().IsAnimationEnd()) {
		// アニメ変更
		model->GetAnimator().ChangeAnimeSmooth("立ち", true, 0.05f);
		// 行動切り替え
		return std::make_shared<Stand>();
	}

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Die::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	// アニメ終了判定
	if (model->GetAnimator().IsAnimationEnd()) {
		m_cnt++;

		auto m = owner->GetMatrix();
		m.Move(0, 0.1f, 0);
		owner->SetMatrix(m, false);

		if (m_cnt > 60) {
			// キャラを消す
			m_pChara->Delete();
		}
	}

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpStart::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponentを取得する
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	//// アニメ終了判定
	//if (model->GetAnimator().IsAnimationEnd()) {

	//	// アニメ変更
	//	model->GetAnimator().ChangeAnimeSmooth("上昇", false, 1);
	//	// 行動切り替え
	//	return std::make_shared<JumpUp>();
	//}

	KdVec3 vDir;
	// 子からカメラオブジェクトを検索・取得
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//カメラのZ方向
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y成分は潰す
		camZ.Normalize();	// 正規化

		// カメラのX方向
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y成分は潰す
		camX.Normalize();	// 正規化

		// 方向キーをもとに、移動方向を決定
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

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

	// 上に力を加える
	m_pChara->m_vForce = m_vForce;

	// 行動切り替え
	return std::make_shared<JumpUp>();

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpUp::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponentを取得する
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	// アニメ終了
	//if (model->GetAnimator().IsAnimationEnd()) {
	//	// アニメ変更
	//	model->GetAnimator().ChangeAnimeSmooth("落下", false, 0.1f);
	//	// 行動切り替え
	//	return std::make_shared<JumpDown>();
	//}

	KdVec3 vDir;
	// 子からカメラオブジェクトを検索・取得
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//カメラのZ方向
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y成分は潰す
		camZ.Normalize();	// 正規化

		// カメラのX方向
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y成分は潰す
		camX.Normalize();	// 正規化

		// 方向キーをもとに、移動方向を決定
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

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

	// 重力
	m_pChara->m_vForce.y -= 9.81f * 0.016f * 0.016f;

	// 力移動
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	// 行動切り替え
	return std::make_shared<JumpDown>();
}

SPtr<PlayerScript::BaseAction> PlayerScript::JumpDown::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを取得する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;
	// BasicInputComponentを取得する
	auto input = owner->GetComponent<BasicInputComponent>();
	if (input == nullptr)return nullptr;

	KdVec3 vDir;
	// 子からカメラオブジェクトを検索・取得
	SPtr<Object> cam = owner->FindObject("Cam");
	if (cam) {
		//カメラのZ方向
		KdVec3 camZ = cam->GetMatrix().GetZAxis();
		camZ.y = 0;			// Y成分は潰す
		camZ.Normalize();	// 正規化

		// カメラのX方向
		KdVec3 camX = cam->GetMatrix().GetXAxis();
		camX.y = 0;			// Y成分は潰す
		camX.Normalize();	// 正規化

		// 方向キーをもとに、移動方向を決定
		if (input->m_keysRepeat['W'])
		{
			vDir += camZ * m_pChara->m_ms;
		}
		if (input->m_keysRepeat['S'])
		{
			vDir += camZ * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['A'])
		{
			vDir += camX * -m_pChara->m_ms;
		}
		if (input->m_keysRepeat['D'])
		{
			vDir += camX * m_pChara->m_ms;
		}

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

	// 地上？
	if (m_pChara->m_isSky == false) {
		// アニメ変更
		//model->GetAnimator().ChangeAnimeSmooth("着地", false, 1);
		// 行動切り替え
		return std::make_shared<Landing>();
	}

	// 重力
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// 力移動
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}

SPtr<PlayerScript::BaseAction> PlayerScript::Landing::Update()
{
	// オーナー取得
	auto owner = m_pChara->GetOwner();

	// ModelComponentを検索する
	auto model = owner->GetComponent<ModelComponent>();
	if (model == nullptr)return nullptr;

	//// アニメ終了
	//if (model->GetAnimator().IsAnimationEnd()) {
	//	// アニメ変更
	//	model->GetAnimator().ChangeAnimeSmooth("立ち", true, 0.05f);
	//	// 行動切り替え
	//	return std::make_shared<Stand>();
	//}

	// 行動切り替え
	return std::make_shared<Stand>();

	// 重力
	m_pChara->m_vForce.y -= 9.8f * 0.016f * 0.016f;

	// 力移動
	KdMatrix m = owner->GetMatrix();
	m.Move(m_pChara->m_vForce);
	owner->SetMatrix(m, false);

	return nullptr;
}