#include "Common.h"
#include "SceneObject.h"
#include <cmath>

SceneObject::SceneObject(std::string id) :
	transform_(std::make_unique<Transform>()),
	cBuffer_(new ConstantBuffer()),
	cBufferDef_(new ConstantBufferDef()),
	components_(),
	id_(id)
{
	cBuffer_->model = transform_->GetModel();
	
	cBufferDef_->size = sizeof(SceneObject::ConstantBuffer);
	cBufferDef_->ptr = cBuffer_;
}

SceneObject::~SceneObject() {
	SAFE_FREE(cBuffer_);
}

void SceneObject::Update() 
{
	for (const auto& c : components_) 
	{
		if (c.get()->enabled)
			c.get()->Update();
	}

	cBuffer_->model = transform_->GetModel();
}

