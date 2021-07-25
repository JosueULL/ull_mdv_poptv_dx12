#pragma once

#include "Component.h"

class FPCameraCtrlComponent : public Component
{
public: 
	COMPONENT_BASE(FPCameraCtrlComponent)

	void Update();
};