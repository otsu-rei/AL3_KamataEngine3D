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
#include "ColliderManager.h"


GameScene::GameScene() {}

GameScene::~GameScene() {
}	


void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	sceneManager_ = std::make_unique<SceneManager>();
	std::unique_ptr<IScene> scene = std::make_unique<Scene_Title>();
	scene->Init();
	sceneManager_->Init(scene);

}

void GameScene::Update() {

	sceneManager_->Update();

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
	sceneManager_->DrawBackSprite();

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
	sceneManager_->Draw3D();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	sceneManager_->DrawLine();
	ColliderManager::GetInstance()->DrawColliders();

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	sceneManager_->DrawFrontSprite();


	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}