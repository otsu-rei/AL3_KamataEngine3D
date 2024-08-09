#include "SceneManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SceneManager class methods
////////////////////////////////////////////////////////////////////////////////////////////



void SceneManager::Init(std::unique_ptr<IScene>& startScene) { scene_ = std::move(startScene); }

void SceneManager::Term() {}

void SceneManager::Update() {

	if (scene_->GetNextScene()) { //!< 次のsceneがある場合
		scene_ = std::move(scene_->GetNextScene());
	}

	scene_->Update();
}

void SceneManager::DrawBackSprite() { scene_->DrawBackSprite(); }

void SceneManager::Draw3D() { scene_->Draw3D(); }

void SceneManager::DrawLine() { scene_->DrawLine(); }

void SceneManager::DrawFrontSprite() { scene_->DrawFrontSprite(); }
