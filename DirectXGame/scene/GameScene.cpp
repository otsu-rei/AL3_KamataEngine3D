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

	enemyBodyModel_.reset(Model::CreateFromOBJ("enemy_body"));
	enemyPropellerModel_.reset(Model::CreateFromOBJ("enemy_propeller"));

	weapon_.reset(Model::CreateFromOBJ("weapon"));

	hitEffectModel_.reset(Model::CreateFromOBJ("sphere"));

	// effectmanager
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->Init();

	//* player *//
	playerTextureHandle_ = TextureManager::Load("uvChecker.png");

	player_ = std::make_unique<Player>();
	player_->Init({headModel_.get(), bodyModel_.get(), lArmModel_.get(), rArmModel_.get()});

	player_->SetWeapon(weapon_.get());
	player_->GetWeapon()->SetEffect(effectManager_.get(), hitEffectModel_.get());

	player_->SetGameScene(this);
	player_->SetViewProj(&followCamera_->GetViewProjection());

	// 追従対象の設定
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//* enemy *//
	// 敵の追加
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Init({enemyBodyModel_.get(), enemyPropellerModel_.get()});

	// listに追加
	enemies_.push_back(std::move(enemy));

	// lockOn
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Init();

	// lockOnのptrを渡す
	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());

	//* collisionManager *//
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

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

	//!< 自機の更新処理
	player_->Update();

	//!< 敵の更新処理
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	lockOn_->Update(enemies_, viewProjection_);

	//!< カメラの更新処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

	} else { //!< GameCamera
		followCamera_->Update();
		viewProjection_.matView       = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	}

	viewProjection_.TransferMatrix();

	// 当たり判定の登録
	collisionManager_->Reset(); //!< 新しく登録するので

	collisionManager_->AddCollider(player_.get());
	
	// 攻撃中なら登録する
	if (player_->GetBehavior() == Behavior::kAttack) {
		// todo: 1frameだけ判定が遅れることがある
		collisionManager_->AddCollider(player_->GetWeaponCollider());
	}

	for (const auto& enemy : enemies_) {
		collisionManager_->AddCollider(enemy.get());
	}

	// 当たり判定の実行
	collisionManager_->CheckAllCollisions();

	// worldTransfromの更新
	collisionManager_->UpdateWorldTransform();

	// エフェクトの全更新
	effectManager_->Update();

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
	
	for (auto& enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}

	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	collisionManager_->Draw(viewProjection_);

	effectManager_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	lockOn_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}