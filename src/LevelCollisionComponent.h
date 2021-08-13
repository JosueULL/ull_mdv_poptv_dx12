#pragma once

#include "Component.h"
#include "Observer.h"
#include <vector>
#include <glm/glm.hpp>

struct LevelTile;
class LevelMap;

class LevelCollisionComponent : public Component
{
public:

	COMPONENT_BASE(LevelCollisionComponent)
		
	void Update();

	bool IsOutOfBounds(glm::vec3 pos);

	void BindToMap(LevelMap* map);

	std::shared_ptr<Subject<LevelTile>> OnPickupOverlap;
private:
	glm::vec3 lastPos_;
	LevelMap* map_;
	
};