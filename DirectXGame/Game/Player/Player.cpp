#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include "GameScene.h"
#include "imgui.h"
#include <cassert>
#include <algorithm>

#include "MyMath.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(Model* model, uint32_t textureHandle, const Vector3f& pos) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform3DReticle_.Initialize();

	worldTransform_.translation_ = pos;

	uint32_t reticleTexture = TextureManager::Load("reticle.png");
	sprite2DReticle_.reset(Sprite::Create(reticleTexture, {WinApp::kWindowWidth / 2.0f, WinApp::kWindowHeight / 2.0f}, {0.2f, 0.2f, 1.0f, 1.0f}, {0.5f, 0.5f}));

	lockOnTexture_ = TextureManager::Load("lockOn.png");

	viewProj_.Initialize();

	// collider
	SetRadius(kCollisionRadius_);
	SetCollisionAttribute(PLAYER);
	SetCollisionMask(ENEMY_BULLET);

}

void Player::Update(const ViewProjection& viewProj) {

	Move();

	/*MoveController();*/

	/*Rotate();*/

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit * -1, kMoveLimit);
	worldTransform_.UpdateMatrix();

	viewProj_.matView = Matrix::Inverse(worldTransform_.matWorld_);

	UpdateReticle(viewProj);

	Attack();

	/*AttackController();*/
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
}

void Player::DrawUI(const ViewProjection& viewProj) {
	sprite2DReticle_->Draw();

	// 前フレームの描画情報の削除
	lockOnSprites_.clear();

	// screen座標変換用のmatrixの設定
	Matrix4x4 vpvMatrix
		= viewProj.matView * viewProj.matProjection * Matrix::MakeViewport(0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), 0.0f, 1.0f);

	for (const auto& enemy : lockOnEnemies_) {

		// 生きているかどうかの確認
		if (!isAliveEnemy(enemy)) {
			continue;
		}

		// screen座標に変換
		Vector3f enemyScreenPos = Matrix::Transform(enemy->GetWorldPosition(), vpvMatrix);

		std::unique_ptr<Sprite> newSprite;
		newSprite.reset(Sprite::Create(lockOnTexture_, {enemyScreenPos.x, enemyScreenPos.y}, {1.0f, 0.05f, 0.05f, 1.0f}, {0.5f, 0.5f}));
		newSprite->SetSize({128.0f, 128.0f});
		newSprite->Draw();

		// コマンド終了まで情報の保持
		lockOnSprites_.push_back(
			std::move(newSprite)
		);
	}
}

void Player::Term() { sprite2DReticle_.reset(); }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {

		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);
		ImGui::DragFloat3("reticle", &worldTransform3DReticle_.translation_.x, 0.0f);

		ImGui::Spacing();

		ImGui::DragFloat("distanceReticleObject", &distanceReticleObject_, 1.0f);

		ImGui::TreePop();
	}

}

void Player::SetOnDebugImGui() {
	if (ImGui::TreeNode("player")) {

		ImGui::Text("2DReticle: %.2f, %.2f", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
		ImGui::Text("Near:      %.2f, %.2f, %.2f", nearPos.x, nearPos.y, nearPos.z);
		ImGui::Text("Far:       %.2f, %.2f, %.2f", farPos.x, farPos.y, farPos.z);
		ImGui::Text("3DReticle: %.2f, %.2f, %.2f",
			worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z
		);

		ImGui::TreePop();
	}
}

void Player::OnCollision() {}

Vector3f Player::GetWorldPosition() const {
	Vector3f result;
	result.x = worldTransform_.matWorld_.m[3][0];
	result.y = worldTransform_.matWorld_.m[3][1];
	result.z = worldTransform_.matWorld_.m[3][2];
	
	return result;
}

//=========================================================================================
// private
//=========================================================================================

void Player::Move() {
	Vector3f velocity = {0.0f, 0.0f, 0.0f};

	// キー入力によるplayerの移動
	if (input_->PushKey(DIK_W)) {
		velocity.y += kMoveSpeed_;
	}

	if (input_->PushKey(DIK_S)) {
		velocity.y -= kMoveSpeed_;
	}

	if (input_->PushKey(DIK_A)) {
		velocity.x -= kMoveSpeed_;
	}

	if (input_->PushKey(DIK_D)) {
		velocity.x += kMoveSpeed_;
	}

	worldTransform_.translation_ += velocity; 
	
}

void Player::MoveController() {
	XINPUT_STATE joyState;
	Vector3f velocity = {0.0f, 0.0f, 0.0f};

	if (input_->GetJoystickState(0, joyState)) {
		velocity.x += static_cast<float>(joyState.Gamepad.sThumbLX) / SHRT_MAX * kMoveSpeed_;
		velocity.y += static_cast<float>(joyState.Gamepad.sThumbLY) / SHRT_MAX * kMoveSpeed_;

	} else {
		return; //!< コントローラーが接続されてない場合
	}

	worldTransform_.translation_ += velocity;
}

void Player::Rotate() {
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed_;
	}

	if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed_;
	}
}

void Player::Attack() { 
	if (input_->TriggerKey(DIK_SPACE)) {
		if (lockOnEnemies_.empty()) { //!< lockOnした標的がいない場合

			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();

			// 弾の進行方向の設定
			Vector3f velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
			velocity = Vector::Normalize(velocity) * kBulletSpeed_;

			newBullet->Init(model_, Matrix::Transform(worldTransform_.translation_, worldTransform_.parent_->matWorld_), velocity);

			gameScene_->AddPlayerBullet(newBullet);

		} else { //!< lockOnした標的がいる場合
			for (const auto& enemy : lockOnEnemies_) {

				// 生きているかどうかの確認
				if (!isAliveEnemy(enemy)) {
					continue;
				}
				
				std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();

				// 弾の進行方向の設定
				Vector3f velocity = enemy->GetWorldPosition() - GetWorldPosition();
				velocity = Vector::Normalize(velocity) * kBulletSpeed_;

				newBullet->Init(model_, Matrix::Transform(worldTransform_.translation_, worldTransform_.parent_->matWorld_), velocity);

				gameScene_->AddPlayerBullet(newBullet);
			}
			
			// 発射したのでlistから除外
			lockOnEnemies_.clear();
		}
		
	}
}

void Player::AttackController() {
	XINPUT_STATE joyState;

	if (!input_->GetJoystickState(0, joyState)) {
		return; //!< コントローラーが接続されてない場合, retrun;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();

		// 弾の進行方向の設定
		Vector3f velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
		velocity = Vector::Normalize(velocity) * kBulletSpeed_;

		newBullet->Init(model_, Matrix::Transform(worldTransform_.translation_, worldTransform_.parent_->matWorld_), velocity);

		gameScene_->AddPlayerBullet(newBullet);
	}
}

void Player::UpdateReticle(const ViewProjection& viewProj) {
	/*UpdateReticleDirection();*/

	UpdateReticleMouse(viewProj);

	/*UpdateReticleController(viewProj);*/

	UpdateReticleLockOn(viewProj);
}

void Player::UpdateReticleDirection() {

	Vector3f offset = Matrix::TransformNormal({0.0f, 0.0f, 1.0f}, worldTransform_.parent_->matWorld_);
	offset = Vector::Normalize(offset) * 50.0f;

	worldTransform3DReticle_.translation_ = Matrix::Transform(offset, worldTransform_.matWorld_);
	worldTransform3DReticle_.UpdateMatrix();
}

void Player::UpdateReticleMouse(const ViewProjection& viewProj) {
	// 2Dマウス座標から3D空間にレティクルを更新
	
	POINT mousePosition;
	// マウス座標を取得
	GetCursorPos(&mousePosition);

	// クライアント領域に変換
	ScreenToClient(WinApp::GetInstance()->WinApp::GetHwnd(), &mousePosition);

	sprite2DReticle_->SetPosition({static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)});

	worldTransform3DReticle_.translation_ = ScreenToWorld(viewProj, sprite2DReticle_->GetPosition(), distanceReticleObject_);
	worldTransform3DReticle_.UpdateMatrix();
}

void Player::UpdateReticleController(const ViewProjection& viewProj) {
	// コントローラーRight入力から3D空間に弾を発射

	XINPUT_STATE joyState;

	if (!input_->GetJoystickState(0, joyState)) {
		return; //!< コントローラーが接続されてない場合, retrun;
	}

	// parameter < TODO: hに移動
	const float reticleSpeed = 5.0f;

	Vector2f spritePosition = sprite2DReticle_->GetPosition();

	spritePosition.x += static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX * reticleSpeed;
	spritePosition.y -= static_cast<float>(joyState.Gamepad.sThumbRY) / SHRT_MAX * reticleSpeed;
	sprite2DReticle_->SetPosition(spritePosition);

	worldTransform3DReticle_.translation_ = ScreenToWorld(viewProj, sprite2DReticle_->GetPosition(), distanceReticleObject_);
	worldTransform3DReticle_.UpdateMatrix();
}

void Player::UpdateReticleLockOn(const ViewProjection& viewProj) {
	static Vector2f preLockEnemyScreenPos;

	// 現在のreticleの座標の取得
	Vector2f reticlePos = sprite2DReticle_->GetPosition();

	// screen座標変換用のmatrixの設定
	Matrix4x4 vpvMatrix
	    = viewProj.matView * viewProj.matProjection * Matrix::MakeViewport(0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), 0.0f, 1.0f);

	// enemiesとの参照
	for (const auto& enemy : gameScene_->GetEnemies()) {

		// lockon範囲内かどうかの計算
		float distance = Vector::Length(enemy->GetWorldPosition() - worldTransform_.parent_->GetTransform());

		if (distance >= kLockOnRange_) {
			continue; //!< 範囲外なので
		}

		// enemyPosをスクリーン上の座標に変換
		Vector3f enemyScreenPos = Matrix::Transform(enemy->GetWorldPosition(), vpvMatrix); //!< zは使わないこと

		// hack: Vector2 length function
		float length = Vector::Length({reticlePos.x - enemyScreenPos.x, reticlePos.y - enemyScreenPos.y, 0.0f});

		if (length <= sprite2DReticle_->GetSize().x) { //!< x, yのsize同じなのでxをradiusとして利用
			// spriteを敵に合わせる
			sprite2DReticle_->SetPosition({enemyScreenPos.x, enemyScreenPos.y});

			// targetを敵に合わせる
			worldTransform3DReticle_.translation_ = enemy->GetWorldPosition();
			worldTransform3DReticle_.UpdateMatrix();

			preLockEnemyScreenPos = {enemyScreenPos.x, enemyScreenPos.y};
			unlockT_ = 1.0f;

			// lockOnしたのでlistにptrを追加
			lockOnEnemies_.insert(enemy.get());

			// todo: lockOnした敵にレティクルを出す
			
			return; //!< ロックオンしてるので早期return
		}
	}

	// TODO: world3DReticleの設定. lockOnが外れた際, lerpでmousePositionまで戻す

	// 外れた際, unlockTの減少
	unlockT_ -= 1.0f / 4.0f;
	unlockT_ = std::clamp(unlockT_, 0.0f, 1.0f);

	// reticleのpositionを元のpositionに戻す
	sprite2DReticle_->SetPosition({
	    std::lerp(reticlePos.x, preLockEnemyScreenPos.x, unlockT_),
	    std::lerp(reticlePos.y, preLockEnemyScreenPos.y, unlockT_),
	});

	worldTransform3DReticle_.translation_ = ScreenToWorld(viewProj, sprite2DReticle_->GetPosition(),distanceReticleObject_);
	worldTransform3DReticle_.UpdateMatrix();
}

bool Player::isAliveEnemy(Enemy* target) { 

	auto it = std::find_if(gameScene_->GetEnemies().begin(), gameScene_->GetEnemies().end(), [target](const std::unique_ptr<Enemy>& enemyPtr) { return target == enemyPtr.get(); });

	if (it == gameScene_->GetEnemies().end()) {
		return false; //!< 存在してない
	}

	return true;
}

Vector3f Player::ScreenToWorld(const ViewProjection& viewProj, const Vector2f& screenPosition, float distance) {

	Matrix4x4 matVPV
		= viewProj.matView * viewProj.matProjection * Matrix::MakeViewport(0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), 0.0f, 1.0f);

	Matrix4x4 matInverseVPV = Matrix::Inverse(matVPV);

	nearPos = {static_cast<float>(screenPosition.x), static_cast<float>(screenPosition.y), 0.0f};
	farPos = {static_cast<float>(screenPosition.x), static_cast<float>(screenPosition.y), 1.0f};

	nearPos = Matrix::Transform(nearPos, matInverseVPV);
	farPos = Matrix::Transform(farPos, matInverseVPV);

	Vector3f direction = farPos - nearPos;
	direction = Vector::Normalize(direction);

	return nearPos + direction * distance;
}
