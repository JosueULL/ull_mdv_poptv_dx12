#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Transform.h"
#include "Component.h"
#include "ConstantBufferDef.h"

// Components ---- (needed for templates... find a solution?)
#include "Camera.h"
#include "MeshRendererComponent.h"
#include "LevelCollisionComponent.h"

class SceneObject {

public:
	struct ConstantBuffer
	{
		glm::mat4 model;
	};

	explicit SceneObject(std::string id);
	SceneObject(const SceneObject&) = delete;
	~SceneObject();

	void Update();

	std::string GetId() {
		return id_;
	}

	ConstantBufferDef* GetCBufferDef() {
		return cBufferDef_;
	}

	ConstantBuffer* GetCBuffer() {
		return cBuffer_;
	}

	Transform* GetTransform() {
		return transform_.get();
	}

	template <class T>
	T* AddComponent() 
	{
		Component* newCmp = new T(this);
		AddComponent(newCmp);
		return static_cast<T*>(newCmp);
	}

	void AddComponent(Component* newCmp) {
		std::unique_ptr<Component> component = std::unique_ptr<Component>(newCmp);
		components_.push_back(std::move(component));
	}

	template <class T>
	T* GetComponent() {
		for (auto const& c : components_) {
			Component* cPtr = c.get();
			T* t = dynamic_cast<T*>(cPtr);
			if (t != nullptr) 
			{
				return t;
			}
		}
		return nullptr;
	}

private:
	std::unique_ptr<Transform> transform_;
	ConstantBuffer* cBuffer_;
	ConstantBufferDef* cBufferDef_;
	std::vector<std::unique_ptr<Component>> components_;
	std::string id_;
};