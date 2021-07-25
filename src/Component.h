#pragma once

#include <memory>

class SceneObject;

class Component {
private:
	SceneObject* sceneObj_;
public:

	Component() = delete;
	virtual ~Component() {};
	Component(const Component&) = delete;

	Component(SceneObject* owner) :
		sceneObj_(owner)
	{

	}

	SceneObject* GetSceneObject() {
		return sceneObj_;
	}

	virtual void Update() 
	{
		// Nothing
	}
};