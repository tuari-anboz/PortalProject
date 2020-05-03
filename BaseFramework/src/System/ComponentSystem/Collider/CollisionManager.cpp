#include "main.h"

#include "../../GameManager.h"
#include "BaseColliderComponent.h"
#include "CollisionManager.h"

// �q�b�g��ԃt���O
namespace HitStates {
	enum {
		ENTER = 0x00000001,	// HIT����1���
		STAY = 0x00000002,	// HIT��������
		EXIT = 0x00000004,	// HIT���Ȃ��Ȃ���1���
	};
}

void CollisionManager::Run()
{
	//======================================================
	//
	// �SAtk���X�g����A�SDef���X�g�֔�����s��
	//
	//======================================================

	//++++++++++++++++++++++++++
	// �V���O���X���b�h�ł̔���
	//++++++++++++++++++++++++++
	if(m_isMultiThread == false){

		//=================================
		// ������s
		//=================================
		for (auto&& atk : m_atkList) {

			HitTestToDefList(atk.get());

		}

	}
	//++++++++++++++++++++++++++
	// �}���`�X���b�h�ŕ��񔻒���s
	// Atk�^�C�v���X���b�h�œ������s����ADef�^�C�v���X�g�Ɣ��肷��
	//++++++++++++++++++++++++++
	else {

		//=================================
		// �}���`�X���b�h�ŕ��񔻒���s
		// �U�^�C�v���X���b�h�œ��삵�A�h�^�C�v���X�g�Ɣ��肷��
		//=================================
		struct ThreadData {
			int listNum = 0;
			std::atomic<int> nowIdx = 0;	// �J�E���^(�A�g�~�b�N�ϐ�)
		};
		ThreadData thData;

		// �P�X���b�h�ł̏����֐�(Atk���̏���)
		auto execThread = [this, &thData]() {
			// ���X�g�̍Ō�܂Ń��[�v����������
			while (1) {
				// nowIdx��1���Z���A���Z�u�O�v�̒l��Ԃ� atomic�^�̕ϐ��͕����X���b�h���瓯�����삵�Ă����S
				int idx = thData.nowIdx.fetch_add(1);
				if (idx >= thData.listNum)break;

				// atkObj vs �Sm_defList
				HitTestToDefList(m_atkList[idx].get());
			}
		};

		// CPU�� �擾
		SYSTEM_INFO sys;
		GetSystemInfo(&sys);
		int cpuCnt = (int)sys.dwNumberOfProcessors;

		std::vector<std::future<void>> fut(cpuCnt);

		// �X���b�h�f�[�^�ݒ�
		thData.nowIdx = 0;
		thData.listNum = m_atkList.size();

		// CPU���Ԃ�X���b�h���N�����A����Ŕ�������s����
		for (DWORD i = 0; i < fut.size(); i++) {
			fut[i] = std::async(std::launch::async | std::launch::deferred, execThread);
		}
		// �S�X���b�h�̏I���ҋ@
		for (DWORD i = 0; i < fut.size(); i++) {
			fut[i].wait();
		}

	}



	//======================================================
	//
	// ���ʒʒm�֐����s
	//
	//======================================================
	for (auto&& atk : m_atkList) {

		// ����q�b�g��
		if (atk->m_hitState & HitStates::ENTER) {
			if (atk->m_onHitEnter)atk->m_onHitEnter(atk.get());
		}

		// �q�b�g��������
		if (atk->m_hitState & HitStates::STAY) {
			if (atk->m_onHitStay)atk->m_onHitStay(atk.get());
		}

		// �q�b�g���Ȃ��Ȃ����ŏ��̈��ڂ̂�
		if (atk->m_hitState & HitStates::EXIT) {
			if (atk->m_onHitExit)atk->m_onHitExit(atk.get());
		}

	}
}

void CollisionManager::HitTestToDefList(BaseColliderComponent * atk)
{
	if (atk->IsAtk() == false)return;

	// ���茋�ʃf�[�^���N���A����
	atk->m_results.clear();

	// Def���Ɣ���
	for (auto&& def : m_defList) {

		//------------------------
		// ���O�`�F�b�N
		//------------------------

		// ���� vs ���� �t�B���^����
		if ( (atk->m_groupFilter & def->m_maskFilter) == 0 ||
			 (atk->m_maskFilter & def->m_groupFilter) == 0
		) {
			continue;
		}

		// ����������͖���
		if (atk->GetOwner() == def->GetOwner())continue;

		// �������X�g�ɓo�^����Ă���GameObject�͖���
//		if (atk->m_ignoreList.count(def->GetGameObjectRaw()) >= 1)continue;
		if (atk->m_ignoreMap.count(def->GetOwner()) >= 1)continue;

		//------------------------
		// ����
		//------------------------
		atk->HitTest(def.get());
	}

	//-------------------------
	// �q�b�g��ԃt���O����
	//-------------------------

	// ����HIT�t���O��HIT�I���t���O��OFF
	atk->m_hitState &= ~HitStates::ENTER;	// OFF
	atk->m_hitState &= ~HitStates::EXIT;	// OFF

	// �q�b�g���Ă���
	if (atk->m_results.size() > 0) {
		if (!(atk->m_hitState & HitStates::STAY)) {	// �O��HIT���ł͂Ȃ�
			atk->m_hitState |= HitStates::ENTER;	// ����HIT ON
			atk->m_hitState |= HitStates::STAY;		// HIT�� ON
		}
	}
	// �q�b�g���Ă��Ȃ�
	else {
		// HIT���Ȃ�HIT���t���OOFF���āAHIT�I���t���OON
		if (atk->m_hitState & HitStates::STAY) {
			atk->m_hitState |= HitStates::EXIT;		// ON
			atk->m_hitState &= ~HitStates::STAY;	// OFF
		}
	}
}
