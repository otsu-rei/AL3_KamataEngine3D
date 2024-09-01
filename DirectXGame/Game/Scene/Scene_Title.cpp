#include "Scene_Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* other scene access
#include <Scene_Game.h>

// engine
#include <TextureManager.h>
#include <WinApp.h>
#include <PrimitiveDrawer.h>
#include <AxisIndicator.h>

// Game
#include <ColliderManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Init() {

	ColliderManager::GetInstance()->Init();

	viewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	//AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	/*model_.reset(Model::CreateFromOBJ("Title"));
	transform_.Initialize();*/
	/*transform_.scale_ = {2.0f, 2.0f, 2.0f};*/
	/*transform_.UpdateMatrix();*/

	field_ = std::make_unique<Field>();
	field_->Init();

	player_ = std::make_unique<Player>();
	player_->Init();

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();
	gameCamera_->SetPlayer(player_.get());
	player_->SetGameCamera(gameCamera_.get());

	models_ = std::make_unique<TitleModels>();
	models_->Init();
}

void Scene_Title::Term() {}

void Scene_Title::Update() {

	XINPUT_STATE state = {};
	XINPUT_STATE preState = {};

	if (input_->GetJoystickState(0, state) && input_->GetJoystickStatePrevious(0, preState)) {
		bool isTriggerRight
			= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
		
		if (isTriggerRight) {
			SetNextScene<Scene_Game>();
		}

	} else if (input_->TriggerKey(DIK_SPACE)) {
		SetNextScene<Scene_Game>();
	}

	player_->Update();

	gameCamera_->Update();
	viewProjection_.matView       = gameCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = gameCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	models_->Update(gameCamera_.get());

	ColliderManager::GetInstance()->UpdateAllCollider();
	
}

void Scene_Title::DrawBackSprite() { }

void Scene_Title::Draw3D() {
	/*model_->Draw(transform_, viewProjection_);*/

	models_->Draw(viewProjection_);

	player_->Draw(viewProjection_);
	field_->Draw(viewProjection_);
}

void Scene_Title::DrawLine() {}

void Scene_Title::DrawFrontSprite() {}
