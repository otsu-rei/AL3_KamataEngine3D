#include "Scene_Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <PrimitiveDrawer.h>
#include <AxisIndicator.h>

#include "ColliderManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Init() {

	viewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(200.0f);

	box1_ = std::make_unique<Box>();
	box1_->Init();

	player_ = std::make_unique<Player>();
	player_->Init();
	
}

void Scene_Title::Term() {}

void Scene_Title::Update() {

	//!< カメラの更新処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

	} else { //!< GameCamera
	}

	viewProjection_.TransferMatrix();

	player_->Update();
	
	if (input_->TriggerKey(DIK_SPACE)) { //!< test scene
		nextScene_ = std::make_unique<Scene_Title>();
		nextScene_->Init();
	}

	ColliderManager::GetInstance()->Update();
}

void Scene_Title::DrawBackSprite() {}

void Scene_Title::Draw3D() {
	box1_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
}

void Scene_Title::DrawLine() {}

void Scene_Title::DrawFrontSprite() {}
