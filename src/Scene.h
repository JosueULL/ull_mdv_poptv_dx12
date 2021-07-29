#pragma once

#include <vector>
#include <memory>

#include "SceneObject.h"
#include "SceneResourcesDesc.h"

class Camera;
class Texture;
class Mesh;
class Material;

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
	std::unique_ptr<Camera> mainCam_;
	std::unique_ptr<Camera> uiCam_;

	std::vector<std::unique_ptr<SceneObject>> objects_;
	std::vector<std::unique_ptr<SceneObject>> ui_;

public:
	Scene();
	virtual ~Scene();

	virtual void Update();

	SceneObject* AddObject(std::string id);
	SceneObject* AddUIElement(std::string id);
	Camera* AddMainCamera();
	Camera* AddUICamera();
	Texture* AddTexture(std::string id, std::string path);
	Mesh* AddMesh(std::string id, std::string path);
	Material* AddMaterial(std::string id, std::string shaderpath, bool instancing);
	InstanceBufferDef* AddInstanceBuffer(std::string id, void* data, int count, int size);

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
	template<class T>
	std::vector<T*> GetAllUIComponents() {
		std::vector<T*> list;
		for (auto const& obj : ui_)
		{
			T* c = obj.get()->GetComponent<T>();
			if (c != nullptr) {
				list.push_back(c);
			}
		}
		return list;
	}

	Camera* GetMainCamera() { return mainCam_.get(); }
	Camera* GetUICamera() { return uiCam_.get(); }
};