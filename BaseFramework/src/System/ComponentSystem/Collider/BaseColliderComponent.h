#pragma once

#include "../BaseComponent.h"

class Object;
class BaseColliderComponent;

//================================
// ���ʃf�[�^
//================================
struct CollisionResult {
	SPtr<BaseColliderComponent>	Collider;	// �q�b�g����̃R���C�_�[�R���|�[�l���g
	KdVec3 Pos;								// �q�b�g�ʒu(�����݁A�ꕔ�̌`��ł̂ݐ��m�Ȓl�ɂȂ��Ă��܂�)
	KdVec3 Push;							// �����x�N�g��(�����݁A�ꕔ�̌`��ł̂ݐ��m�Ȓl�ɂȂ��Ă��܂�)
	float Dist = 0;							// �ŒZ����(�����݁A�ꕔ�̌`��ł̐��m�Ȓl�ɂȂ��Ă��܂�)

	// vs���b�V����p
	int MeshIdx = -1;	// (vs���b�V����p)���b�V���ԍ�
	int FaceIdx = -1;	// (vs���b�V����p)�ʔԍ�
};

//==============================================
//
// Collider��{�R���|�[�l���g
// �E[ �d�l ]
// �@m_isAtk�t���O�ɂ��A�u���肷�鑤�v�Ɓu���肳��鑤�v�Ƃɕ������
// �@���肷�鑤		�c �u���肳��鑤�v�ɑ΂��Ă����蔻����s���A���̌��ʃf�[�^���L�����Am_onHit�n�̊֐������s(�ʒm)����B
// �@���肳��鑤	�c ��������͔��肵�ɍs���Ȃ��B�u���肷�鑤�v����Ђ����画�肳���݂̂ŁA���ʃf�[�^���������A�ʒm���s��Ȃ��B
//
//==============================================
class BaseColliderComponent : public BaseComponent {
	friend class CollisionManager;
	friend class CollisionFunctions;
public:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̕ϐ��E�֐�
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// �R���C�_�[�̎�ޒ萔
	enum class Types {
		Sphere = 0,		// ��
		Ray = 1,		// ���C
		Box = 2,		// ��
		Mesh = 3,		// ���b�V��
	};
	// �`��̎��(�����_�E���L���X�g�p)
	virtual Types GetType() const = 0;

	// �����蔻����s�@���� vs obj
	virtual bool HitTest(BaseColliderComponent* obj) = 0;

	// ����f�o�b�O�`��
	virtual void DebugDraw(const KdVec4& color) = 0;

	// ���肷�鑤�H
	bool IsAtk() const { return m_isAtk; }
	// �O���[�v�t�B���^�ݒ�
	void SetGroupFilter(int filter) { m_groupFilter = filter; }
	// �}�X�N�t�B���^�ݒ�
	void SetMaskFilter(int filter) { m_maskFilter = filter; }

	// �����G���W���̌`��Ƃ��Ďg�p����H
	bool UsePhysics() const { return m_usePhysics; }

	// �^�O�ݒ�E�擾
	const std::string& GetTag() const { return m_tag; }
	void SetTag(const std::string& tag) { m_tag = tag; }

	//----------------------------------------------
	//
	// ���肷�鑤(m_isAtk == true ��) ��p�̃f�[�^
	//
	//----------------------------------------------

	// �����蔻�茋�ʃf�[�^�擾�B�����ɂ����蔻��̌��ʃf�[�^�������Ă���B
	const std::vector<CollisionResult>& GetResults() const { return m_results; }

	// �������X�g�֓o�^
	void AddIgnore(Object* obj, UINT lifespan) {
		m_ignoreMap[obj] = lifespan;
	}

	// �q�b�g�ʒm�֐��I�u�W�F�N�g
	std::function<void(BaseColliderComponent*)>	m_onHitEnter;	// �q�b�g���̏���̂ݎ��s�����
	std::function<void(BaseColliderComponent*)>	m_onHitStay;	// �q�b�g�������Ǝ��s�����
	std::function<void(BaseColliderComponent*)>	m_onHitExit;	// �q�b�g���Ȃ��Ȃ�������̂ݎ��s�����

	// �����J�E���^�[���Z�b�g
	void SetDisableCounter(int count) {
		m_disableCounter = count;
	}
	// �����J�E���^�[�̒l�����Z����(���Ԃ���������)
	void AddDisableCounter(int count) {
		m_disableCounter += count;
	}
	// �����J�E���^�[�i�s
	void UpdateDisableCounter() {
		if (m_disableCounter >= 0) {
			m_disableCounter--;
			if (m_disableCounter <= -1) {
				m_enable = false;	// �����ɂ���
			}
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// ��������ʏ�R���|�[�l���g�̊֐��I�[�o�[���C�h
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// (Editor�p)�`��
	virtual void EditorDraw() override;

	//----------------------------
	// Serialize / Deserialize
	//----------------------------

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Deserialize(const json11::Json& jsonObj) override {
		BaseComponent::Deserialize(jsonObj);

		m_tag = jsonObj["Tag"].string_value();
		m_isAtk = jsonObj["IsAtk"].bool_value();
		m_groupFilter = jsonObj["GroupFilter"].int_value();
		m_maskFilter = jsonObj["MaskFilter"].int_value();
		m_usePhysics = jsonObj["UsePhysics"].bool_value();
	}
	// JSON������
	virtual void Serialize(json11::Json::object& outJsonObj) override {
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Tag"] = m_tag;
		outJsonObj["IsAtk"] = m_isAtk;
		outJsonObj["GroupFilter"] = m_groupFilter;
		outJsonObj["MaskFilter"] = m_maskFilter;
		outJsonObj["UsePhysics"] = m_usePhysics;
	}

	//----------------------------
	// Editor(ImGui)
	//----------------------------
	virtual void ImGuiUpdate() override;


protected:

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//
	// �R���C�_�[�̃f�[�^
	//
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// �ėp�^�O
	std::string			m_tag;

	// ���肷�鑤(���ʃf�[�^�̋L����ʒm���\)
	bool				m_isAtk = false;

	// �t�B���^
	int					m_groupFilter = 1;			// �O���[�v�t�B���^�@
	int					m_maskFilter = 0;	// �}�X�N�t�B���^

	// �q�b�g��ԃt���O(����q�b�g���A�q�b�g���A�q�b�g�������̔���p)
	int					m_hitState = 0;

	// ���ʃf�[�^�@�����ɂ����蔻��̌��ʃf�[�^�������Ă���
	std::vector<CollisionResult>	m_results;

	// �����}�b�v�@�����ɓo�^����Ă���GameObject�͔���Ώۂ��珜�O�����B
	// ���x�d���̂��߁A���|�C���^���g�p�B���ۂɓ����ɂ̓A�N�Z�X���Ă��܂���B
	std::unordered_map<Object*, UINT>	m_ignoreMap;
	void IgnoreMapUpdate();

	// �����G���W���̌`��Ƃ��Ďg�p����
	bool				m_usePhysics = false;

	// �w�莞�Ԍ��Enable��false�ɂ���J�E���^
	int		m_disableCounter = -1;

};

// ���R���C�_�[�R���|�[�l���g
#include "SphereColliderComponent.h"
// ���C�R���C�_�[�R���|�[�l���g
#include "RayColliderComponent.h"
// ���R���C�_�[�R���|�[�l���g
#include "BoxColliderComponent.h"
// ���b�V���R���C�_�[�R���|�[�l���g
#include "MeshColliderComponent.h"

// Collision�Ǘ��N���X
#include "CollisionManager.h"
