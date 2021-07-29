#include "TestScene.h"

#include "System.h"
#include "LevelMap.h"
#include "Keyboard.h"
#include "Material.h"
#include "SceneObject.h"
#include "MeshRendererComponent.h"
#include "InstancedMeshRendererComponent.h"
#include "FPCameraCtrlComponent.h"

TestScene::TestScene() : Scene()
{

	levelMap_.reset(new LevelMap(this, "Assets/Levels/level.txt"));

	// Resources --------------------------------
	//AddTexture("doom", "Assets/Textures/doom.jpg");
	//AddTexture("fire", "Assets/Textures/fire.jpg");
	AddTexture("checker", "Assets/Textures/checker.jpg");

	AddMesh("monkey", "Assets/Meshes/monkey.obj");
	//AddMesh("spaceship", "Assets/Meshes/spaceship.obj");
	
	Material* mat1 = AddMaterial("mat.checker", "Assets/Shaders/test.hlsl", false);
	mat1->SetTexture(0, "checker");

	SceneObject* so1 = AddObject("obj1");
	MeshRendererComponent* mrc = so1->AddComponent<MeshRendererComponent>();
	mrc->SetMesh("monkey");
	mrc->SetMaterial(mat1);
	so1->GetTransform()->SetLocalScale(glm::vec3(0.25f));
	so1->GetTransform()->SetLocalPosition({ 0,1,0 });

	/*
	// Objects --------------------------------
	so1 = AddObject("obj1");
	MeshRendererComponent* mrc = so1->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(BuiltInRes::Mesh::Cube);
	mrc->SetMaterial(mat1);
	so1->GetTransform()->SetLocalScale(glm::vec3(0.25f));

	so2 = AddObject("obj2");
	so2->GetTransform()->SetLocalScale(glm::vec3(0.5f));
	mrc = so2->AddComponent<MeshRendererComponent>();
	mrc->SetMesh("spaceship");
	mrc->SetMaterial(mat2);
	

	SceneObject* uiso = AddUIElement("uiLife");
	uiso->GetTransform()->SetLocalScale({200, 100, 1});
	uiso->GetTransform()->SetLocalPosition({ 200, 200, 1 });
	MeshRendererComponent* uiMR = uiso->GetComponent<MeshRendererComponent>(); // NOT GREAT
	uiMR->SetMaterial(mat1);
	*/

	Camera* cam = GetMainCamera();
	FPCameraCtrlComponent* fpCtrl = cam->GetSceneObject()->AddComponent<FPCameraCtrlComponent>();
	fpCtrl->SetGroundHeight(0.75f);
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
