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
	TextureManager::Load("reticle.png"); //!< レティクル画像(仮)

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
	
	enemies_.push_back(std::move(newEnemy_));
}

void GameScene::Update() {

#ifdef _DEBUG

	ImGui::Begin("main");

	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);
	railCamera_->SetOnImGui();
	player_->SetOnImGui();

	////!< ImGuiのstack idへの対策
	//int id = 0;
	//for (const auto& enemy : enemies_) {
	//	std::string labelID = "##";
	//	labelID += std::to_string(id);

	//	enemy->SetOnImGui(labelID.c_str());
	//	id++;
	//}

	//ImGui::Text("waitTime: %d", waitTime_);

	ImGui::End();

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView       = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

	} else {
		railCamera_->Update();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	}

	viewProjection_.TransferMatrix();


	skydome_->Update();
	
	//!< 本体の更新処理
	player_->Update(viewProjection_);
	UpdatePlayerBullet();

	UpdateEnemyPopCommands();

	// 死んだ敵機の削除
	enemies_.remove_if([](auto& enemy) {
		if (enemy->IsDead()) {
			return true;
		}

		return false;
	});

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	UpdateEnemyBullet();

	CheckAllCollision();

	#ifdef _DEBUG

	ImGui::Begin("debug");
	player_->SetOnDebugImGui();
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

	/*DrawGrid({0.0f}, 10.0f, 6, {1.0f, 1.0f, 1.0f, 1.0f});*/
	
	skydome_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	DrawPlayerBullet(viewProjection_);

	for (auto& enemy : enemies_) {
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
	player_->DrawUI();

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
	std::list<Collider*> colliders_;
	// コライダーをリストに登録
	// 自キャラ
	colliders_.push_back(player_.get());
	
	// 敵キャラすべて
	for (auto& enemy : enemies_) {
		colliders_.push_back(enemy.get());
	}

	// 自弾すべて
	for (auto& bullet : playerBullets_) {
		colliders_.push_back(bullet.get());
	}

	// 敵弾すべて
	for (auto& bullet : enemyBullets_) {
		colliders_.push_back(bullet.get());
	}

	// リスト内のペアの総当たり
	auto itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		
		auto itrB = itrA; //!< 自身に当たることを避けるため
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {

			CheckCollisionPair(*itrA, *itrB);
		}
	}

	colliders_.clear();
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

			enemies_.push_back(std::move(newEnemy_));

		} else if (word.find("WAIT") == 0) { //!< 待機コマンド
			std::getline(line_stream, word, ',');
			
			int32_t waitTime = std::atoi(word.c_str());

			isWait_ = true;
			waitTime_ = waitTime;

			break; //!< 次のコマンドへの待機
		}
	}

}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask())
		|| !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return; //!< 属性(自分)とマスク(判定先)が一致しない場合
	}

	float lenght = Vector::Length(colliderA->GetWorldPosition() - colliderB->GetWorldPosition());

	if (lenght <= colliderA->GetRadius() + colliderB->GetRadius()) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}
