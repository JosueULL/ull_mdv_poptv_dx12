#include "SceneObject.h"
#include "Component.h"

Transform* Component::GetTransform() 
{
	return sceneObj_->GetTransform();
}
