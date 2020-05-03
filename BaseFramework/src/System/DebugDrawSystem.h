#pragma once

//================================================
//
// デバッグ描画システム
//
//================================================
class DebugDrawSystem {
public:

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 線描画
	// ・pos1		: 開始座標
	// ・pos2		: 終了座標
	// ・color		: 色
	// ・mat		: 変換行列
	// ・duration	: 表示時間
	void Line(const KdVec3& pos1, const KdVec3& pos2, const KdVec4& color, const KdMatrix& mat, int duration);

	// 球の線描画
	// ・pos		: 座標
	// ・radius		: 半径
	// ・color		: 色
	// ・mat		: 変換行列
	// ・duration	: 表示時間
	void SphereLine(const KdVec3& pos, float radius, const KdVec4& color, const KdMatrix& mat, int duration);

	// 箱の線描画
	// ・vLocalCenter	: 座標(ローカル)
	// ・vHalfSize		: サイズ(ローカル)
	// ・color			: 色
	// ・mat			: 変換行列
	// ・duration		: 表示時間
	void BoxLine(const KdVec3& vLocalCenter, const KdVec3& vHalfSize, const KdVec4& color, const KdMatrix& mat, int duration);

	// 2Dテキストを表示(3D座標から2Dへ変換します)
	// ・text			: 表示文字列
	// ・pos3D			: 3D座標(2Dへ変換される)
	// ・color			: 表示色
	// ・duration		: 表示時間
	void Text(const std::string& text, const KdVec3& pos3D, const KdVec4& color, int duration);

private:

	struct Node {
		std::function<void()>	proc;
		int						lifespan = 1;
	};

	std::vector< Node >	m_Nodes;

	std::vector< Node >	m_TextNodes;

//============================================================
// シングルトン
//============================================================
private:
	DebugDrawSystem() {
	}
public:
	static DebugDrawSystem &GetInstance(){
		static DebugDrawSystem Instance;
		return Instance;
	}


};

#define DebugDrawSys DebugDrawSystem::GetInstance()

