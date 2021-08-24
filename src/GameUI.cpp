
#include "GameUI.h"
#include "TestScene.h"
#include "Scene.h"
#include "System.h"
#include "LevelMap.h"
#include "Window.h"

GameUI::GameUI(Scene* scene, LevelMap* map) :
	numKeys_(0)
{
	
	matKey_ = scene->AddSpriteMaterial("key", "Assets/Textures/keyUI.png");
	matNoKey_ = scene->AddSpriteMaterial("noKey", "Assets/Textures/noKeyUI.png");
	
	int pickups = (int)map->GetPickups().size();
	for (int i = 0; i < pickups; ++i) {
		std::string id = "uiKey";
		id += std::to_string(i);

		MeshRendererComponent* uiMR = scene->AddUISprite(id, matNoKey_, {20 + 50 * (i + 1), 600}, { 50, 50 });
		keyRenderers_.push_back(uiMR);
	}

	int screenw = System::instance().GetWindow()->GetWidth();
	int screenh = System::instance().GetWindow()->GetHeight();

	Material* matKeysText = scene->AddSpriteMaterial("text.ui.keys", "Assets/Textures/keys.png");
	scene->AddUISprite("ui.keys.text", matKeysText, { 80, 670 }, { 94*0.5f, 50*0.5f });

	Material* matGameOver = scene->AddSpriteMaterial("text.ui.gameOver", "Assets/Textures/youDied.png");
	gameOverSprite_ = scene->AddUISprite("ui.gameOver.text", matGameOver, { screenw * 0.5f, screenh * 0.5f - 50 }, { 580 * 0.5f, 100 * 0.5f });
	gameOverSprite_->enabled = false;

	Material* matWin = scene->AddSpriteMaterial("text.ui.win", "Assets/Textures/youEscaped.png");
	winSprite_ = scene->AddUISprite("ui.win.text", matWin, { screenw * 0.5f, screenh * 0.5f - 50 }, { 580 *0.5f, 100 * 0.5f });
	winSprite_->enabled = false;
}

GameUI::~GameUI() {
}

void GameUI::SetPickupTaken(int pickupIndex) {
	keyRenderers_[pickupIndex]->SetMaterial(matKey_);
}

void GameUI::SetState(GameState state) {
	switch (state) {
	case GameState::Playing: {
		winSprite_->enabled = false;
		gameOverSprite_->enabled = false;
		break;
	}
	case GameState::Defeat: {
		winSprite_->enabled = false;
		gameOverSprite_->enabled = true;
		break;
	}
	case GameState::Victory: {
		gameOverSprite_->enabled = false;
		winSprite_->enabled = true;
		break;
	}
	}
	
}

