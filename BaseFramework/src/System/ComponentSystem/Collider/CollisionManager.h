#pragma once

#include "BaseColliderComponent.h"

//===================================================
//
// �����蔻��Ǘ��N���X
// �E�R���C�_�[��o�^���A���݂�������s���A���ʂ�ʒm����(�ʒm��Atk�����̃R���C�_�[�̂�)�B
// �E�}���`�X���b�h�ł̕��񔻒���\�B
//
//===================================================
class CollisionManager {
public:

	//-------------------------------------------------------
	// ����I�u�W�F�N�g��o�^
	//-------------------------------------------------------
	void Register(const SPtr<BaseColliderComponent>& obj) {
		if (obj == nullptr)return;

		// �u���肷�鑤�v�֓o�^
		if (obj->IsAtk()) {
			m_atkList.push_back(obj);
		}
		// �u���肳��鑤�v�֓o�^
		else {
			m_defList.push_back(obj);
		}
	}

	//-------------------------------------------------------
	// �o�^����Ă��锻��I�u�W�F�N�g���X�g���N���A����
	//-------------------------------------------------------
	void ClearList() {
		for (auto&& atk : m_atkList)
		{
			atk->m_onHitEnter = nullptr;
			atk->m_onHitStay = nullptr;
			atk->m_onHitExit = nullptr;
		}
		m_atkList.clear();
		m_defList.clear();
	}

	//-------------------------------------------------------
	// ������s���ʒm
	//-------------------------------------------------------
	void Run();

	//-------------------------------------------------------
	// �f�o�b�O�`��
	// �E�o�^���Ă���R���C�_�[�����o�I�Ɋm�F�ł���悤�ɁA
	// �@�R���C�_�[�`���q�b�g������Ȃǂŕ`�悷��B
	//-------------------------------------------------------
	void DebugDraw() {
		KdVec4 color;

		color = { 1.0f, 0.1f, 0.1f, 0.6f };
		for (auto&& obj : m_atkList) {

			obj->DebugDraw(color);
		}

		color = { 0.1f, 0.1f, 1.0f, 0.3f };
		for (auto&& obj : m_defList) {

			obj->DebugDraw(color);
		}
	}

	//-------------------------------------------------------
	// ����ݒ�E�擾
	//-------------------------------------------------------
	// �}���`�X���b�h�ł̔��� On/Off
	void SetMultiThread(bool enable) {
		m_isMultiThread = enable;
	}

	// �}���`�X���b�h���삩�H
	bool IsMultiThread() const { return m_isMultiThread; }

	// �t�B���^�[���X�g�̂P���ڂ̃f�[�^
	struct FilterNode {
		std::string name;	// �t�B���^�[��
		int flag = 0;		// �r�b�g�t���O
		bool atk = false;	// ���肷�鑤
	};
	// �t�B���^�[���X�g�̐ݒ�
	void InitFilter(const std::vector<CollisionManager::FilterNode>& atkFilters,
					const std::vector<CollisionManager::FilterNode>& defFilters
	) {
		m_atkFilterList = atkFilters;
		m_defFilterList = defFilters;
	}
	// �t�B���^�[���X�g�擾
	const std::vector<CollisionManager::FilterNode>& GetAtkFilterList() const { return m_atkFilterList; }
	const std::vector<CollisionManager::FilterNode>& GetDefFilterList() const { return m_defFilterList; }

private:

	// atk vs m_defList �̔�������s
	void HitTestToDefList(BaseColliderComponent* atk);

	// ���肷�鑤 ���X�g
	std::vector<SPtr<BaseColliderComponent>>	m_atkList;

	// ���肳��鑤 ���X�g
	std::vector<SPtr<BaseColliderComponent>>	m_defList;

	// Run�֐��̂����蔻����}���`�X���b�h�ōs��
	bool m_isMultiThread = true;

	// �t�B���^�[���X�g
	std::vector<CollisionManager::FilterNode>	m_atkFilterList;
	std::vector<CollisionManager::FilterNode>	m_defFilterList;

//=======================================
// �V���O���g��
//=======================================
private:
	CollisionManager() {
		InitFilter(
			{
				{u8"�Ԃ���",	0x00000001},
				{u8"�U��",		0x00000002},
			},
			{
				{u8"�w�i��",	0x00010000},
				{u8"����",		0x00020000},
				{u8"�G",		0x00040000},
				{u8"�v���C���[",0x00080000},
				{u8"�|�[�^��",	0x00100000},
				{u8"�T�[�`"	   ,0x00200000},
				{u8"�G�f�B�^�[",0x00400000},
				{u8"�{�^��",    0x00800000},
				{u8"�{�b�N�X"  ,0x01000000},
			}
		);
	}
public:
	static CollisionManager& GetInstance() {
		static CollisionManager instance;
		return instance;
	}
};

#define ColMgr CollisionManager::GetInstance()
