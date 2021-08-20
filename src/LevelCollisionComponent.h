#pragma once

#include "Component.h"
#include "Observer.h"
#include <vector>
#include <glm/glm.hpp>

struct LevelTile;
struct LevelEnemy;
class LevelMap;

class LevelCollisionComponent : public Component
{
public:

	COMPONENT_BASE(LevelCollisionComponent)
		
	void Update();

	bool IsOutOfBounds(glm::vec3 pos);

	void BindToMap(LevelMap* map);

	std::shared_ptr<Subject<LevelTile>> OnPickupOverlap;
	std::shared_ptr<Subject<LevelEnemy>> OnEnemyOverlap;
	std::shared_ptr<Subject<LevelTile>> OnExitOverlap;
private:
	glm::vec3 lastPos_;
	LevelMap* map_;
	
};