#include "LevelMap.h"
#include "Transform.h"
#include "LevelCollisionComponent.h"

LevelCollisionComponent::LevelCollisionComponent(SceneObject* owner) :
	Component(owner),
	OnPickupOverlap(std::make_shared<Subject<LevelTile>>())
{
	
}


bool LevelCollisionComponent::IsOutOfBounds(glm::vec3 pos)
{
	int z = int(pos.x / LEVEL_SCALE);
	int x = -int(pos.z / LEVEL_SCALE);

	if (x < 0 || x > map_->GetWidth() || z < 0 || z > map_->GetHeight()) return true;

	for (const LevelTile& lt : map_->GetWalkable())
	{
		glm::vec3 tilePos = glm::vec3(lt.y * LEVEL_SCALE, 0, -lt.x * LEVEL_SCALE);
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

	for (const auto& p : map_->GetPickups()) {
		glm::vec3 tilePos = glm::vec3(p.y * LEVEL_SCALE, 0, -p.x * LEVEL_SCALE);
		if (glm::distance(pos, tilePos) < 1) 
		{
			OnPickupOverlap->Notify(p);
		}
	}

}

void LevelCollisionComponent::BindToMap(LevelMap* map) {
	map_ = map;

	map->RegisterCollisionCallbacks(this);
}
