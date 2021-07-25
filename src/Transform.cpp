#include "Transform.h"

void Transform::UpdateModel() {

	mModel_ = glm::identity<glm::mat4>();

	mModel_ = glm::translate(mModel_, position_);
	mModel_ = glm::scale(mModel_, scale_);

	glm::quat rotQuat = glm::quat(glm::radians(rotation_));
	mModel_ *= glm::toMat4(rotQuat);
	
	dirty_ = false;
}