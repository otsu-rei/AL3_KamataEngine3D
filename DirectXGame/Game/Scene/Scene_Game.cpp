#include "Scene_Game.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* other scene
#include <Scene_Title.h>

#include <AxisIndicator.h>
#include <PrimitiveDrawer.h>

#include "ColliderManager.h"
#include "imgui.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Game class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Init() {

	ColliderManager::GetInstance()->Init();

	viewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	//AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(200.0f);

	field_ = std::make_unique<Field>();
	field_->Init();

	stageManager_ = std::make_unique<StageManager>();

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	stageEditor_ = std::make_unique<StageEditer>();
	stageEditor_->Init(gameCamera_.get());
	stageEditor_->LoadJson(stageManager_->GetStageFilename());
	
	gameCamera_->SetPlayer(stageEditor_->GetPlayer());
}

void Scene_Game::Term() {}

void Scene_Game::Update() {

#ifdef _DEBUG

	ImGui::Begin("main");

	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);

	gameCamera_->UpdateImGui();
	stageEditor_->UpdateImGui();

	ImGui::End();

#endif

	XINPUT_STATE state    = {};
	XINPUT_STATE preState = {};

	// stage切り替え
	if (input_->GetJoystickState(0, state) && input_->GetJoystickStatePrevious(0, preState)) {
		
		bool isTriggerLeft = state.Gamepad.wButtons & ~preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		
		if (isTriggerLeft) { //!< pre
			if (stageManager_->IsPreStage()) {
				// preStageに移行
				stageManager_->SetPreStageNum();
				stageEditor_->LoadJson(stageManager_->GetStageFilename());

			} else {
				// Titleに移行
				nextScene_ = std::make_unique<Scene_Title>();
				nextScene_->Init();
			}
		}

		bool isTriggerUp = state.Gamepad.wButtons & ~preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;

		if (isTriggerUp) { //!< reset
			stageEditor_->LoadJson(stageManager_->GetStageFilename());
		}

		bool isTriggerRight = state.Gamepad.wButtons & ~preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

		if (isTriggerRight) { //!< next
			if (stageManager_->IsNextStage()) {
				// preStageに移行
				stageManager_->SetNextStageNum();
				stageEditor_->LoadJson(stageManager_->GetStageFilename());

			}
		}
	}

	

	stageEditor_->Update();

	//!< カメラの更新処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

	} else { //!< GameCamera
		gameCamera_->Update();
		viewProjection_.matView = gameCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = gameCamera_->GetViewProjection().matProjection;
	}

	viewProjection_.TransferMatrix();

	ColliderManager::GetInstance()->UpdateAllCollider();

	stageEditor_->ReactionUpdate();

#ifdef _DEBUG

	ImGui::Begin("debug variables");

	ImGui::End();

#endif

}

void Scene_Game::DrawBackSprite() {
}

void Scene_Game::Draw3D() {
	field_->Draw(viewProjection_);
	stageEditor_->Draw(viewProjection_);
	
}

void Scene_Game::DrawLine() { }

void Scene_Game::DrawFrontSprite() {}
