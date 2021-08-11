#pragma once

#include "Component.h"
#include <glm/glm.hpp>

class FPCameraCtrlComponent : public Component
{

public: 
	COMPONENT_BASE(FPCameraCtrlComponent)

	void Update();

	void SetGroundHeight(float h) {
		groundH_ = h;
	}

private:
	float accumulation_;
	float groundH_;
	float moveSpeed_;
	float bobbingSpeed_;
	float bobbingAmplitude_;
};