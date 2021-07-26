#pragma once

#include <vector>
#include <memory>

#include "SceneObject.h"
#include "SceneResourcesDesc.h"

class Camera;

class BuiltInRes {
public:
	class Mesh {
	public:
		static const std::string Quad;
		static const std::string Cube;
	};
};

class Scene {
private:
	SceneResourcesDesc sceneRes_;

	void Initialize();
	void AddBuiltInResources();
protected:
	std::vector<std::unique_ptr<SceneObject>> objects_;

	SceneObject* AddObject(std::string id);
	Camera* AddCamera(std::string id);
	void AddTexture(std::string id, std::string path);
	void AddMesh(std::string id, std::string path);
	InstanceBufferDef* AddInstanceBuffer(std::string id, void* data, int count, int size);

public:
	Scene();
	virtual ~Scene();

	virtual void Update();

	const SceneResourcesDesc GetResourcesDesc() { return sceneRes_; };

	template<class T>
	std::vector<T*> GetAllComponents() {
		std::vector<T*> list;
		for (auto const& obj : objects_)
		{
			T* c = obj.get()->GetComponent<T>();
			if (c != nullptr) {
				list.push_back(c);
			}
		}
		return list;
	}

	Camera* GetMainCamera();
};