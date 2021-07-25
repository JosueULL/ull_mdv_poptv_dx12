#include "Scene.h"

#include <fstream>
#include <string>

#include "MeshQuad.h"
#include "MeshCube.h"

#include "Texture.h"
#include "ConstantBufferDef.h"
#include "ImageIO.h"
#include "RubyTexture.h"

const std::string BuiltInRes::Mesh::Quad = "BuiltIn/Mesh/Quad";
const std::string BuiltInRes::Mesh::Cube = "BuiltIn/Mesh/Cube";

 Scene::Scene() :
	 sceneRes_(),
	 objects_()
 {
	 AddBuiltInResources();
 }

 Scene::~Scene() {
	 sceneRes_.ReleaseAll();
	 objects_.clear();
 }

 void Scene::AddBuiltInResources() 
 {
	 // Quad Mesh
	 GraphicResourceDesc d = GraphicResourceDesc();
	 d.Id = BuiltInRes::Mesh::Quad;
	 d.Data = new MeshQuad();
	 d.Type = GraphicResourceDesc::ResourceType::Mesh;
	 sceneRes_.Graphics.push_back(d);


	 // Cube Mesh
	 d = GraphicResourceDesc();
	 d.Id = BuiltInRes::Mesh::Cube;
	 d.Data = new MeshCube();
	 d.Type = GraphicResourceDesc::ResourceType::Mesh;
	 sceneRes_.Graphics.push_back(d);
 }


 SceneObject* Scene::AddObject(std::string id) {
	 SceneObject* newObj = new SceneObject(id);
	 objects_.push_back(std::unique_ptr<SceneObject>(newObj));

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = id;
	 gr.Data = newObj->GetCBufferDef();
	 gr.Type = GraphicResourceDesc::ResourceType::ConstantBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return newObj;
 }

 Camera* Scene::AddCamera(std::string id) {
	 SceneObject* newObj = new SceneObject(id);
	 objects_.push_back(std::unique_ptr<SceneObject>(newObj));

	 Camera* cam = newObj->AddComponent<Camera>();

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = id;
	 gr.Data = cam->GetCBufferDef();
	 gr.Type = GraphicResourceDesc::ResourceType::ConstantBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return cam;
 }


 void Scene::AddTexture(std::string id, std::string path) {
	 GraphicResourceDesc d2 = GraphicResourceDesc();
	 Texture* texture = new Texture();
	 texture->data = LoadImageFromFile(path.c_str(), 1, &texture->width, &texture->height);
	 d2.Id = id;
	 d2.Data = texture;
	 d2.Type = GraphicResourceDesc::ResourceType::Texture;
	 sceneRes_.Graphics.push_back(d2);
 }
 
 Camera* Scene::GetMainCamera() {
	 // TODO - Cache this somehow
	 for (auto const& obj : objects_)
	 {
		 Camera* c = obj.get()->GetComponent<Camera>();
		 if (c != nullptr) {
			 return c;
		 }
	 }
	 return nullptr;
 }

void Scene::Initialize()
{
	
}

void Scene::Update() {
	for (const auto& sceneObj : objects_) {
		sceneObj.get()->Update();
	}
}