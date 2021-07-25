#include "Camera.h"

Camera::Camera(SceneObject* owner) :
	Component(owner),
	cBuffer_(std::make_unique<ConstantBuffer>()),
	cBufferDef_(std::make_unique<ConstantBufferDef>())
{
	//glm::transpose(glm::perspectiveFovZO<float>(glm::radians(45.0f), 800, 400, 0.01f, 1000.0f));
	
	cBuffer_.get()->view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 2));
	cBuffer_.get()->proj = glm::perspectiveFovLH_ZO(glm::radians(45.0f), 800.0f, 600.0f, 0.1f, 1000.0f); //glm::orthoLH_ZO(-10.f, 10.f, -5.f, 5.f, 0.1f, 1000.0f);
	
	ConstantBufferDef* cbd = cBufferDef_.get();
	cbd->size = sizeof(ConstantBuffer);
	cbd->ptr = cBuffer_.get();
}
