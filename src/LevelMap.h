#pragma once

#include "Common.h"
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
#define DEADEND_90	'J'
#define DEADEND_180 'L'
#define DEADEND_270 'E'

#define CORRIDOR_0	'|'
#define CORRIDOR_90	'-'

#define CORNER_0	'p'
#define CORNER_90	'q'
#define CORNER_180	'd'
#define CORNER_270	'b'

#define SIDEWALL_0		'^'
#define SIDEWALL_90		'>'
#define SIDEWALL_180	'_'
#define SIDEWALL_270	'<'


class Scene;
class LevelCollisionComponent;
class MeshRendererComponent;

struct LevelTile {
	int x;
	int y;

	bool operator==(const LevelTile& other) const {
		return x == other.x && y == other.y;
	}
};

struct LevelEnemy {
	glm::vec3 pos;
	int fromIndex;
	int toIndex;
	std::vector<LevelTile> path;
	float yaw;
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
	std::vector<LevelEnemy> GetEnemies() const { return enemies_; }
	LevelTile GetExit() const { return exit_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	bool IsExitUnlocked() const { return exitUnlocked_; }
	void UnlockExit();

	void Update();
	void RegisterCollisionCallbacks(LevelCollisionComponent* col);

	static glm::vec3 LevelToWorld(int x, int y) {
		return glm::vec3(x * LEVEL_SCALE, 0, y * LEVEL_SCALE);
	}

	static glm::vec3 LevelToWorld(LevelTile tile) {
		return LevelToWorld(tile.x, tile.y);
	}

	static LevelTile WorldToLevel(glm::vec3 worldPos) {
		return { int(worldPos.x / LEVEL_SCALE), int(worldPos.z / LEVEL_SCALE) };
	}
	
private:
	glm::vec3 start_;
	int startRot_;
	LevelTile exit_;
	int exitRot_;
	int width_;
	int height_;
	bool exitUnlocked_;

	void ParseLevelFile(std::string path);
	void InstantiateLevelContent(Scene* scene);
	void UpdatePickups();
	void UpdateEnemies();

	std::vector<LevelTile> walkable_;
	std::unordered_map<char, std::vector<LevelTile>> pieces_;
	std::vector<LevelTile> pickups_;
	std::vector<LevelEnemy> enemies_;
	std::unordered_map<char, InstancedMeshRendererComponent::InstanceData*> buffers_;
	
	InstancedMeshRendererComponent::InstanceData* pickupsBuffer_;
	InstancedMeshRendererComponent::InstanceData* enemiesBuffer_;

	MeshRendererComponent* exitMR_;

	OBSERVER(OnPickupOverlap, LevelTile)
};