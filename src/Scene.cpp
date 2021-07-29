#include "Scene.h"

#include <fstream>
#include <string>

#include "MeshQuad.h"
#include "MeshCube.h"

#include "Texture.h"
#include "ConstantBufferDef.h"
#include "RenderStateDef.h"
#include "ImageIO.h"
#include "MeshIO.h"


const std::string BuiltInRes::Mesh::Quad = "BuiltIn/Mesh/Quad";
const std::string BuiltInRes::Mesh::Cube = "BuiltIn/Mesh/Cube";

 Scene::Scene() :
	 sceneRes_(),
	 objects_()
 {
	 AddBuiltInResources();

	 AddMainCamera();
	 AddUICamera();
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

 Camera* Scene::AddMainCamera() {
	 SceneObject* newObj = new SceneObject("mainCamera");
	 objects_.push_back(std::unique_ptr<SceneObject>(newObj));

	 Camera* cam = newObj->AddComponent<Camera>();
	 mainCam_.reset(cam);

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = "mainCamera";
	 gr.Data = cam->GetCBufferDef();
	 gr.Type = GraphicResourceDesc::ResourceType::ConstantBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return cam;
 }

 Camera* Scene::AddUICamera() {
	 uiCam_ = std::make_unique<Camera>(nullptr);
	 uiCam_->SetOrthographic();

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = "uiCamera";
	 gr.Data = uiCam_->GetCBufferDef();
	 gr.Type = GraphicResourceDesc::ResourceType::ConstantBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return uiCam_.get();
 }


 Texture* Scene::AddTexture(std::string id, std::string path) {
	 GraphicResourceDesc d2 = GraphicResourceDesc();
	 Texture* texture = new Texture();
	 texture->data = LoadImageFromFile(path.c_str(), 1, &texture->width, &texture->height);
	 d2.Id = id;
	 d2.Data = texture;
	 d2.Type = GraphicResourceDesc::ResourceType::Texture;
	 sceneRes_.Graphics.push_back(d2);
	 return texture;
 }

 Material* Scene::AddMaterial(std::string id, std::string path, bool instancing) 
 {
	 GraphicResourceDesc shaderGR = GraphicResourceDesc();
	 RenderStateDef* shader = new RenderStateDef({ path, instancing });
	 shaderGR.Id = path;
	 shaderGR.Data = shader;
	 shaderGR.Type = GraphicResourceDesc::ResourceType::RenderState;
	 sceneRes_.Graphics.push_back(shaderGR);

	 GraphicResourceDesc matGR = GraphicResourceDesc();
	 Material* mat = new Material();
	 mat->SetShader(path);
	 matGR.Id = id;
	 matGR.Data = mat;
	 matGR.Type = GraphicResourceDesc::ResourceType::Material;
	 sceneRes_.Graphics.push_back(matGR);

	 return mat;
 }


 SceneObject* Scene::AddUIElement(std::string id) {
	 SceneObject* newObj = new SceneObject(id);
	 ui_.push_back(std::unique_ptr<SceneObject>(newObj));

	 MeshRendererComponent* mr = newObj->AddComponent<MeshRendererComponent>();
	 mr->SetMesh(BuiltInRes::Mesh::Quad);

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = id;
	 gr.Data = newObj->GetCBufferDef();
	 gr.Type = GraphicResourceDesc::ResourceType::ConstantBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return newObj;
 }


 Mesh* Scene::AddMesh(std::string id, std::string path) {
	 GraphicResourceDesc d2 = GraphicResourceDesc();
	 Mesh* mesh = LoadObjMeshFromFile(path.c_str());
	 d2.Id = id;
	 d2.Data = mesh;
	 d2.Type = GraphicResourceDesc::ResourceType::Mesh;
	 sceneRes_.Graphics.push_back(d2);
	 return mesh;
 }

 InstanceBufferDef* Scene::AddInstanceBuffer(std::string id, void* data, int count, int size) {
	 InstanceBufferDef* bufferDef = new InstanceBufferDef();
	 bufferDef->id = id;
	 bufferDef->ptr = data;
	 bufferDef->instanceCount = count;
	 bufferDef->size = size;

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = bufferDef->id;
	 gr.Data = bufferDef;
	 gr.Type = GraphicResourceDesc::ResourceType::InstanceBuffer;
	 sceneRes_.Graphics.push_back(gr);

	 return bufferDef;
 }
 
void Scene::Initialize()
{
	
}

void Scene::Update() {
	for (const auto& sceneObj : objects_) {
		sceneObj.get()->Update();
	}

	for (const auto& uiObj : ui_) {
		uiObj.get()->Update();
	}
}