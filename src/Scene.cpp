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
	 mainCam_ = nullptr;
	 objects_.clear();
 }

 void Scene::AddBuiltInResources() 
 {
	 // Quad Mesh
	 GraphicResourceDesc d = GraphicResourceDesc();
	 d.Id = BUILTIN_MESH_QUAD;
	 d.Data = new MeshQuad();
	 sceneRes_.Meshes.push_back(d);

	 // Cube Mesh
	 d = GraphicResourceDesc();
	 d.Id = BUILTIN_MESH_CUBE;
	 d.Data = new MeshCube();
	 sceneRes_.Meshes.push_back(d);
 }

 SceneObject* Scene::AddObject(std::string id) {
	 SceneObject* newObj = new SceneObject(id);
	 objects_.push_back(std::unique_ptr<SceneObject>(newObj));

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = id;
	 gr.Data = newObj->GetCBufferDef();
	 sceneRes_.ConstantBuffers.push_back(gr);

	 return newObj;
 }

 Camera* Scene::AddMainCamera() {
	 SceneObject* newObj = AddObject("mainCamera");
	 Camera* cam = newObj->AddComponent<Camera>();
	 mainCam_ = cam;

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = ID_MAINCAM_BUFFER;
	 gr.Data = cam->GetCBufferDef();
	 sceneRes_.ConstantBuffers.push_back(gr);

	 return cam;
 }

 Camera* Scene::AddUICamera() {
	 uiCam_ = std::make_unique<Camera>(nullptr);
	 uiCam_->SetOrthographic();

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = ID_UICAM_BUFFER;
	 gr.Data = uiCam_->GetCBufferDef();
	 sceneRes_.ConstantBuffers.push_back(gr);

	 return uiCam_.get();
 }

 Texture* Scene::AddTexture(std::string id, std::string path) {
	 GraphicResourceDesc d2 = GraphicResourceDesc();
	 Texture* texture = new Texture();
	 texture->data = LoadImageFromFile(path.c_str(), 1, &texture->width, &texture->height);
	 d2.Id = id;
	 d2.Data = texture;
	 sceneRes_.Textures.push_back(d2);
	 return texture;
 }

 Material* Scene::AddMaterial(std::string id, std::string path, bool instancing, bool compileShader) 
 {
	 GraphicResourceDesc shaderGR = GraphicResourceDesc();
	 RenderStateDef* shader = new RenderStateDef({ path, instancing, compileShader });
	 shaderGR.Id = path;
	 shaderGR.Data = shader;
	 sceneRes_.RenderStates.push_back(shaderGR);

	 GraphicResourceDesc matGR = GraphicResourceDesc();
	 Material* mat = new Material();
	 mat->SetShader(path);
	 matGR.Id = id;
	 matGR.Data = mat;
	 sceneRes_.Materials.push_back(matGR);

	 return mat;
 }


 SceneObject* Scene::AddUIElement(std::string id) {
	 SceneObject* newObj = new SceneObject(id);
	 ui_.push_back(std::unique_ptr<SceneObject>(newObj));

	 MeshRendererComponent* mr = newObj->AddComponent<MeshRendererComponent>();
	 mr->SetMesh(BUILTIN_MESH_QUAD);

	 GraphicResourceDesc gr = GraphicResourceDesc();
	 gr.Id = id;
	 gr.Data = newObj->GetCBufferDef();
	 sceneRes_.ConstantBuffers.push_back(gr);

	 return newObj;
 }

 
 MeshRendererComponent* Scene::AddUISprite(std::string id, Material* mat, glm::vec2 pos, glm::vec2 size) {
	 
	 SceneObject* uiso = AddUIElement(id);
	 uiso->GetTransform()->SetLocalScale({ size.x, size.y, 1 });
	 uiso->GetTransform()->SetLocalPosition({ pos.x, pos.y, 1 });

	 MeshRendererComponent* uiMR = uiso->GetComponent<MeshRendererComponent>();
	 uiMR->SetMaterial(mat);

	 return uiMR;
 }

 Material* Scene::AddSpriteMaterial(std::string idBase, std::string texturePath) {

	 std::string texId = "tex.ui." + idBase;
	 AddTexture(texId, texturePath);
	 Material* mat = AddMaterial("mat.ui."+idBase, "Assets/Shaders/ui", false, false);
	 mat->SetTexture(0, texId);
	 return mat;
 }

 Mesh* Scene::AddMesh(std::string id, std::string path) {
	 GraphicResourceDesc d2 = GraphicResourceDesc();
	 Mesh* mesh = LoadObjMeshFromFile(path.c_str());
	 d2.Id = id;
	 d2.Data = mesh;
	 sceneRes_.Meshes.push_back(d2);
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
	 sceneRes_.InstanceBuffers.push_back(gr);

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