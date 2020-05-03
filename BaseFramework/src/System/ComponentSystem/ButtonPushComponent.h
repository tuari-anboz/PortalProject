#pragma once

#include "BaseComponent.h"

// ボタンを押せるようにするコンポーネント
// 正直存在価値がちょっと微妙
// 後々の修正が必要かも
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