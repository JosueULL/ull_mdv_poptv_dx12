#pragma once

#include "Observer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "InstancedMeshRendererComponent.h"

#define LEVEL_SCALE 2.0f

#define NOTHING		'#'
#define WALKABLE	' '

#define DEADEND_0	'T'
#define DEADEND_90	'E'
#define DEADEND_180 'L'
#define DEADEND_270 'J'

#define CORRIDOR_0	'|'
#define CORRIDOR_90	'-'

#define CORNER_0	'p'
#define CORNER_90	'b'
#define CORNER_180	'd'
#define CORNER_270	'q'

#define SIDEWALL_0		'^'
#define SIDEWALL_90		'<'
#define SIDEWALL_180	'_'
#define SIDEWALL_270	'>'


class Scene;
class LevelCollisionComponent;

struct LevelTile {
	int x;
	int y;
};

class LevelMap
{

public:
	LevelMap(Scene* scene, std::string levelPath);
	~LevelMap();

	glm::vec3 GetStart() const { return start_; }
	int GetStartRotation() const { return startRot_; }
	std::vector<LevelTile> GetWalkable() const { return walkable_; }
	std::vector<LevelTile> GetPickups() const { return pickups_; }
	int GetWidth() { return width_; }
	int GetHeight() { return height_; }

	void RegisterCollisionCallbacks(LevelCollisionComponent* col);
	
private:
	glm::vec3 start_;
	int startRot_;
	int width_;
	int height_;

	void UpdatePickups();

	std::vector<LevelTile> walkable_;
	std::unordered_map<char, std::vector<LevelTile>> pieces_;
	std::vector<LevelTile> pickups_;
	std::unordered_map<char, InstancedMeshRendererComponent::InstanceData*> buffers_;
	
	InstancedMeshRendererComponent::InstanceData* pickupsBuffer_;

	Listener<LevelTile>* OnPickupOverlapped;
	void OnPickupOverlappedCallback(LevelTile tile);
};