#include "TestScene.h"

#include "System.h"
#include "LevelMap.h"
#include "Keyboard.h"
#include "Material.h"
#include "SceneObject.h"
#include "MeshRendererComponent.h"
#include "InstancedMeshRendererComponent.h"
#include "FPCameraCtrlComponent.h"
#include "LevelCollisionComponent.h"

TestScene::TestScene() : Scene()
{

	levelMap_.reset(new LevelMap(this, "Assets/Levels/level.txt"));

	// Resources --------------------------------
	//AddTexture("noise", "Assets/Textures/noise.jpg");
	//AddTexture("doom", "Assets/Textures/doom.jpg");
	//AddTexture("fire", "Assets/Textures/fire.jpg");
	AddTexture("checker", "Assets/Textures/checker.jpg");
	
	
	Material* mat1 = AddMaterial("mat.checker", "Assets/Shaders/baseTexture.hlsl", false);
	mat1->SetTexture(0, "checker");
	

	/*
	AddMesh("skull", "Assets/Meshes/skull.obj");
	//AddMesh("spaceship", "Assets/Meshes/spaceship.obj");

	
	
	// Objects --------------------------------
	so1 = AddObject("obj1");
	MeshRendererComponent* mrc = so1->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(BuiltInRes::Mesh::Cube);
	mrc->SetMaterial(mat1);
	so1->GetTransform()->SetLocalScale(glm::vec3(0.25f));

	so2 = AddObject("obj2");
	so2->GetTransform()->SetLocalScale(glm::vec3(0.5f));
	mrc = so2->AddCompwonent<MeshRendererComponent>();
	mrc->SetMesh("spaceship");
	mrc->SetMaterial(mat2);
	*/

	/*
	SceneObject* so1 = AddObject("keytest");
	MeshRendererComponent* mrc = so1->AddComponent<MeshRendererComponent>();
	mrc->SetMesh("key");
	mrc->SetMaterial(mat2);
	so1->GetTransform()->SetLocalScale(glm::vec3(0.0015f));
	so1->GetTransform()->SetLocalPosition({ 10,0.25,-8 });
	*/

	SceneObject* uiso = AddUIElement("uiLife");
	uiso->GetTransform()->SetLocalScale({200, 100, 1});
	uiso->GetTransform()->SetLocalPosition({ 200, 200, 1 });
	MeshRendererComponent* uiMR = uiso->GetComponent<MeshRendererComponent>(); // NOT GREAT UIRenderer maybe?
	uiMR->SetMaterial(mat1);
	

	Camera* cam = GetMainCamera();
	FPCameraCtrlComponent* fpCtrl = cam->GetSceneObject()->AddComponent<FPCameraCtrlComponent>();
	fpCtrl->SetGroundHeight(0.75f);
	cam->GetTransform()->SetLocalPosition(levelMap_->GetStart());
	cam->GetTransform()->SetLocalRotation({ 0, levelMap_->GetStartRotation(), 0 });

	LevelCollisionComponent* lvlCol = cam->GetSceneObject()->AddComponent<LevelCollisionComponent>();
	lvlCol->BindToMap(levelMap_.get());
	
}

void TestScene::Update()
{
	// TEST ----
	static int counter = 0;
	counter++;
	//float scale = std::sin(static_cast<float> (counter) / 128.0f);
	//so1->GetTransform()->SetLocalScale(glm::vec3(scale));
	//so1->GetTransform()->SetLocalPosition(glm::vec3(scale, 0, 1));
	//so1->GetTransform()->SetLocalRotation(glm::vec3(scale * 360.0f, scale * 360.0f, scale * 360.0f));
	//so1->GetTransform()->SetLocalRotation(glm::vec3(-scale * 90.0f, 0, 0));

	

	static float x = 0;
	
	//so2->GetTransform()->SetLocalPosition(glm::vec3(x, 0, 1));
	//so2->GetTransform()->SetLocalRotation(glm::vec3(0, scale * 360.0f, 0));
	//so2->GetTransform()->SetLocalRotation(glm::vec3(0, 0, -scale * 180.0f));
	// END TEST ----


	Scene::Update();
}
