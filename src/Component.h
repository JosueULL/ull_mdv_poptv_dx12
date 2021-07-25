#pragma once

#include <memory>

#define COMPONENT_BASE(name) \
	name() = delete; \
	name(const name&) = delete; \
	explicit name(SceneObject* owner); \

class SceneObject;
class Transform;

class Component {
private:
	SceneObject* sceneObj_;
public:

	Component() = delete;
	virtual ~Component() {};
	Component(const Component&) = delete;

	explicit Component(SceneObject* owner) :
		sceneObj_(owner)
	{

	}

	SceneObject* GetSceneObject() { return sceneObj_; }
	Transform* GetTransform();

	virtual void Update() 
	{
		// Nothing
	}
};