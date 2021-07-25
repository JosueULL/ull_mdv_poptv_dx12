#include "Transform.h"

void Transform::UpdateModel() {

	mModel_ = glm::identity<glm::mat4>();

	mModel_ = glm::translate(mModel_, position_);
	mModel_ = glm::scale(mModel_, scale_);

	UpdateRotation();
	mModel_ *= mRotation_;
	
	dirty_ = false;
}

void Transform::UpdateRotation()
{
	glm::quat rotQuat = glm::quat(glm::radians(rotation_));
	mRotation_ = glm::toMat4(rotQuat);
}
