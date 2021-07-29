#pragma once

#include "Scene.h"
#include <memory>

class SceneObject;
class LevelMap;

class TestScene : public Scene {

private:
	
	std::unique_ptr<LevelMap> levelMap_;

public:
	TestScene();
	void Update();
};