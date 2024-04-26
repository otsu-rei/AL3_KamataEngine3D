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

	// game 
	skydome_.reset();
	player_.reset();
	enemys_.clear();

	enemyBullets_.clear();
	playerBullets_.clear();

	cubeModel_.reset();
	skydomeModel_.reset();

	debugCamera_.reset();

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(200.0f);

	// railCamera
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Init({0.0f, 0.0f, -20.0f}, {0.0f});

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
	player_->Init(cubeModel_.get(), playerTextureHandle_, {0.0f, -4.0f, 30.0f});
	player_->SetGameScene(this);
	player_->SetParent(&railCamera_->GetWorldTranform());

	// enemy
	enemyTextureHandle_ = TextureManager::Load("cube/cube.jpg");
	LoadEnemyPopData();

	std::unique_ptr<Enemy> newEnemy_ = std::make_unique<Enemy>();
	newEnemy_->SetPlayer(player_.get());
	newEnemy_->SetGameScene(this);
	newEnemy_->Init(cubeModel_.get(), enemyTextureHandle_, {0.0f, 5.0f, 20.0f});
	
	enemys_.push_back(std::move(newEnemy_));
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("main");

	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);
	railCamera_->SetOnImGui();
	player_->SetOnImGui();

	//!< ImGuiのstack idへの対策
	int id = 0;
	for (const auto& enemy : enemys_) {
		std::string labelID = "##";
		labelID += std::to_string(id);

		enemy->SetOnImGui(labelID.c_str());
		id++;
	}

	ImGui::Text("waitTime: %d", waitTime_);

	ImGui::End();

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();

	} else {
		railCamera_->Update();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}

	skydome_->Update();
	
	//!< 本体の更新処理
	player_->Update();
	UpdatePlayerBullet();

	UpdateEnemyPopCommands();

	// 死んだ敵機の削除
	enemys_.remove_if([](auto& enemy) {
		if (enemy->IsDead()) {
			return true;
		}

		return false;
	});

	for (auto& enemy : enemys_) {
		enemy->Update();
	}

	UpdateEnemyBullet();

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

	/*DrawGrid({0.0f}, 10.0f, 6, {1.0f, 1.0f, 1.0f, 1.0f});*/
	
	skydome_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	DrawPlayerBullet(viewProjection_);

	for (auto& enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	DrawEnemyBullet(viewProjection_);

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

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet>& enemyBullet) {
	enemyBullets_.push_back(std::move(enemyBullet));
	//
}

void GameScene::AddPlayerBullet(std::unique_ptr<PlayerBullet>& playerBullet) {
	playerBullets_.push_back(std::move(playerBullet));
	//
}

void GameScene::UpdatePlayerBullet() {
	// deathフラグの立った弾の削除
	playerBullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			return true;
		}

		return false;
	});

	for (auto& bullet : playerBullets_) {
		bullet->Update();
	}
}

void GameScene::UpdateEnemyBullet() {
	//!< 敵弾の更新処理
	// deathフラグの立った弾の削除
	enemyBullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			return true;
		}

		return false;
	});

	for (auto& bullet : enemyBullets_) {
		bullet->Update();
	}

}

void GameScene::DrawPlayerBullet(const ViewProjection& viewProj) {
	for (auto& bullet : playerBullets_) {
		bullet->Draw(viewProj);
	}
}

void GameScene::DrawEnemyBullet(const ViewProjection& viewProj) {
	for (auto& bullet : enemyBullets_) {
		bullet->Draw(viewProj);
	}
}

void GameScene::CheckAllCollision() {
	// 判定対象A, Bの座標
	Vector3 posA, posB;

#pragma region 自キャラと敵弾の当たり判定

	posA = player_->GetWorldPosition();

	for (auto& bullet : enemyBullets_) {
		posB = bullet->GetWorldPosition();

		float length = Vector::Length(posA - posB);
		if (length <= player_->GetCollisionRadius() + bullet->GetCollisionRadius()) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 敵キャラと自弾の当たり判定

	for (auto& enemy : enemys_) {
		posA = enemy->GetWorldPosition();

		for (auto& bullet : playerBullets_) {
			posB = bullet->GetWorldPosition();

			float length = Vector::Length(posA - posB);
			if (length <= player_->GetCollisionRadius() + enemy->GetCollisionRadius()) {
				player_->OnCollision();
				bullet->OnCollision();
			}
		}
	}

#pragma endregion

#pragma region 自弾と敵弾の当たり判定

	for (auto& pBullet : playerBullets_) {
		posA = pBullet->GetWorldPosition();

		for (auto& eBullet : enemyBullets_) {
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

void GameScene::LoadEnemyPopData() {
	const std::string filePath = "./resources/enemyPop.csv"; //!< ファイルパス

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	enemyPopCommands_ << file.rdbuf();

	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	if (isWait_) { //!< 待機中...
		waitTime_--;

		if (waitTime_ <= 0) {
			isWait_ = false;
		}

		return;
	}

	std::string line;

	while (std::getline(enemyPopCommands_, line)) {
		std::istringstream line_stream(line);

		std::string word;
		std::getline(line_stream, word, ',');

		// "//" ...
		if (word.find("//") == 0) { //!< コメント"//"なので読み飛ばし
			continue;
		}

		if (word.find("POP") == 0) { //!< 敵の出現コマンド
			Vector3f pos;

			std::getline(line_stream, word, ',');
			pos.x = static_cast<float>(std::atof(word.c_str()));

			std::getline(line_stream, word, ',');
			pos.y = static_cast<float>(std::atof(word.c_str()));

			std::getline(line_stream, word, ',');
			pos.z = static_cast<float>(std::atof(word.c_str()));

			// 新しい敵の出現
			std::unique_ptr<Enemy> newEnemy_ = std::make_unique<Enemy>();
			newEnemy_->SetPlayer(player_.get());
			newEnemy_->SetGameScene(this);
			newEnemy_->Init(cubeModel_.get(), enemyTextureHandle_, pos);

			enemys_.push_back(std::move(newEnemy_));

		} else if (word.find("WAIT") == 0) { //!< 待機コマンド
			std::getline(line_stream, word, ',');
			
			int32_t waitTime = std::atoi(word.c_str());

			isWait_ = true;
			waitTime_ = waitTime;

			break; //!< 次のコマンドへの待機
		}
	}

}
