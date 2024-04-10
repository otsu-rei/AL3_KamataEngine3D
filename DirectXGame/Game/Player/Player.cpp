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

	speed_ = 0.2f;

}

void Player::Update() {

	Vector3f velocity = {0.0f, 0.0f, 0.0f};

	// キー入力によるplayerの移動
	if (input_->PushKey(DIK_W)) {
		velocity.y += speed_;
	}

	if (input_->PushKey(DIK_S)) {
		velocity.y -= speed_;
	}

	if (input_->PushKey(DIK_A)) {
		velocity.x -= speed_;
	}

	if (input_->PushKey(DIK_D)) {
		velocity.x += speed_;
	}

	worldTransform_.translation_ += velocity;

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit * -1, kMoveLimit);

	worldTransform_.matWorld_ = Matrix::MakeAffine(
		worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_
	);

	worldTransform_.TransferMatrix();
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
}

void Player::Term() {
	
}

void Player::SetOnImGui() {

	if (ImGui::CollapsingHeader("player")) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
	}

}
