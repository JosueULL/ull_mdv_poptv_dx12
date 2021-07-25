#pragma once


#pragma warning(disable:4201)   // suppress even more warnings about nameless structs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {

public:

	Transform() :
		position_(0),
		scale_(1),
		rotation_(0),
		mModel_(glm::identity<glm::mat4>()),
		dirty_(true)
	{

	}

	glm::mat4 GetModel() {
		if (dirty_)
			UpdateModel();

		return mModel_;
	}

	void SetLocalScale(glm::vec3 scale) {
		scale_ = scale;
		dirty_ = true;
	}

	void SetLocalPosition(glm::vec3 position) {
		position_ = position;
		dirty_ = true;
	}

	void SetLocalRotation(glm::vec3 rotation) {
		rotation_ = rotation;
		dirty_ = true;
	}

private:
	glm::vec3 position_;
	glm::vec3 scale_;
	glm::vec3 rotation_;

	glm::mat4 mModel_;	

	bool dirty_;

	void UpdateModel();
};