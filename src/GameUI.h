#pragma once

#include "Observer.h"
#include <vector>

class Scene;
class Material;
class LevelMap;
class LevelCollisionComponent;
class MeshRendererComponent;

struct LevelTile;

class GameUI {

public:
	GameUI(Scene* scene, LevelMap* map);
	~GameUI();

	OBSERVER(OnPickupOverlap, LevelTile)

private:
	int numKeys_;
	std::vector< MeshRendererComponent* > keyRenderers_;
	Material* matKey_;
	Material* matNoKey_;
};