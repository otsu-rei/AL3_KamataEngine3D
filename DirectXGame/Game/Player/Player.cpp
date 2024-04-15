#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include "imgui.h"
#include <cassert>
#include <algorithm>

#include "MyMath.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(Model* model, uint32_t textureHandle) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();


}

void Player::Update() {

	// deathフラグの立った弾の削除
	bullets_.remove_if([](auto& bullet) {

		if (bullet->IsDead()) {
			return true;
		}

		return false;
	});

	Move();

	/*Rotate();*/

	worldTransform_.UpdateMatrix();

	Attack();

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);

	for (auto& bullet : bullets_) {
		bullet->Draw(viewProj);
	}
}

void Player::Term() { bullets_.clear(); }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {
		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);
		ImGui::TreePop();
	}

}

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

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit * -1, kMoveLimit);
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
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();

		// 弾の進行方向の設定
		
		Vector3f velocity = {0.0f, 0.0f, kBulletSpeed_};
		velocity = Matrix::TransformNormal(velocity, worldTransform_.matWorld_); //!< 自機の向きに合わせる

		newBullet->Init(model_, worldTransform_.translation_, velocity);

		bullets_.push_back(std::move(newBullet));
	}
}
