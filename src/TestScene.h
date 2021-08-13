#pragma once

#include "Scene.h"
#include <memory>

class SceneObject;
class LevelMap;
class GameUI;

class TestScene : public Scene {

private:
	
	std::unique_ptr<LevelMap> levelMap_;
	std::unique_ptr<GameUI> gameUI_;
public:
	TestScene();
	void Update();
};