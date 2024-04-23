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
#include "MyMath.h"


GameScene::GameScene() {}

GameScene::~GameScene() {

	// game 
	skydome_.reset();
	player_.reset();
	enemy_.reset();

	cubeModel_.reset();
	skydomeModel_.reset();

	debugCamera_.reset();

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(200.0f);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	cubeModel_.reset(Model::Create());
	skydomeModel_.reset(Model::CreateFromOBJ("skydome", true));

	// skydome
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init(skydomeModel_.get());

	// player
	playerTextureHandle_ = TextureManager::Load("uvChecker.png");

	player_ = std::make_unique<Player>();
	player_->Init(cubeModel_.get(), playerTextureHandle_);

	// enemy
	enemyTextureHandle_ = TextureManager::Load("cube/cube.jpg");

	enemy_ = std::make_unique<Enemy>();
	enemy_->SetPlayer(player_.get());
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

	skydome_->Update();
	
	player_->Update();
	enemy_->Update();

	CheckAllCollision();

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

	skydome_->Draw(viewProjection_);
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

void GameScene::CheckAllCollision() {
	// 判定対象A, Bの座標
	Vector3 posA, posB;

	// 弾の取得
	auto& playerBullets = player_->GetBullets();
	auto& enemyBullets  = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定

	posA = player_->GetWorldPosition();

	for (auto& bullet : enemyBullets) {
		posB = bullet->GetWorldPosition();

		float length = Vector::Length(posA - posB);
		if (length <= player_->GetCollisionRadius() + enemy_->GetCollisionRadius()) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 敵キャラと自弾の当たり判定

	posA = enemy_->GetWorldPosition();

	for (auto& bullet : playerBullets) {
		posB = bullet->GetWorldPosition();

		float length = Vector::Length(posA - posB);
		if (length <= player_->GetCollisionRadius() + enemy_->GetCollisionRadius()) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 自弾と敵弾の当たり判定

	for (auto& pBullet : playerBullets) {
		posA = pBullet->GetWorldPosition();

		for (auto& eBullet : enemyBullets) {
			posB = eBullet->GetWorldPosition();

			float length = Vector::Length(posA - posB);
			if (length <= pBullet->GetCollisionRadius() + eBullet->GetCollisionRadius()) {
				pBullet->OnCollision();
				eBullet->OnCollision();
			}

		}
	}

#pragma endregion

}
