#pragma once

#include "Observer.h"
#include "glm/glm.hpp"
#include <vector>

class Scene;
class Material;
class LevelMap;
class LevelCollisionComponent;
class MeshRendererComponent;

struct LevelTile;

enum class GameState;

class GameUI {

public:
	GameUI(Scene* scene, LevelMap* map);
	~GameUI();

	void SetState(GameState state);
	void SetPickupTaken(int index);
private:
	int numKeys_;
	std::vector< MeshRendererComponent* > keyRenderers_;
	Material* matKey_;
	Material* matNoKey_;

	MeshRendererComponent* gameOverSprite_;
	MeshRendererComponent* winSprite_;
};