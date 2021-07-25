#pragma once

#include "Scene.h"

class SceneObject;

class TestScene : public Scene {

private:
	
	SceneObject* so1;
	SceneObject* so2;

public:
	TestScene();
	void Update();
};