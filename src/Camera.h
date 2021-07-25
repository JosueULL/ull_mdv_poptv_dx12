#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.h"
#include "ConstantBufferDef.h"

class Camera : public Component{
public:
	struct ConstantBuffer
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	Camera() = delete;
	Camera(const Camera&) = delete;

	explicit Camera(SceneObject* owner);

	ConstantBuffer* GetCBuffer() {
		return cBuffer_.get();
	}

	ConstantBufferDef* GetCBufferDef() {
		return cBufferDef_.get();
	}

private:
	std::unique_ptr<ConstantBuffer> cBuffer_;
	std::unique_ptr<ConstantBufferDef> cBufferDef_;
};
