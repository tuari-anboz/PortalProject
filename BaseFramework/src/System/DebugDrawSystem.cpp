#include "main.h"
#include "DebugDrawSystem.h"

void DebugDrawSystem::Update()
{
	// 3Dノード寿命処理
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ) {
		// 寿命
		(*it).lifespan--;
		if ((*it).lifespan <= 0) {
			it = m_Nodes.erase(it);
		}
		else {
			++it;
		}
	}

	// 2Dテキストノード寿命処理
	if (m_TextNodes.empty() == false) {

		ShMgr.m_KdSpriteSh.Begin();

		for (auto it = m_TextNodes.begin(); it != m_TextNodes.end(); ) {
			// 寿命
			(*it).lifespan--;
			if ((*it).lifespan <= 0) {
				it = m_TextNodes.erase(it);
			}
			else {
				++it;
			}
		}

		ShMgr.m_KdSpriteSh.End();
	}
}

void DebugDrawSystem::Draw()
{
	for (auto& node : m_Nodes) {
		node.proc();
	}

	for (auto& node : m_TextNodes) {
		node.proc();
	}
}

void DebugDrawSystem::Line(const KdVec3 & pos1, const KdVec3 & pos2, const KdVec4 & color, const KdMatrix& mat, int duration)
{
	Node node;

	node.proc = [pos1, pos2, color, m = mat]() {
		ShMgr.m_KdPrimSh.DrawLine(pos1, pos2, color, &m);
	};

	node.lifespan = duration;

	m_Nodes.push_back(node);
}

void DebugDrawSystem::SphereLine(const KdVec3 & pos, float radius, const KdVec4 & color, const KdMatrix& mat, int duration)
{
	Node node;

	node.proc = [pos, radius, color, m = mat]() {
		ShMgr.m_KdPrimSh.DrawLine_Sphere(pos, radius, color, &m);
	};

	node.lifespan = duration;

	m_Nodes.push_back(node);

}

void DebugDrawSystem::BoxLine(const KdVec3 & vLocalCenter, const KdVec3 & vHalfSize, const KdVec4& color, const KdMatrix& mat, int duration)
{
	Node node;

	node.proc = [vLocalCenter, vHalfSize, color, m = mat]() {
		ShMgr.m_KdPrimSh.DrawLine_Box(vLocalCenter, vHalfSize, color, &m);
	};

	node.lifespan = duration;

	m_Nodes.push_back(node);

}

void DebugDrawSystem::Text(const std::string & text, const KdVec3 & pos3D, const KdVec4 & color, int duration)
{
	Node node;

	node.proc = [text, pos3D, color]() {

		KdVec3 pos2D;
		ShMgr.Convert3Dto2D(pos2D, pos3D);

		if (pos2D.z > 0) {
			// 下地
			ShMgr.m_KdSpriteSh.DrawFont(0, text, pos2D.x + 1, pos2D.y + 1, &KdVec4(0, 0, 0, 1), 0);
			// 文字
			ShMgr.m_KdSpriteSh.DrawFont(0, text, pos2D.x, pos2D.y, &color, 0);
		}
	};

	node.lifespan = duration;

	m_TextNodes.push_back(node);
}
