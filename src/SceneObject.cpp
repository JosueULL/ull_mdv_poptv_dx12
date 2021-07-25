#include "SceneObject.h"
#include <cmath>

SceneObject::SceneObject(std::string id) :
	transform_(std::make_unique<Transform>()),
	cBuffer_(std::make_unique<ConstantBuffer>()),
	cBufferDef_(std::make_unique<ConstantBufferDef>()),
	components_(),
	id_(id)
{
	cBuffer_.get()->model = transform_->GetModel();
	ConstantBufferDef* cbd = cBufferDef_.get();
	cbd->size = sizeof(SceneObject::ConstantBuffer);
	cbd->ptr = cBuffer_.get();
}

void SceneObject::Update() 
{

	for (const auto& c : components_) 
	{
		c.get()->Update();
	}

	cBuffer_.get()->model = transform_->GetModel();
}

