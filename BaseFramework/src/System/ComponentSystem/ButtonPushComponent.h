#pragma once

#include "BaseComponent.h"

// �{�^����������悤�ɂ���R���|�[�l���g
// �������݉��l��������Ɣ���
// ��X�̏C�����K�v����
//

class ButtonPushComponent : public BaseComponent
{
public:
	virtual void Update() override;

	virtual void ImGuiUpdate() override;

	virtual void Deserialize(const json11::Json& jsonObj)override;

	virtual void Serialize(json11::Json::object& outJson)override;
private:

};