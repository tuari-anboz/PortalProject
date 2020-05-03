#pragma once

#include "BaseComponent.h"

namespace PhiysicsCollide
{
}

// RigidBodyComponent
// 物理挙動の剛体を生成したりする
// 
// 
class RigidBodyComponent : public BaseComponent
{
public:
	virtual void Start()override;
	virtual void Update()override;
	// ImGui処理
	virtual void ImGuiUpdate() override;

	//
	void UpdateRigidBody();

	// rigidbody取得
	SPtr<KdPhysicsRigidBody>& GetRigidBody() { return m_body; }

	// 剛体に力を加える
	void SetLinearVelocity(const KdVec3& vec)
	{
		m_body->SetLinearVelocity(vec);
	}

	// 剛体の判定グループを変更する
	// update : 変更した後剛体を再度物理ワールドに追加するフラグ
	void SetCollisionGroup(const int group, bool update = false);

	// 剛体はコンポーネントの有効/無効を切り替えた時に物理ワールドから削除する
	void SetEnable(bool enable)override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------
	// JSONデータから、初期設定を行う
	virtual void Deserialize(const json11::Json& jsonObj) override;
	// JSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) override;
private:
	// 剛体
	SPtr<KdPhysicsRigidBody> m_body = nullptr;

	// キネマティック剛体On/Off
	bool m_isKinematic = false;
	// 質量
	float m_mass = 0;
	// 摩擦力
	float m_friction = 0;
	// 反発力
	float m_restitution = 0;
	// 判定グループ
	int m_group = 1;
	// 判定マスク
	int m_mask = 0xFFFFFFFF;
	// ノンスリープ
	bool m_NonSleep = false;
};