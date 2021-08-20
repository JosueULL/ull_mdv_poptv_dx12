#pragma once

#include "Scene.h"
#include <memory>

class FPCameraCtrlComponent;
class SceneObject;
class LevelMap;
class GameUI;

struct LevelEnemy;


enum class GameState {
	Playing,
	Defeat,
	Victory
};

class TestScene : public Scene {
public:
	TestScene();
	~TestScene();
	void Update();

private:
	int pickups_;
	std::unique_ptr<LevelMap> levelMap_;
	std::unique_ptr<GameUI> gameUI_;
	GameState currentState_;
	FPCameraCtrlComponent* fpCamCtrl_;

	OBSERVER(OnEnemyOverlap, LevelEnemy)
	OBSERVER(OnPickupOverlap, LevelTile)
	OBSERVER(OnExitOverlap, LevelTile)
};