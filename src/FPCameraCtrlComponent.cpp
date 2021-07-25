#include "FPCameraCtrlComponent.h"
#include "System.h"
#include "SceneObject.h"
#include "Keyboard.h"

FPCameraCtrlComponent::FPCameraCtrlComponent(SceneObject* owner) : Component(owner) 
{
}

void FPCameraCtrlComponent::Update() {
	Keyboard* kb = System::instance().GetKeyboard();
	Transform* transform = GetTransform();
	
	glm::vec3 rot = transform->GetLocalRotation();
	glm::vec3 pos = transform->GetLocalPosition();
	
	if (kb->IsKeyHeld('W')) {
		pos += transform->GetForward() * 0.01f;
	}
	if (kb->IsKeyHeld('S')) {
		pos -= transform->GetForward() * 0.01f;
	}
	if (kb->IsKeyHeld('A')) {
		rot.y -= 1.0f;
	}
	if (kb->IsKeyHeld('D')) {
		rot.y += 1.0f;
	}

	transform->SetLocalPosition(pos);
	transform->SetLocalRotation(rot);
}