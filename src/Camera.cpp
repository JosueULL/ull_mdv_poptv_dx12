#include "Common.h"
#include "System.h"
#include "Window.h"
#include "Camera.h"
#include "Transform.h"

Camera::Camera(SceneObject* owner) :
	Component(owner),
	cBuffer_(new ConstantBuffer()),
	cBufferDef_(new ConstantBufferDef())
{
	Window* window = System::instance().GetWindow();
	cBuffer_->view = glm::lookAtLH(glm::vec3( 0,0,0 ), { 0,0,1 }, { 0,1,0 });
	cBuffer_->proj = glm::perspectiveFovLH_ZO(glm::radians(45.0f), (float)window->GetWidth(), (float)window->GetHeight(), 0.1f, 1000.0f);
	
	cBufferDef_->size = sizeof(ConstantBuffer);
	cBufferDef_->ptr = cBuffer_;
}

Camera::~Camera() {
	SAFE_FREE(cBuffer_);
}

void Camera::Update() 
{
	Transform* t = GetTransform();
	glm::mat4 view = glm::lookAtLH(t->GetLocalPosition(), t->GetLocalPosition() + t->GetForward(), t->GetUp());
	cBuffer_->view = view;
}

void Camera::SetOrthographic() 
{
	Window* window = System::instance().GetWindow();
	cBuffer_->proj = glm::orthoLH_ZO(0.0f, (float)window->GetWidth(), 0.0f, (float)window->GetHeight(), 0.1f, 1000.0f);
}