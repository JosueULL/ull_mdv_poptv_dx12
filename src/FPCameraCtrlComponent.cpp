#define NOMINMAX

#include "FPCameraCtrlComponent.h"
#include "System.h"
#include "SceneObject.h"
#include "Keyboard.h"

FPCameraCtrlComponent::FPCameraCtrlComponent(SceneObject* owner) : 
	Component(owner),
	accumulation_(0),
	groundH_(0),
	moveSpeed_(0.05f),
	bobbingSpeed_(6.f),
	bobbingAmplitude_(0.035f)
{
}

void FPCameraCtrlComponent::Update() {
	Keyboard* kb = System::instance().GetKeyboard();
	Transform* transform = GetTransform();
	
	glm::vec3 rot = transform->GetLocalRotation();
	glm::vec3 pos = transform->GetLocalPosition();
	glm::vec3 prevPos = pos;

	if (kb->IsKeyHeld('W')) {
		pos += transform->GetForward() * moveSpeed_;
	}
	if (kb->IsKeyHeld('S')) {
		pos -= transform->GetForward() * moveSpeed_;
	}
	if (kb->IsKeyHeld('A')) {
		rot.y -= 2.5f;
	}
	if (kb->IsKeyHeld('D')) {
		rot.y += 2.5f;
	}

	float frameAccum = glm::distance(pos, prevPos);
	accumulation_ += frameAccum;
	pos.y = groundH_ + std::sin(accumulation_ * bobbingSpeed_) * bobbingAmplitude_;

	transform->SetLocalPosition(pos);
	transform->SetLocalRotation(rot);
}