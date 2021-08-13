
#include "GameUI.h"
#include "Scene.h"
#include "LevelMap.h"

GameUI::GameUI(Scene* scene, LevelMap* map) :
	numKeys_(0)
{
	scene->AddTexture("ui.key", "Assets/Textures/keyUI.png");
	scene->AddTexture("ui.nokey", "Assets/Textures/noKeyUI.png");

	matKey_ = scene->AddMaterial("mat.ui.key", "Assets/Shaders/ui.hlsl", false);
	matKey_->SetTexture(0, "ui.key");
	matNoKey_ = scene->AddMaterial("mat.ui.key", "Assets/Shaders/ui.hlsl", false);
	matNoKey_->SetTexture(0, "ui.nokey");

	int pickups = (int)map->GetPickups().size();
	for (int i = 0; i < pickups; ++i) {
		std::string id = "uiKey";
		id += std::to_string(i);
		SceneObject* uiso = scene->AddUIElement(id);
		uiso->GetTransform()->SetLocalScale({ 50, 50, 1 });
		uiso->GetTransform()->SetLocalPosition({ 20 + 50 * (i + 1), 650, 1 });
		MeshRendererComponent* uiMR = uiso->GetComponent<MeshRendererComponent>(); // NOT GREAT UIRenderer maybe?
		uiMR->SetMaterial(matNoKey_);
		keyRenderers_.push_back(uiMR);
	}

	BIND_OBSERVER(OnPickupOverlap, LevelTile);
}


GameUI::~GameUI() {
	FREE_OBSERVER(OnPickupOverlap);
}

void GameUI::OnPickupOverlapCallback(LevelTile tile) {
	(void)tile;
	keyRenderers_[numKeys_]->SetMaterial(matKey_);
	++numKeys_;
}