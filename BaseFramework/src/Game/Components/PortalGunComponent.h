#pragma once

#include "System\ComponentSystem\BaseComponent.h"

class PortalGunComponent : public BaseComponent
{
public:
	virtual void Start() override;

	virtual void Update() override;

	virtual void ImGuiUpdate()override;

	virtual void Deserialize(const json11::Json& jsonObj)override;

	virtual void Serialize(json11::Json::object& outJsonObj)override;

private:
	// ’¼‘O‚Ü‚Å‚ÌƒJƒƒ‰‚Ìs—ñ‚ğ•Û‘¶
	KdMatrix m_mCam;
};