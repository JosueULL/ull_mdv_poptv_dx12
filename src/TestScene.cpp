#include "TestScene.h"


#include "System.h"
#include "GameUI.h"
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
	gameUI_.reset(new GameUI(this, levelMap_.get()));

	// Resources --------------------------------
	//AddTexture("noise", "Assets/Textures/noise.jpg");
	//AddTexture("doom", "Assets/Textures/doom.jpg");
	//AddTexture("fire", "Assets/Textures/fire.jpg");
	AddTexture("checker", "Assets/Textures/checker.jpg");
	
	
	
	Material* mat1 = AddMaterial("mat.checker", "Assets/Shaders/baseTexture.hlsl", false);
	mat1->SetTexture(0, "checker");
	

	Camera* cam = GetMainCamera();
	FPCameraCtrlComponent* fpCtrl = cam->GetSceneObject()->AddComponent<FPCameraCtrlComponent>();
	fpCtrl->SetGroundHeight(0.75f);
	cam->GetTransform()->SetLocalPosition(levelMap_->GetStart());
	cam->GetTransform()->SetLocalRotation({ 0, levelMap_->GetStartRotation(), 0 });

	LevelCollisionComponent* lvlCol = cam->GetSceneObject()->AddComponent<LevelCollisionComponent>();
	lvlCol->BindToMap(levelMap_.get());
	
	lvlCol->OnPickupOverlap->Attach(gameUI_.get()->OnPickupOverlap);
}

void TestScene::Update()
{

	Scene::Update();
}
