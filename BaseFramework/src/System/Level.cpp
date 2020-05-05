#include "main.h"

#include "GameManager.h"

Level::Level()
{
	// �o�b�N�o�b�t�@�Ɠ����T�C�Y��RT�쐬
	for (int i = 0; i < 2; i++)
	{
		m_rtWork[i].CreateRT(
			KD3D.GetBackBuffer()->GetInfo().Width,
			KD3D.GetBackBuffer()->GetInfo().Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT
		);
	}
	m_rtHighBrightness.CreateRT(
		KD3D.GetBackBuffer()->GetInfo().Width,
		KD3D.GetBackBuffer()->GetInfo().Height,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
	m_blurTex.Create(KD3D.GetBackBuffer()->GetInfo().Width,
		KD3D.GetBackBuffer()->GetInfo().Height
	);
}

// Serialize/Deserialize

void Level::Deserialize(const json11::Json & jsonObj)
{
	// ���[�g���쐬
	m_root = std::make_shared<Object>();

	// Root�I�u�W�F�N�g
	json11::Json jsn = jsonObj["Object"];
	// �f�V���A���C�Y
	m_root->Deserialize(jsn);
	// �O���t�B�b�N�ݒ�
	if (jsonObj["GraphicSetting"].is_object())
	{
		auto& graSetting = jsonObj["GraphicSetting"];
		m_fogColor.Deserialize(graSetting["FogColor"]);
		m_fogDensity = (float)graSetting["FogDensity"].number_value();
	}
}

void Level::Serialize(json11::Json::object & outJson)
{
	// 
	json11::Json::object root;
	m_root->Serialize(root);

	outJson["Object"] = root;

	// �O���t�B�b�N�ݒ�
	json11::Json::object graphicSetting;
	graphicSetting["FogColor"] = m_fogColor.Serialize();
	graphicSetting["FogDensity"] = m_fogDensity;
	outJson["GraphicSetting"] = graphicSetting;
}

void Level::Load(const std::string& filename)
{
	// ���x���t�@�C�������L�����Ă���
	m_levelFilename = filename;


	// ���[�g���쐬
	m_root = std::make_shared<Object>();

	// JSON�t�@�C���̓��e��S�ĕ�����Ƃ��ēǂݍ��ށB
	std::string str = KdLoadStringFile(filename);
	// JSON���
	std::string err;
	json11::Json jsn = json11::Json::parse(str, err);
	if (err.size() > 0) {
		// ��̓G���[�@err�ɃG���[���e��������œ����Ă���
		assert(0 && "JSON��̓G���[�I");
	}

	Deserialize(jsn);

	// IBL�e�N�X�`���ǂݍ���
	m_texIBL = KDResFactory.GetTexture("data/Texture/IBL/Park.dds");
	if (m_texIBL)m_texIBL->PSSetShaderResource(11);

	/*
	// Root�I�u�W�F�N�g
	json11::Json jsonObj = jsn["Object"];
	// �f�V���A���C�Y
	m_root->Deserialize(jsonObj);
	*/
}

void Level::Save()
{
	/*
	// 
	json11::Json::object root;
	m_root->Serialize(root);

	// �X�e�[�W�f�[�^
	json11::Json::object stageData;
	stageData["Object"] = root;
	// ������
	json11::Json jsn = stageData;
	std::string strJson = jsn.dump();	// ������֕ϊ�
	*/

	json11::Json::object serial;
	Serialize(serial);

	// �t�@�C���֕ۑ�
	json11::Json jsn = serial;
	std::string strJson = jsn.dump();	// ������֕ϊ�
	KdSaveStringFile(m_levelFilename, strJson);
}

void Level::Update()
{
	// �@����I�u�W�F�N�g���X�g���N���A����
	ColMgr.ClearList();

	// ���[�g�I�u�W�F�N�g���s
	m_root->UpdateObject();

	// �B�ƇC�R���W�����������s(�A�œo�^���ꂽ�R���C�_�[�B�𔻒肵�A���ʂ�ʒm����)
	ColMgr.Run();


}

void Level::Draw()
{
	//========================
	// �O���t�B�b�N�X�ݒ�
	//========================
	// �t�H�O�ݒ�
	ShMgr.m_cb7_Camera.GetWork().DistanceFogColor = m_fogColor;
	ShMgr.m_cb7_Camera.GetWork().DistanceFogDensity = m_fogDensity;
	ShMgr.m_cb7_Camera.Write();

	// �I�u�W�F�N�g�̕`��O����
	m_root->PreDrawObject();

	// ���C�g�f�[�^����������
	ShMgr.m_cb8_Light.Write();
	
	// �I�u�W�F�N�g��3D�`�揈��
	//m_root->DrawObject();

	// (0)�V���h�E�}�b�v����
	{
		// �֐����쐬
		auto drawProc = [this]()
		{
			m_root->DrawShadowMapObject();
		};
		ShMgr.m_genShadowMapSh.Generate(drawProc);
	}

	// �I�u�W�F�N�g��3D�`�揈��
	{
		// ���݂�RT,Z�o�b�t�@�ƕۑ�
		KdRTSaver rtSave;

		// RT,Z�o�b�t�@�ύX�p
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[0]);
		rtc.Depth(*KD3D.GetZBuffer());
		rtc.SetToDevice();		// RT,Z�o�b�t�@��؂�ւ���

		// RT���N���A
		m_rtWork[0].ClearRT({ 0, 0, 0, 1 });
		// Z�o�b�t�@���N���A
		KD3D.GetZBuffer()->ClearDepth();

		// �S���̂�`��
		m_root->DrawObject();
	}

	// �AWork[1]�փR�s�[
	{
		// ���݂�RT,Z�o�b�t�@�ƕۑ�
		KdRTSaver rtSave;

		// RT,Z�o�b�t�@�ύX�p
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[1]);
		rtc.Depth(nullptr);
		rtc.SetToDevice();		// RT,Z�o�b�t�@��؂�ւ���

		ShMgr.m_postProcessSh.CopyDraw(m_rtWork[0]);
	}

	// �B���������̕`��
	{
		// ���݂�RT,Z�o�b�t�@�ƕۑ�
		// ���݂�RT,Z�o�b�t�@�ƕۑ�
		KdRTSaver rtSave;

		// RT,Z�o�b�t�@�ύX�p
		KdRTChanger rtc;
		rtc.RT(0, m_rtWork[1]);
		rtc.Depth(*KD3D.GetZBuffer());
		rtc.SetToDevice();		// RT,Z�o�b�t�@��؂�ւ���

		// Work[0]���V�F�[�_�ɃZ�b�g���Ă���
		m_rtWork[0].PSSetShaderResource(10);

		// ���������̂�`��
		m_root->DrawTransparencyObject();

		// �e�N�X�`��������
		KdTexture::s_emptyTex.PSSetShaderResource(10);
	}

	// �C���P�x���o
	{
		// ���݂�RT,Z�o�b�t�@�ƕۑ�
		KdRTSaver rtSave;

		// RT,Z�o�b�t�@�ύX�p
		KdRTChanger rtc;
		rtc.RT(0, m_rtHighBrightness);
		rtc.Depth(nullptr);
		rtc.SetToDevice();		// RT,Z�o�b�t�@��؂�ւ���

		// �AWork���獂�P�x���o
		ShMgr.m_postProcessSh.BrightFilteringDraw(m_rtWork[1]);
	}

	// �D���P�x���o�摜��MGF
	ShMgr.m_postProcessSh.GenerateBlur(m_blurTex, m_rtHighBrightness);

	// �E�S�ڂ����摜�����Z�����`��
	{
		KdStateSaver stSave;
		stSave.SaveBS();		// ���݂�BlendState���L�����Ă���

		float blendFactor[] = { 0,0,0,0 };
		// ���Z�����ɐ؂�ւ�
		KD3D.GetDevContext()->OMSetBlendState(ShMgr.m_bsAdd, blendFactor, 0xFFFFFFF);

		// RT��؂�ւ���
		{
			// ���݂�RT,Z�o�b�t�@�ƕۑ�
			KdRTSaver rtSave;

			// RT,Z�o�b�t�@�ύX�p
			KdRTChanger rtc;
			rtc.RT(0, m_rtWork[1]);
			rtc.Depth(nullptr);
			rtc.SetToDevice();		// RT,Z�o�b�t�@��؂�ւ���

			//
			for (int i = 0; i < 5; i++)
			{
				ShMgr.m_postProcessSh.CopyDraw(m_blurTex.m_rt[i][0], { 0.2f, 0.2f, 0.2f, 1 });	// 20%
			}
		}
	}

	// �FWork -> BackBuffer
	//ShMgr.m_postProcessSh.CopyDraw(m_rtWork);
	ShMgr.m_postProcessSh.ToneMappingDraw(m_rtWork[1]);

	// �I�u�W�F�N�g�̃G�t�F�N�g�`��
	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteOFF, 0);	// Z��������OFF
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Both);									// ���ʕ`��
	m_root->DrawEffectObject();
	KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteON, 0);	// Z��������ON
	KD3D.GetDevContext()->RSSetState(ShMgr.m_rs_Default);								// �\�ʕ`��

	// �SObject��2D�`��
	ShMgr.m_KdSpriteSh.Begin(false);
	{
		KdStateSaver StateSaver;
		StateSaver.SaveDS();
		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteOFF, 0);

		m_root->DrawSpriteObject();

		KD3D.GetDevContext()->OMSetDepthStencilState(ShMgr.m_ds_ZCompareON_ZWriteON, 0);
	}
	ShMgr.m_KdSpriteSh.End();
}

void Level::ImGuiUpdate()
{
	//=============================
	// ObjectList�E�B���h�E
	//=============================
	if (ImGui::Begin("Object List", nullptr, ImGuiWindowFlags_MenuBar))
	{
		// ���C�����j���[
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// New���j���[
				if (ImGui::MenuItem("New")) {
					// Root�I�u�W�F�N�g���쐬
					m_root = std::make_shared<Object>();
					m_root->GetName() = "Root";
				}
				// Load���j���[
//				if (ImGui::MenuItem("Load")) {
//					Load();
//				}
				ImGui::Selectable("Load", false, ImGuiSelectableFlags_DontClosePopups);
				{
					std::string filename;
					if (ImGuiBeginPopup_ResourceBrowser(
						"LoadLevelPopup", filename, { ".json" }
					)) {
						// ���݂�Popup�E�B���h�E������
						ImGui::CloseCurrentPopup();
						// ���x���ǂݍ���
						Load(filename);
					}
				}

				// Save���j���[
				if (ImGui::MenuItem("Save")) {
					Save();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// ���݂̃��x���t�@�C����
		ImGuiInputString("Level File", m_levelFilename);

		if (ImGui::BeginTabBar("LevelTabBar", 0))
		{
			if (ImGui::BeginTabItem("Scene Setting"))
			{
				// �I�u�W�F�N�g�ǉ��{�^��
				if (GameMgr.GetSelectObj().expired() == false) {
					ImGui::Button("Add Object");
					ImGuiShowHelp(u8"�I��Object�̎q�Ƃ��āA�V�KObject��ǉ�����", false);
					// ���N���b�N�ŃR���e�L�X�g���j���[�\��
					if (ImGui::BeginPopupContextItem("AddObjectPopup", 0)) {
						// �o�^����Ă���S�N���X
						for (auto&& cls : GameMgr.m_objectClassMaker) {
							if (ImGui::Selectable(cls.first.c_str())) {
								// ���̃N���X�𐶐�
								SPtr<Object> p = cls.second();
								// �ǉ�
								//AddObject(p);
								// �I�𕨂̎q�Ƃ��Ēǉ�
								p->SetParent(GameMgr.GetSelectObj().lock());
								// ������I���ɂ���
								GameMgr.SetSelectObj(p);
							}
						}

						ImGui::EndPopup();
					}
				}

				// �I�u�W�F�N�g���X�g
		//		for (auto&& obj : m_objectList) {
				// �P��Object��ImGui����
				std::function<void(const SPtr<Object>&)> rec = [this, &rec](const SPtr<Object>& obj)
				{
					// �A�h���X������ID�Ƃ��Ďg��
					ImGui::PushID((int)obj.get());

					/*
					// ����obj���I�𒆁H
					bool bSelected = false;
					if (GameMgr.m_Editor_SelectObj.lock() == obj) {
						bSelected = true;
					}
					// �I�u�W�F�N�g�I
					if (ImGui::Selectable(obj->m_name.c_str(), bSelected))
					{
						GameMgr.m_Editor_SelectObj = obj;
					}
					*/

					// �c���[�̓���t���O
					int treeFlags = ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_OpenOnDoubleClick |
						ImGuiTreeNodeFlags_OpenOnArrow;
					// �I������Ă��鎞�́A�I����Ԃ̃t���O��t����
					if (GameMgr.GetSelectObj().lock() == obj)
					{
						treeFlags |= ImGuiTreeNodeFlags_Selected;
						//-----------------
						// Auto Scroll
						//-----------------
						// �I�u�W�F�N�g���I�����ꂽ�΂���ł������ĂȂ��Ƃ�
						if (GameMgr.m_Editor_JustSelected && !ImGui::IsItemVisible())
						{
							ImGui::SetScrollHereY();
						}
						GameMgr.m_Editor_JustSelected = false;
					}

					// �I�����ꂽ�A�C�e�����c���[�̓����ɉB��Ă���ꍇ�Ƀc���[���J��
					std::function<bool(const SPtr<Object>&)> selectedchild = [this, &selectedchild](const SPtr<Object>& obj)
					{
						if (GameMgr.GetSelectObj().lock() == obj)
						{
							return true;
						}
						else
						{
							// �q���ċA
							for (auto&& child : obj->GetChilds())
							{
								if (selectedchild(child))
								{
									return true;
								}
							}
						}
					};

					// �q���ċA
					for (auto&& child : obj->GetChilds()) {
						if (selectedchild(child))
						{
							ImGui::SetNextTreeNodeOpen(true);
						}
					}

					// �c���[�m�[�h
					bool bTreeOpen = ImGui::TreeNodeEx(obj.get(), treeFlags, obj->GetName().c_str());
					
					// �c���[�m�[�h���N���b�N���ꂽ��
					if (ImGui::IsItemClicked())
					{
						GameMgr.SetSelectObj(obj);
					}

					//-----------------
					// Drag & Drop
					//-----------------
					// Drag
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Object�̃A�h���X�𐔒l�Ƃ��ċL������
						intptr_t n = (intptr_t)obj.get();
						ImGui::SetDragDropPayload("ObjectDragDrop", &n, sizeof(n));
						ImGui::Text(u8"�e�ƂȂ�Object��I�����Ă�������");
						ImGui::EndDragDropSource();
					}
					// Object��Drop
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ObjectDragDrop"))
						{
							IM_ASSERT(payload->DataSize == sizeof(intptr_t));
							// int�^�ŋL�����Ă����A�h���X��Object�ɕ���
							Object* srcObj = (Object*)(*(intptr_t*)payload->Data);
							// �����̎q�ɂȂ�
							srcObj->SetParent(obj);
						}
						ImGui::EndDragDropTarget();
					}
					//-----------------

					// Ctrl + �}�E�X�z�C�[���ŏ��ԓ���ւ�
					if (treeFlags & ImGuiTreeNodeFlags_Selected) {// �I��
						if (ImGui::IsKeyDown(VK_CONTROL)) {
							if (ImGui::GetIO().MouseWheel > 0) {
								obj->MoveUp();
							}
							else if (ImGui::GetIO().MouseWheel < 0) {
								obj->MoveDown();
							}
						}
					}

					// �E�N���b�N�̃|�b�v�A�b�v���j���[
					if (ImGui::BeginPopupContextItem("ObjectPopupMenu", 1))
					{
						// �v���n�u�t�@�C����
						ImGui::Selectable("Save Prefab", false, ImGuiSelectableFlags_DontClosePopups);
						{
							std::string filename;
							if (ImGuiBeginPopup_ResourceBrowser(
								"SavePrefabPopup", filename, { ".pref" }, true
							)
								) {
								// ���݂�Popup�E�B���h�E������
								ImGui::CloseCurrentPopup();

								// �t�H���_��I�������Ƃ�

								// obj���V���A���C�Y(JSON�f�[�^��)
								json11::Json::object serial;
								obj->Serialize(serial);
								// ������
								// JSON�f�[�^�𕶎���
								json11::Json jsn = serial;
								std::string strJson = jsn.dump();
								// �t�@�C���֕ۑ�
		//						KdSaveStringFile(filename + "/test.pref", strJson);
								KdSaveStringFile(filename + "/" + obj->GetName() + ".pref", strJson);

							}

						}

						// �v���n�u�t�@�C������V�K�I�u�W�F�N�g�쐬
						ImGui::Selectable("Load Prefab", false, ImGuiSelectableFlags_DontClosePopups);
						{
							std::string filename;
							if (ImGuiBeginPopup_ResourceBrowser(
								"LoadPrefabPopup", filename, { ".pref" }
							)
								) {
								// ���݂�Popup�E�B���h�E������
								ImGui::CloseCurrentPopup();

								// �t�@�C����I�񂾎�
								std::string strJson = KdLoadStringFile(filename);

								// JSON���
								std::string err;
								json11::Json jsn = json11::Json::parse(strJson, err);
								if (err.size() == 0) {
									// �V�K�I�u�W�F�N�g�쐬
									SPtr<Object> p = GameMgr.m_objectClassMaker[jsn["ClassName"].string_value()]();
									if (p) {
										// �f�V���A���C�Y
										p->Deserialize(jsn);
										// GameMgr�ɒǉ�
		//								AddObject(p);
										// Root�̎q�ɂ���
										p->SetParent(m_root);
										// ������I����Ԃɂ���
										GameMgr.SetSelectObj(p);
									}
								}

							}
						}

						// ����
						if (ImGui::Selectable("Duplicate")) {
							// obj���V���A���C�Y(JSON�f�[�^��)
							json11::Json::object serial;
							obj->Serialize(serial);
							// ������
							// �V�K�I�u�W�F�N�g���쐬
							std::string className = serial["ClassName"].string_value();
							SPtr<Object> newObj = GameMgr.m_objectClassMaker[className]();
							// �f�V���A���C�Y
							newObj->Deserialize(serial);
							// ���X�g�֒ǉ�
		//					AddObject(newObj);
							// Root�̎q�ɂ���
							newObj->SetParent(obj->GetParent());
							// ������I����Ԃɂ���
							GameMgr.SetSelectObj(newObj);
						}

						// �폜
						if (ImGui::Selectable("Delete")) {
							obj->Delete();
						}

						// �K���Ō�ɌĂԂ���
						ImGui::EndPopup();
					}

					// �c���[�m�[�h���J����Ă���
					if (bTreeOpen)
					{
						// �q���ċA
						for (auto&& child : obj->GetChilds()) {
							rec(child);
						}

						// Tree Node�̏I���
						ImGui::TreePop();
					}

					// 
					ImGui::PopID();	// ����ID������
				};

				// �I�u�W�F�N�g���X�g
		//		for (auto&& obj : m_objectList) {
		//			rec(obj);
		//		}
				// ���[�g����ImGui�������s
				rec(m_root);

				ImGui::EndTabItem();
			}
			// �O���t�B�b�N�ݒ�^�u
			if (ImGui::BeginTabItem("GraphicsSetting"))
			{
				// �O���t�B�b�N�ݒ�
				ImGui::ColorEdit3("Fog Color", m_fogColor);
				ImGui::DragFloat(u8"Fog���x", &m_fogDensity, 0.001f, 0, 10.0f);

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
