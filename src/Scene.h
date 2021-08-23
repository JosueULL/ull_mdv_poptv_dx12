#pragma once

#include <vector>
#include <memory>

#include "SceneObject.h"
#include "SceneResourcesDesc.h"

class Camera;
class Texture;
class Mesh;
class Material;


#define BUILTIN_MESH_QUAD "BuiltIn/Mesh/Quad"
#define BUILTIN_MESH_CUBE "BuiltIn/Mesh/Cube"

#define ID_MAINCAM_BUFFER "camBuffer.mainCamera";
#define ID_UICAM_BUFFER "camBuffer.uiCamera";

class Scene {
private:
	SceneResourcesDesc sceneRes_;

	void Initialize();
	void AddBuiltInResources();

protected:
	Camera* mainCam_;
	std::unique_ptr<Camera> uiCam_;

	std::vector<std::unique_ptr<SceneObject>> objects_;
	std::vector<std::unique_ptr<SceneObject>> ui_;

public:
	Scene();
	virtual ~Scene();

	virtual void Update();

	SceneObject* AddObject(std::string id);
	SceneObject* AddUIElement(std::string id);
	MeshRendererComponent* AddUISprite(std::string id, Material* mat, glm::vec2 pos, glm::vec2 size);
	Material* AddSpriteMaterial(std::string id, std::string texturePath);
	Camera* AddMainCamera();
	Camera* AddUICamera();
	Texture* AddTexture(std::string id, std::string path);
	Mesh* AddMesh(std::string id, std::string path);
	Material* AddMaterial(std::string id, std::string shaderpath, bool instancing, bool compileShader);
	InstanceBufferDef* AddInstanceBuffer(std::string id, void* data, int count, int size);

	const SceneResourcesDesc& GetResourcesDesc() { return sceneRes_; };

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

	Camera* GetMainCamera() { return mainCam_; }
	Camera* GetUICamera() { return uiCam_.get(); }
};