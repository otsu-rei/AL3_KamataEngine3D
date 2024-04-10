#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include <cassert>

//-----------------------------------------------------------------------------------------
// adapter include
//-----------------------------------------------------------------------------------------
// engine
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"

#include "Grid.h"


GameScene::GameScene() {}

GameScene::~GameScene() {

	// game 
	player_.reset();
	enemy_.reset();

	cubeModel_.reset();

	debugCamera_.reset();

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	cubeModel_.reset(Model::Create());

	// player
	playerTextureHandle_ = TextureManager::Load("uvChecker.png");

	player_ = std::make_unique<Player>();
	player_->Init(cubeModel_.get(), playerTextureHandle_);

	// enemy
	enemyTextureHandle_ = TextureManager::Load("cube/cube.jpg");

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init(cubeModel_.get(), enemyTextureHandle_);
	// todo: enemyがnullptrの場合の処理の追加

}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("main");

	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);

	player_->SetOnImGui();
	enemy_->SetOnImGui();

	ImGui::End();

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();

	} else {
		viewProjection_.UpdateMatrix();
	}
	
	player_->Update();
	enemy_->Update();
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
	enemy_->Draw(viewProjection_);

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
