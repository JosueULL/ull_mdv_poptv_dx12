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

	COMPONENT_BASE(Camera)

	~Camera();

	ConstantBuffer* GetCBuffer() {
		return cBuffer_;
	}

	ConstantBufferDef* GetCBufferDef() {
		return cBufferDef_;
	}

	void Update() override;
	void SetOrthographic();

private:
	ConstantBuffer* cBuffer_;
	ConstantBufferDef* cBufferDef_;
};
