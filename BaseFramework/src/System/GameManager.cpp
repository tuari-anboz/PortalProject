#include "main.h"

#include "GameManager.h"


void Object::UpdateObject()
{
	// �L���Ȃ珊�����Ă���R���|�[�l���g������
	if (m_isActive)
	{
		// �e�R���|�[�l���g�̏�����s����
		for (auto && comp : m_components)
		{
			comp->CallStart();
		}

		if (GameMgr.m_Editor_IsPlay)
		{
			// �S�R���|�[�l���g�̏��������s
			for (auto&& comp : m_components)
			{
				// �L���ȃR���|�[�l���g�̂ݏ��������s
				if (comp->GetEnable())
				{
					comp->Update();
				}
			}
		}
	}

	// �폜�t���O��On�̃R���|�[�l���g������
	{
		auto it = m_components.begin();
		while (it != m_components.end()) {
			// �폜�t���OOn?
			if ((*it)->IsDelete()) {
				// ���X�g����폜���āA���̂���󂯎��
				it = m_components.erase(it);
				continue;
			}
			// ���̂��
			++it;
		}
	}

	// �e�{�[���ɒǏ]������
	if (m_followBoneName.size() > 0)
	{
		// �e�I�u�W�F�N�g
		if (auto parent = GetParent())
		{
			// �e��ModelComponent
			if (auto model = parent->GetComponent<ModelComponent>())
			{
				// BoneController�擾
				auto & bc = model->GetBc();
				// �w�薼�̃{�[��������
				auto bone = bc.SearchBone(m_followBoneName);
				if (bone)
				{
					// �{�[���̃��[���h�s������߂�
					KdMatrix mBone = bone->LocalMat * parent->GetMatrix();
					// �����ɃZ�b�g
					SetMatrix(mBone, true);
				}
			}
		}
	}


	// �X�V����
	Update();

	// �L���Ȃ�q������
	if (m_isActive)
	{
		// �q������
		for (auto&& child : m_childs)
		{
			child->UpdateObject();
		}
	}

	// �폜�t���O��On�̎q������
	{
		auto it = m_childs.begin();
		while (it != m_childs.end())
		{
			// �폜�t���OOn?
			if ((*it)->IsDelete())
			{
				// ���X�g����폜���āA���̂�������Ƃ�
				it = m_childs.erase(it);
				continue;
			}
			// ���̂��
			++it;
		}
	}

}

void Object::PreDrawObject()
{
	// �S�R���|�[�l���g�̏��������s
	for (auto&& comp : m_components) {
		comp->PreDraw();
	}

	// �`��O����
	PreDraw();

	// �q������
	for (auto&& child : m_childs)
	{
		child->PreDrawObject();
	}
}

void Object::DrawObject()
{
	// �L���Ȃ�`�揈�������s
	if (m_isActive)
	{
		// �S�R���|�[�l���g�̏��������s
		for (auto&& comp : m_components) {
			comp->Draw();
		}

		// �`�揈��
		Draw();

		// �q������
		for (auto&& child : m_childs)
		{
			child->DrawObject();
		}
	}
}

void Object::DrawTransparencyObject()
{
	// �L���Ȃ�`�揈�������s
	if (m_isActive)
	{
		// �S�R���|�[�l���g�̏��������s
		for (auto&& comp : m_components) {
			comp->DrawTransparency();
		}

		// �`�揈��
		DrawTransparency();

		// �q������
		for (auto&& child : m_childs) {
			child->DrawTransparencyObject();
		}
	}
}

void Object::DrawShadowMapObject()
{
	// �L���Ȃ�`�揈�������s
	if (m_isActive)
	{
		// �S�R���|�[�l���g�̏��������s
		for (auto&& comp : m_components) {
			comp->DrawShadowMap();
		}

		// �`�揈��
		DrawTransparency();

		// �q������
		for (auto&& child : m_childs) {
			child->DrawShadowMapObject();
		}
	}
}


void Object::DrawEffectObject()
{
	// �L���Ȃ�`�揈�������s
	if (m_isActive)
	{
		// �S�R���|�[�l���g�̏��������s
		for (auto&& comp : m_components) {
			comp->DrawEffect();
		}

		// �`��
		DrawEffect();

		// �q������
		for (auto&& child : m_childs) {
			child->DrawEffectObject();
		}
	}
}


void Object::ImGuiUpdate()
{
	// �����蔻��̃}���`�X���b�h��ON
	ColMgr.SetMultiThread(true);

	// �N���X����\��
	ImGui::Separator();
	ImGui::Text((GetObjectClassName() + " Class").c_str());
	ImGui::Separator();
	ImGui::Spacing();

	// �R���|�[�l���g�ǉ��{�^��
	ImGui::Button("Add Component");
	// ���N���b�N�ŃR���e�L�X�g���j���[�\��
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// �o�^����Ă���S�N���X
		for (auto&& cls : GameMgr.m_componentClassMaker) {
			if (ImGui::Selectable(cls.first.c_str())) {
				// ���̃R���|�[�l���g�𐶐�
				auto comp = cls.second();
				// �ǉ�
				AddComponent(comp);
			}
		}

		ImGui::EndPopup();
	}


	// ���O
	ImGuiInputString("Name", m_name);
	// ����
	ImGui::DragFloat(u8"����", &m_mass,0.01f,0,1000);
	// �s��
	KdMatrix m = GetMatrix();
	ImGuizmoEditTransform(m, ShMgr.m_mView, ShMgr.m_mProj, nullptr);
	SetMatrix(m, true);


	// �S�R���|�[�l���g��ImGui����
	for (auto&& comp : m_components) {

		ImGui::PushID( (int)comp.get() );

		bool bOpen = ImGui::CollapsingHeader(comp->ClassName().c_str(),
			ImGuiTreeNodeFlags_DefaultOpen);

		// �E�N���b�N�ő���|�b�v�A�b�v
		if (ImGui::BeginPopupContextItem("ComponentOperation")) {
			// �폜
			if (ImGui::Selectable("Delete")) {
				comp->Delete();	// �폜�t���OOn
			}
			if (ImGui::Selectable("MoveUp"))
			{
				comp->MoveUp();
			}
			if (ImGui::Selectable("MoveDown"))
			{
				comp->MoveDown();
			}

			ImGui::EndPopup();
		}

		// CollapsingHeader���J���Ă���Ƃ��͓��e��\������
		if (bOpen) {
			comp->ImGuiUpdate();
		}


		ImGui::PopID();
	}
}

void Object::Deserialize(const json11::Json & jsonObj)
{
	// ���O
	m_name = jsonObj["Name"].string_value();
	// �s��Z�b�g
	m_mWorld.Deserialize(jsonObj["Matrix"]);
	// ����
	m_mass = static_cast<float>(jsonObj["Mass"].number_value());		// (float)�]���̃L���X�g�͔񐄏�
	// �L��
	if (jsonObj["Active"].is_bool())
	{
		m_isActive = jsonObj["Active"].bool_value();
	}
	// �R���|�[�l���g
	auto& compAry = jsonObj["Components"].array_items();
	for (auto&& compJson : compAry) {
		// �R���|�[�l���g�̃N���X��
		std::string className = compJson["ClassName"].string_value();
		// className�̃N���X�𐶐�
		auto comp = GameMgr.m_componentClassMaker[className]();
		// �R���|�[�l���g���f�V���A���C�Y
		comp->Deserialize(compJson);
		// �R���|�[�l���g���X�g�֒ǉ�
		AddComponent(comp);
	}
	// �q
	auto& childAry = jsonObj["Childs"].array_items();
	for (auto&& childJson : childAry)
	{
		// �I�u�W�F�N�g�̃N���X��
		std::string className = childJson["ClassName"].string_value();
		// className�̃N���X�𐶐�
		auto obj = GameMgr.m_objectClassMaker[className]();
		// �e��ݒ�(�e�̎q���X�g�ɒǉ������
		obj->SetParent(shared_from_this());
		// �R���|�[�l���g���f�V���A���C�Y
		obj->Deserialize(childJson);
	}
}

void GameManager::Init()
{
	//================================
	// �V�X�e���n�̃R���|�[�l���g�̃N���X��o�^
	//================================
	REGISTER_COMP_CLASS(PlayerInputComponent);
	REGISTER_COMP_CLASS(BasicInputComponent);
	REGISTER_COMP_CLASS(ModelComponent);
	REGISTER_COMP_CLASS(EffectModelComponent);
	REGISTER_COMP_CLASS(SpriteComponent);
	REGISTER_COMP_CLASS(CameraComponent);
	REGISTER_COMP_CLASS(LightComponent);
	REGISTER_COMP_CLASS(AudioListenerComponent);
	REGISTER_COMP_CLASS(AudioComponent);
	REGISTER_COMP_CLASS(PortalComponent);
	REGISTER_COMP_CLASS(TitleScript);
	REGISTER_COMP_CLASS(PlayerScript);
	REGISTER_COMP_CLASS(FpsCameraComponent);
	REGISTER_COMP_CLASS(LevelEditorScript);
	REGISTER_COMP_CLASS(UIButtonComponent);
	REGISTER_COMP_CLASS(RigidBodyComponent);
	REGISTER_COMP_CLASS(RideButtonComponent);
	REGISTER_COMP_CLASS(BoxGeneratorComponent);
	REGISTER_COMP_CLASS(DoorComponent);
	REGISTER_COMP_CLASS(ButtonPushComponent);
	REGISTER_COMP_CLASS(FadeComponent);
	REGISTER_COMP_CLASS(LoadLevelComponent);

	// �R���C�_�[�R���|�[�l���g�N���X�o�^
	REGISTER_COMP_CLASS(BoxColliderComponent);
	REGISTER_COMP_CLASS(SphereColliderComponent);
	REGISTER_COMP_CLASS(RayColliderComponent);
	REGISTER_COMP_CLASS(MeshColliderComponent);

	//================================
	// �������[���h�쐬
	//================================
	m_physicsWorld = m_physicsWorld->CreateWorld();
	m_physicsWorld->Init();


	//================================
	// ���x���̐���
	//================================
	m_level = std::make_shared<Level>();
	m_level->Load("data/StageDataLevelEditor.json");

	m_Editor_Log.AddLog(u8"Editor�N��");

	// (��) �g�e�N�X�`���쐬
	m_wave.Create(512, 512);

	// �t�F�[�h������
	m_fade = false;
}


void GameManager::Run()
{
	//============================
	// ���x���̕ύX����
	//============================
	if (m_changeLevelFile.size() > 0) 	// �\�񂪂���H
	{
		// Level��ύX����
		m_level = std::make_shared<Level>();
		m_level->Load(m_changeLevelFile);

		m_changeLevelFile = "";	// �\�����
	}


	//============================
	// �X�V����
	//============================
	if (m_Editor_IsPlay)
	{
		// �������E�̎��Ԃ������߂�(�v�Z����)�B
		m_physicsWorld->StepSimulation(1 / 60.0f);
	}

	// ���x���X�V����
	m_level->Update();

	KDAudioMgr.Update();

	// (Editor)�J��������
	m_Editor_Camera.Update();

	// �g�i�s
	ShMgr.m_postProcessSh.AdvanceWave(m_wave);

	//============================
	// �`�揈��
	//============================

	// �|�C���g���C�g���Z�b�g
	ShMgr.m_KdModelSh.ResetPointLight();
	// ���s���C�g���Z�b�g(�^�����ɂ�������)
	ShMgr.m_KdModelSh.SetDirectionalLight({ 0,0,0 }, { 0,0,0,0 });
	// �X�|�b�g���C�g���Z�b�g
	ShMgr.m_KdModelSh.ResetSpotLight();

	// ���C�g���Z�b�g
	ShMgr.m_cb8_Light.GetWork().DL_Cnt = 0;
	ShMgr.m_cb8_Light.GetWork().PL_Cnt = 0;

	//--------------
	// �J�����ݒ�
	//--------------
	m_Editor_Camera.SetToShader();

	// �o�b�N�o�b�t�@���N���A����
	KD3D.GetBackBuffer()->ClearRT({ 0.2f,0.2f,0.2f, 1 });
	// Z�o�b�t�@���N���A����
	KD3D.GetZBuffer()->ClearDepth();

	// ���x���`�揈��
	m_level->Draw();

	if (m_Editor_showDebug)
	{
		// �����蔻��f�o�b�O�\��(�ŏI�I�ɂ͏���)
		ColMgr.DebugDraw();

		// �I�𕨂̃G�f�B�^�[�`������s
		{
			auto select = m_Editor_SelectObj.lock();
			if (select)
			{
				// �q�̃G�f�B�^�[�`������s
				std::function<void(const SPtr<Object>&)> rec = [this, &rec](const SPtr<Object>& obj)
				{
					obj->EditorDrawObject();

					// �q���ċA
					for (auto&& child : obj->GetChilds())
					{
						rec(child);
					}
				};
				// �֐������s
				rec(select);
			}
		}
	}

	//===========================
	// ImGui�`��
	//===========================
	if (m_Editor_showUI)
	{
		// ImGui�J�n
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGuizmo
		ImGuizmo::BeginFrame();

		// ���s����GUI�E�B���h�E��Ԃ�����
		bool isPlay = m_Editor_IsPlay;
		if(isPlay)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0, 0, 0.6f));
		}

		// ImGui Demo �E�B���h�E�\�� ���������Q�l�ɂȂ�E�B���h�E�ł��Bimgui_demo.cpp�Q�ƁB
		ImGui::ShowDemoWindow(nullptr);
		ImGuiUpdate();

		ImGui::ShowMetricsWindow(nullptr);

		if (isPlay)
		{
			ImGui::PopStyleColor(1);
		}

		// GUI�`����s
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	if (KdIsKeyDown(VK_F1, false))
	{
		m_Editor_showUI = !m_Editor_showUI;
	}

	if (KdIsKeyDown(VK_F2, false))
	{
		m_Editor_showDebug = !m_Editor_showDebug;
	}

	// �o�b�N�o�b�t�@�̓��e����ʂɕ\��
	KD3D.GetSwapChain()->Present(0, 0);

}

void GameManager::ImGuiUpdate()
{

	// ���x����ImGui����
	m_level->ImGuiUpdate();

	//=============================
	// GraphicsDebug�E�B���h�E
	//=============================
	if (ImGui::Begin("Graphics Debug", nullptr)) {
		// �g�p �����}�b�v
		ImGui::Image(m_wave.m_rtHeight[0]->GetSRV(),
			ImVec2(200, 200));
		// �g�p �@���}�b�v
		ImGui::Image(m_wave.m_rtNormal->GetSRV(),
			ImVec2(200, 200));
		// �V���h�E�}�b�v�f�o�b�O�\��
		for (int i = 0; i < 4; i++)
		{
			// GetSRV_Array(i) �c �e�N�X�`���z���i�Ԗڂ�SRV���w��
			ImGui::Image(ShMgr.m_genShadowMapSh.GenDepthMap()->GetSRV_Array(i), ImVec2(100, 100));
		}
	}
	ImGui::End();

	//=============================
	// Inspector�E�B���h�E
	//=============================
	if (ImGui::Begin("Inspector", nullptr)) {
		SPtr<Object> select = m_Editor_SelectObj.lock();
		if (select) {
			// 
			select->ImGuiUpdate();

		}
	}
	ImGui::End();

	//==================================
	// GameManager�E�B���h�E
	//==================================
	if (ImGui::Begin("Game Manager", nullptr))
	{
		// ���s
		if (ImGui::Checkbox(u8"���s", &m_Editor_IsPlay)) {
			static json11::Json::object saveLevel;

			// ���s
			if (m_Editor_IsPlay) {
				// ���݂̃��x�����V���A���C�Y
				m_level->Serialize(saveLevel);
			}
			// ��~
			else {
				m_level->Deserialize(saveLevel);
				saveLevel.clear();
			}
		}
		ImGuiShowHelp(u8"���s����Update�����s�����B", false);

		// �J�����R���|�[�l���g���g�p����
		ImGui::Checkbox(u8"�J�����R���|�[�l���g�g�p", &m_Editor_UseCameraComponent);
		ImGuiShowHelp(u8"�J�����R���|�[�l���g�����݂���ꍇ�́A������g�p����", false);
	}
	ImGui::End();

	//==================================
	// ���O�E�B���h�E
	//==================================
	m_Editor_Log.ImGuiUpdate("Log Window");
}

SPtr<Object> GameManager::Instantiate(const json11::Json & jsonObj)
{
	if (jsonObj.is_null()) return nullptr;

	std::string className = jsonObj["ClassName"].string_value();
	// ���݃`�F�b�N 
	if (m_objectClassMaker.count(jsonObj["ClassName"].string_value()) == 0) return nullptr;
	// ����   
	SPtr<Object> newObj = m_objectClassMaker[className]();

	// �f�V���A���C�Y 
	newObj->Deserialize(jsonObj);

	return newObj;
}

SPtr<Object> GameManager::Duplicate(const SPtr<Object> obj)
{

	// obj���V���A���C�Y(JSON�f�[�^��)
	json11::Json::object serial;
	obj->Serialize(serial);
	// ������
	// �V�K�I�u�W�F�N�g���쐬
	std::string className = serial["ClassName"].string_value();
	SPtr<Object> newObj = GameMgr.m_objectClassMaker[className]();
	// �f�V���A���C�Y
	newObj->Deserialize(serial);
	// Root�̎q�ɂ���
	newObj->SetParent(obj->GetParent());
	// ������I����Ԃɂ���
	GameMgr.m_Editor_SelectObj = newObj;
	return newObj;
}



void Object::SetMatrix(const KdMatrix & m, bool moveChild)
{
	// �ω��O�̍s����o�b�N�A�b�v
	m_mPrevWorld = m_mWorld;

	// �s��Z�b�g
	m_mWorld = m;

	// �ω��O�ƍ���̍s�񂩂�A1�t���[���ł̕ω��ʎZ�o
	KdMatrix mInvPrev;
	m_mPrevWorld.Inverse(mInvPrev);
	KdMatrix mDelta = mInvPrev * m_mWorld;

	// �q���A��������
	if (moveChild)
	{
		// �q�ȉ��̂��ׂĂ�GameObject��Transform��mDelta���������Ă����֐�
		std::function<void(const SPtr<Object>&)> proc =  [this, &proc,&mDelta](const SPtr<Object>& obj)
		{
			// �ω��O�̍s����o�b�N�A�b�v
			obj->m_mPrevWorld = obj->m_mWorld;
			// �K�p
			obj->m_mWorld *= mDelta;

			// �q�ċA
			for (auto&& child : obj->GetChilds())
			{
				proc(child);
			}
		};

		// �q����������
		for (auto&& child : GetChilds())
		{
			proc(child);
		}
	}
}

