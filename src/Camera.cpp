#include "System.h"
#include "Window.h"
#include "Camera.h"
#include "Transform.h"

Camera::Camera(SceneObject* owner) :
	Component(owner),
	cBuffer_(std::make_unique<ConstantBuffer>()),
	cBufferDef_(std::make_unique<ConstantBufferDef>())
{
	AMD::Window* window = System::instance().GetWindow();
	cBuffer_.get()->view = glm::mat4();
	cBuffer_.get()->proj = glm::perspectiveFovLH_ZO(glm::radians(45.0f), (float)window->GetWidth(), (float)window->GetHeight(), 0.1f, 1000.0f); //glm::orthoLH_ZO(-10.f, 10.f, -5.f, 5.f, 0.1f, 1000.0f);
	
	ConstantBufferDef* cbd = cBufferDef_.get();
	cbd->size = sizeof(ConstantBuffer);
	cbd->ptr = cBuffer_.get();
}

void Camera::Update() 
{
	Transform* t = GetTransform();
	glm::mat4 view = glm::lookAtLH(t->GetLocalPosition(), t->GetLocalPosition() + t->GetForward(), t->GetUp());
	cBuffer_.get()->view = view;
}