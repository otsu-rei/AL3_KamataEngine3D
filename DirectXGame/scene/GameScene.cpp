#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include <cassert>

//-----------------------------------------------------------------------------------------
// adapter include
//-----------------------------------------------------------------------------------------
#include <sstream>
#include <fstream>

// engine
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"

#include "Grid.h"
#include "MyMath.h"


GameScene::GameScene() {}

GameScene::~GameScene() {
}	


void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// viewProjection
	viewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(200.0f);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	// models
	cubeModel_.reset(Model::Create());
	skydomeModel_.reset(Model::CreateFromOBJ("skydome", true));
	groundModel_.reset(Model::CreateFromOBJ("ground"));

	headModel_.reset(Model::CreateFromOBJ("chara_head"));
	bodyModel_.reset(Model::CreateFromOBJ("chara_body"));
	lArmModel_.reset(Model::CreateFromOBJ("chara_lArm"));
	rArmModel_.reset(Model::CreateFromOBJ("chara_rArm"));

	// player
	playerTextureHandle_ = TextureManager::Load("uvChecker.png");
	TextureManager::Load("reticle.png"); //!< レティクル画像(仮)

	player_ = std::make_unique<Player>();
	/*player_->Init(playerModel_.get(), {0.0f, 0.0f, 30.0f});*/
	player_->Init(headModel_.get(), bodyModel_.get(), lArmModel_.get(), rArmModel_.get(), {0.0f, 0.0f, 30.0f});
	player_->SetGameScene(this);
	player_->SetViewProj(&followCamera_->GetViewProjection());

	// 追従対象の設定
	followCamera_->SetTarget(&player_->GetWorldTransform());

	// skydome
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init(skydomeModel_.get());

	// ground
	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel_.get());
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("main");

	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);
	player_->SetOnImGui();

	ImGui::End();

#endif // _DEBUG

	//!< カメラの更新処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

	} else { //!< GameCamera
		followCamera_->Update();
		viewProjection_.matView       = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

		// FIXME: 自機の更新処理がカメラの更新処理よりしたにいるので1frameずれる
	}

	viewProjection_.TransferMatrix();
	
	//!< 自機の更新処理
	player_->Update();

#ifdef _DEBUG

	ImGui::Begin("debug");
	ImGui::End();

#endif // _DEBUG

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	player_->Draw(viewProjection_);
	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}