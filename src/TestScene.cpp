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

TestScene::TestScene() : Scene(),
	pickups_(0)
{
	levelMap_.reset(new LevelMap(this, "Assets/Levels/level.txt"));
	gameUI_.reset(new GameUI(this, levelMap_.get()));

	AddTexture("checker", "Assets/Textures/checker.jpg");

	BIND_OBSERVER(OnEnemyOverlap, LevelEnemy);
	BIND_OBSERVER(OnPickupOverlap, LevelTile);
	BIND_OBSERVER(OnExitOverlap, LevelTile);

	Camera* cam = GetMainCamera();
	fpCamCtrl_ = cam->GetSceneObject()->AddComponent<FPCameraCtrlComponent>();
	fpCamCtrl_->SetGroundHeight(0.75f);
	cam->GetTransform()->SetLocalPosition(levelMap_->GetStart());
	cam->GetTransform()->SetLocalRotation({ 0, levelMap_->GetStartRotation(), 0 });

	LevelCollisionComponent* lvlCol = cam->GetSceneObject()->AddComponent<LevelCollisionComponent>();
	lvlCol->BindToMap(levelMap_.get());
	
	lvlCol->OnPickupOverlap->Attach(this->OnPickupOverlap);
	lvlCol->OnExitOverlap->Attach(this->OnExitOverlap);
	lvlCol->OnEnemyOverlap->Attach(this->OnEnemyOverlap);

	currentState_ = GameState::Playing;
}

TestScene::~TestScene() {
	FREE_OBSERVER(OnEnemyOverlap);
	FREE_OBSERVER(OnPickupOverlap);
	FREE_OBSERVER(OnExitOverlap);
}

void TestScene::OnEnemyOverlapCallback(LevelEnemy enemy) {
	currentState_ = GameState::Defeat;
	fpCamCtrl_->enabled = false;
	gameUI_.get()->SetState(currentState_);
}

void TestScene::OnPickupOverlapCallback(LevelTile tile) {
	(void)tile;
	gameUI_.get()->SetPickupTaken(pickups_);
	++pickups_;
	if (pickups_ == levelMap_.get()->GetPickups().size()) {
		levelMap_.get()->UnlockExit();
	}
}


void TestScene::OnExitOverlapCallback(LevelTile tile) {
	(void)tile;
	currentState_ = GameState::Victory;
	fpCamCtrl_->enabled = false;
	gameUI_.get()->SetState(currentState_);
}

void TestScene::Update()
{
	levelMap_->Update();
	Scene::Update();

	switch (currentState_) {
	case GameState::Playing:
	{
		
		break;
	}
	case GameState::Defeat: {
		if (System::instance().GetKeyboard()->IsKeyPressed(VK_SPACE)) 
		{
			currentState_ = GameState::Playing;
			fpCamCtrl_->GetTransform()->SetLocalPosition(levelMap_->GetStart());
			fpCamCtrl_->GetTransform()->SetLocalRotation({ 0, levelMap_->GetStartRotation(), 0 });
			fpCamCtrl_->enabled = true;
			gameUI_.get()->SetState(currentState_);
		}

		if (System::instance().GetKeyboard()->IsKeyPressed(VK_F1)) // CHEAT to carry on playing
		{
			currentState_ = GameState::Playing;
			fpCamCtrl_->enabled = true;
			gameUI_.get()->SetState(currentState_);
		}
		break;
	}
	}
}
