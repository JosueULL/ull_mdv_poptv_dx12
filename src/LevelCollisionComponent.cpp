#include "LevelMap.h"
#include "Transform.h"
#include "LevelCollisionComponent.h"

LevelCollisionComponent::LevelCollisionComponent(SceneObject* owner) :
	Component(owner),
	OnPickupOverlap(std::make_shared<Subject<LevelTile>>()),
	OnEnemyOverlap(std::make_shared<Subject<LevelEnemy>>()),
	OnExitOverlap(std::make_shared<Subject<LevelTile>>())
{
	
}


bool LevelCollisionComponent::IsOutOfBounds(glm::vec3 pos)
{
	LevelTile mytile = LevelMap::WorldToLevel(pos);
	
	if (mytile.x < 0 || mytile.x > map_->GetWidth() || mytile.y < 0 || mytile.y > map_->GetHeight()) return true;

	for (const LevelTile& lt : map_->GetWalkable())
	{
		glm::vec3 tilePos = LevelMap::LevelToWorld(lt);
		float difX = abs(pos.x - tilePos.x);
		float difY = abs(pos.z - tilePos.z);
		if (difX < LEVEL_SCALE * 0.5f && difY < LEVEL_SCALE * 0.5f)
		{
			return false;
		}
	}

	return true;
}

void LevelCollisionComponent::Update()
{
	glm::vec3 pos = GetTransform()->GetLocalPosition();
	glm::vec3 disp = pos - lastPos_;
	disp.y = 0;
	if (glm::length(disp) > 0) 
	{
		disp = glm::normalize(disp);
		glm::vec3 colEndPos = pos + (disp * 0.75f);
		if (IsOutOfBounds(colEndPos))
		{
			if (!IsOutOfBounds(lastPos_)) {
				lastPos_.y = pos.y;
				GetTransform()->SetLocalPosition(lastPos_);
			}
		}
		else
		{
			lastPos_ = pos;
		}
	}

	for (const auto& p : map_->GetPickups()) {
		glm::vec3 tilePos = LevelMap::LevelToWorld(p);
		if (glm::distance(pos, tilePos) < 1) 
		{
			OnPickupOverlap->Notify(p);
		}
	}

	for (const auto& p : map_->GetEnemies()) {
		if (glm::distance(pos, p.pos) < 1)
		{
			OnEnemyOverlap->Notify(p);
		}
	}

	if (map_->IsExitUnlocked())
	{
		glm::vec3 exitPos = LevelMap::LevelToWorld(map_->GetExit());
		if (glm::distance(pos, exitPos) < 1) {
			OnExitOverlap->Notify(map_->GetExit());
		}
	}
}

void LevelCollisionComponent::BindToMap(LevelMap* map) {
	map_ = map;

	map->RegisterCollisionCallbacks(this);
}
