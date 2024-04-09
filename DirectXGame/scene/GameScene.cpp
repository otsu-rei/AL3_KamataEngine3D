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

	sprite_.reset();
	model_.reset();
	debugCamera_.reset();

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("uvChecker.png");
	
	sprite_.reset(Sprite::Create(textureHandle_, {100.0f, 100.0f}));

	model_.reset(Model::Create());
	worldTransform_.Initialize();
	viewProjection_.Initialize();


	soundHandle_ = audio_->LoadWave("mokugyo.wav");
	voiceHandle_ = audio_->PlayWave(soundHandle_, true); //!< soundの再生

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {

	debugCamera_->Update();

	Vector2 position = sprite_->GetPosition();
	position += {2.0f, 1.0f};
	sprite_->SetPosition(position);
	
	if (input_->TriggerKey(DIK_SPACE)) {
		audio_->StopWave(voiceHandle_);
	}

	ImGui::ShowDemoWindow();
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
	
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	DrawGrid({0.0f, 0.0f, 0.0f}, 30.0f, 10, {1.0f, 1.0f, 1.0f, 1.0f}, debugCamera_->GetViewProjection());

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw();


	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
