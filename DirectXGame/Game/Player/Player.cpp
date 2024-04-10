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

	// parameters

}

void Player::Update() {

	Move();

	/*Rotate();*/

	Attack();

	if (bullet_) {
		bullet_->Update();
	}

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);

	if (bullet_) {
		bullet_->Draw(viewProj);
	}
}

void Player::SetOnImGui() {

	if (ImGui::CollapsingHeader("player")) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
	}

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
		bullet_ = new PlayerBullet();
		bullet_->Init(model_, worldTransform_.translation_);
	}
}
