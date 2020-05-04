#pragma once

#include "EditorCamera.h"

// コンポーネント系
#include "ComponentSystem/BaseComponent.h"
#include "ComponentSystem/InputComponent.h"
#include "ComponentSystem/ModelComponent.h"
#include "ComponentSystem/EffectModelComponent.h"
#include "ComponentSystem/SpriteComponent.h"
#include "ComponentSystem/CameraComponent.h"
#include "ComponentSystem/LightComponent.h"
#include "ComponentSystem/AudioComponent.h"
#include "ComponentSystem/PortalComponent.h"
#include "ComponentSystem/TitleScript.h"
#include "ComponentSystem/PlayerScript.h"
#include "ComponentSystem/FpsCameraComponent.h"
#include "ComponentSystem/LevelEditorScript.h"
#include "ComponentSystem/LoadLevelComponent.h"
#include "ComponentSystem/UIButtonComponent.h"
#include "ComponentSystem/RigidBodyComponent.h"
#include "ComponentSystem/RideButtonComponent.h"
#include "ComponentSystem/BoxGeneratorComponent.h"
#include "ComponentSystem/DoorComponent.h"
#include "ComponentSystem/ButtonPushComponent.h"
#include "ComponentSystem/FadeComponent.h"
#include "ComponentSystem/Collider/CollisionManager.h"

// ゲーム系コンポーネント
#include "../Game/Components/AutomaticDoorComponent.h"
#include "../Game/Components/PortalGunComponent.h"
#include "../Game/Components/BoxComponent.h"
#include "../Game/Components/GettingBoxComponent.h"

// オブジェクト基本クラス
class Object : public std::enable_shared_from_this<Object>
{
public:
	// 仮想デストラクタ
	virtual ~Object() {}

private:
	// 更新処理
	virtual void Update() {}
	// 描画前処理
	virtual void PreDraw() {}
	// 描画処理
	virtual void Draw() {}
	// 透明描画処理
	virtual void DrawTransparency() {};
	// シャドウマップ描画処理
	virtual void DrawShadowMap() {};
	//
	virtual void DrawEffect() {}
public:

	// 初期処理(初回のUpdateの直前で実行される)
	virtual void Start() {}

	// 更新処理
	void UpdateObject();
	// 描画前処理
	void PreDrawObject();
	// 描画処理
	void DrawObject();
	// 不透明描画処理
	void DrawTransparencyObject();
	// シャドウマップ描画処理
	void DrawShadowMapObject();
	// エフェクト描画処理
	void DrawEffectObject();
	// 2D描画関数
	void DrawSpriteObject()
	{
		// 全コンポーネントのEditorDrawを実行
		for (auto&& comp : m_components)
		{
			comp->DrawSprite();
		}

		// 子を処理
		for (auto&& child : m_childs)
		{
			child->DrawSpriteObject();
		}
	}

	// (Editor)ImGui処理
	virtual void ImGuiUpdate();

	// (Editor)エディター描画
	void EditorDrawObject() {
		// 全コンポーネントのEditorDrawを実行
		for (auto&& comp : m_components) {
			comp->EditorDraw();
		}
	}

	// クラス名を文字列で取得
	std::string GetObjectClassName() {
		return &typeid(*this).name()[6];
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonObj);

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJson) {
		// クラス名※tyieidは環境依存です！
		outJson["ClassName"] = GetObjectClassName();
		// 名前
		outJson["Name"] = m_name;
		// 行列
		outJson["Matrix"] = m_mWorld.Serialize();
		// 質量
		outJson["Mass"] = m_mass;
		// 有効
		outJson["Active"] = m_isActive;
		// 全コンポーネント
		json11::Json::array compAry;
		for (auto&& comp : m_components) {
			// コンポーネントをシリアライズ
			json11::Json::object serial;
			comp->Serialize(serial);
			// 配列へ追加
			compAry.push_back(serial);
		}
		outJson["Components"] = compAry;

		// 全ての子をシリアライズし、配列化する
		json11::Json::array childAry;
		for (auto&& child : m_childs)
		{
			// Objectをシリアライズ
			json11::Json::object serial;
			child->Serialize(serial);
			// 配列に追加
			childAry.push_back(serial);
		}
		outJson["Childs"] = childAry;
	}

	//===============================
	// コンポーネント関係
	//===============================
	// 指定した型のコンポーネントを検索
	// Tはこの関数を実行するときに指定する
	template<class T>
	SPtr<T> GetComponent() {
		// 全コンポーネントを検索
		for (auto&& comp : m_components) {
			// ダウンキャストでT型へキャストを試みる
			auto p = std::dynamic_pointer_cast<T>(comp);
			// キャストできたら、そいつで確定！！
			if (p)return p;
		}
		// 見つかんなかったら、nullを返す
		return nullptr;
	}

	// 指定した型のコンポーネントを検索
	// Tはこの関数を実行するときに指定する
	template<class T>
	std::vector<SPtr<T>> GetComponents() {
		std::vector<SPtr<T>> results;
		// 全コンポーネントを検索
		for (auto&& comp : m_components) {
			// ダウンキャストでT型へキャストを試みる
			auto p = std::dynamic_pointer_cast<T>(comp);
			// キャストできたら、そいつで確定！！
			if (p)results.push_back(p);
		}
		// 見つかんなかったら、nullを返す
		return std::move(results);
	}

	// コンポーネントを追加
	void AddComponent(const SPtr<BaseComponent>& comp)
	{
		comp->SetOwner(this);
		m_components.push_back(comp);
	}

	// コンポーネントリストを取得
	std::list<SPtr<BaseComponent>>& GetComponentList()
	{
		return m_components;
	}

	//===============================
	// 親と子
	//===============================
	// 子リスト取得
	const std::list < SPtr<Object>>& GetChilds()
	{
		return m_childs;
	}

	// 指定した子を削除(即座に消すため、呼び出し時は注意！)
	void RemoveChild(const SPtr<Object>& obj)
	{
		// 検索
		auto it = std::find(m_childs.begin(),m_childs.end(), obj);
		// 発見
		if (it != m_childs.end())
		{
			// 削除
			m_childs.erase(it);
		}
	}

	// 親を取得
	SPtr<Object> GetParent() const { return m_parent.lock(); }

	// 親を設定
	void SetParent(const SPtr<Object>& parent)
	{
		// 親から脱退する前に、自分のshared_ptrを記憶
		SPtr<Object> spThis = shared_from_this();

		// 現在の親GameObjectから脱退する
		auto nowParent = GetParent();
		if(nowParent)
		{
			nowParent->RemoveChild(spThis);
		}

		// 親ポインタを変更
		m_parent = parent;

		// 新しい親の子として、自分を登録
		if (parent)
		{
			parent->m_childs.push_back(spThis);
		}
	}

	// 一致する名前のObjectを検索(最初に見つかったやつのみ)
	SPtr<Object> FindObject(const std::string & name)
	{
		SPtr<Object> result;
		// 再帰して判定する関数
		std::function<bool(const SPtr<Object>&)> rec = [&rec, &result, &name](const SPtr<Object>& obj)
		{
			// 判定
			if (obj->m_name == name)
			{
				result = obj;
				return true;
			}

			// 子再帰
			for (auto&& child : obj->GetChilds())
			{
				if (rec(child))return true;
			}
			return false;
		};

		// Rootから検索していく
		rec(shared_from_this());

		return result;
	}
	
	// 一致する名前のObjectを「全て」検索
	std::vector<SPtr<Object>> FindObjects(const std::string& name)
	{
		std::vector<SPtr<Object>> results;
		// 再帰して判定する関数
		std::function<void(const SPtr<Object>&)> rec = [&rec, &results, &name](const SPtr<Object>& obj)
		{
			// 判定
			if (obj->m_name == name)
			{
				results.push_back(obj);
			}
			
			// 子再帰
			for (auto&& child : obj->GetChilds())
			{
				rec(child);
			}
		};

		// Root～検索していく
		rec(shared_from_this());

		return results;
	};

	// 1つ上へ
	void MoveUp()
	{
		auto parent = GetParent();
		if (parent)
		{
			auto it = std::find(parent->m_childs.begin(), parent->m_childs.end(), shared_from_this());
			if (it != parent->m_childs.end() && it != parent->m_childs.begin())
			{
				auto prev = it;
				prev--;
				std::swap(*it, *prev);
			}
		}
	}

	// 1つ下へ
	void MoveDown()
	{
		auto parent = GetParent();
		if (parent)
		{
			auto it = std::find(parent->m_childs.begin(), parent->m_childs.end(), shared_from_this());
			if (it != parent->m_childs.end())
			{
				auto next = it;
				next++;

				if (next != parent->m_childs.end())
				{
					std::swap(*it, *next);
				}
			}
		}
	}

	//===============================
	// その他 設定
	//===============================
	// 名前設定
	void SetName(const std::string& _name) { m_name = _name; }
	// 名前取得
	std::string GetName() { return m_name; }
	// 行列設定
	void SetMatrix(const KdMatrix& m, bool moveChild);
	// 行列取得
	const KdMatrix& GetMatrix() const { return m_mWorld; }
	// 変更前行列取得
	const KdMatrix& GetPrevMatrix() const { return m_mPrevWorld; }
	// 変化量行列取得
	void GetDeltaMatrix(KdMatrix& out)
	{
		KdMatrix mInv = m_mPrevWorld;
		mInv.Inverse();
		out = mInv * m_mWorld;
	}
	// 質量取得
	float GetMass() const { return m_mass; }
	// 有効On/Off
	void SetActive(bool active) { m_isActive = active; }
	// 有効か？
	bool IsActive() { return m_isActive; }
	// 削除する
	void Delete() { m_isDelete = true; }
	// 削除状態か？
	bool IsDelete() { return m_isDelete; }

protected:
private:	// 行列は継承先からも直接いじれないようにする！！

	// 名前
	std::string m_name = "No Name";
	// 行列
	KdMatrix	m_mWorld;
	KdMatrix	m_mPrevWorld;		// 変更前の行列

	// 質量 0:完全固定物 0より大きい:動的物
	float m_mass = 0;

	// コンポーネントリスト
	std::list< SPtr<BaseComponent > >	m_components;

	// 子リスト 
	std::list< SPtr<Object> > m_childs;
	// 親のポインタ 
	WPtr<Object>    m_parent; 

	// 親オブジェクトのボーン追従させるボーン名
	std::string m_followBoneName;

	// 削除フラグ
	bool	m_isDelete = false;

	// 初回？
	bool m_isStart = true;

	// 有効？
	bool m_isActive = true;
};

#include "Level.h"

//=============================
// ゲームの基盤となるクラス
//=============================
class GameManager {
public:
	// 初期設定
	void Init();

	// ゲーム処理
	void Run();

	// Object生成
	SPtr<Object> Instantiate(const json11::Json& jsonObj);
	// Object複製
	SPtr<Object> Duplicate(const SPtr<Object> obj);

	// レベル
	SPtr<Level>		m_level;

	// 物理ワールド
	SPtr<KdPhysicsWorld> m_physicsWorld;

	// フェードインフェードアウト(trueの時フェードイン済み)
	bool m_fade = false;

	// ロード中のフラグ(マルチスレッドでのロード画面表示用)
	bool m_isLoading = false;

	//------------------------------------------
	// Editor
	//------------------------------------------
	// (Editor)選択物セット
	void SetSelectObj(const SPtr<Object>& obj)
	{
		m_Editor_JustSelected = true;
		m_Editor_BeforeSelectObj = m_Editor_SelectObj;
		m_Editor_SelectObj = obj;
	}
	// (Editor)選択物ゲット
	WPtr<Object> GetSelectObj()
	{
		return m_Editor_SelectObj;
	}
	// (Editor)選択したばかりの時
	bool m_Editor_JustSelected;
	// (Editor)カメラ
	EditorCamera	m_Editor_Camera;
	// (Editor)カメラコンポーネントを使用する
	bool m_Editor_UseCameraComponent = false;
	// (Editor)実行中
	bool m_Editor_IsPlay = false;
	// (Editor)UI表示
	bool m_Editor_showUI = true;
	// (Editor)LevelEditor実行
	bool m_Editor_Enable_LevelEditor = false;
	// (Editor)ログウィンドウ
	ImGuiLogWindow m_Editor_Log;
	// (Editor)デバッグ表示
	bool m_Editor_showDebug = true;

	// オブジェクトクラスのリスト & 生成
	std::unordered_map< std::string, std::function<SPtr<Object>()>  >
		m_objectClassMaker;

	// コンポーネントクラスのリスト & 生成
	std::unordered_map< std::string, std::function<SPtr<BaseComponent>()>  >
		m_componentClassMaker;

	// Level切り替え(予約)
	void ChangeLevel(const std::string& levelFile)
	{
		m_changeLevelFile = levelFile;
	}

	// 波テクスチャを取得
	WaveTexture&	GetWaveTex() { return m_wave; }

private:
	// ImGui処理
	void ImGuiUpdate();

	// 変更レベルファイル名(予約用)
	std::string		m_changeLevelFile;

	//------------------------------------------
	// Editor
	//------------------------------------------
	// (Editor)選択物
	WPtr<Object>	m_Editor_SelectObj;
	// (Editor)前回の選択物
	WPtr<Object>	m_Editor_BeforeSelectObj;

	// (仮)波テクスチャ
	WaveTexture m_wave;

//=======================
// シングルトン
//=======================
private:
	GameManager() {}

public:
	static GameManager& GetInstance() {
		static GameManager instance;
		return instance;
	}
};

#define GameMgr GameManager::GetInstance()

// Objectクラス登録用マクロ
#define REGISTER_OBJECT_CLASS(className) \
GameMgr.m_objectClassMaker[#className] = []() { \
	return std::make_shared<className>(); \
};

// コンポーネントクラス登録用マクロ
#define REGISTER_COMP_CLASS(className) \
GameMgr.m_componentClassMaker[#className] = []() { \
	return std::make_shared<className>(); \
};
