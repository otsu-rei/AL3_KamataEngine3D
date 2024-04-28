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

	uint32_t textureReticle = TextureManager::Load("reticle.png");
	sprite2DReticle_.reset(Sprite::Create(textureReticle, {WinApp::kWindowWidth / 2.0f, WinApp::kWindowHeight / 2.0f}, {0.2f, 0.2f, 1.0f, 1.0f}, {0.5f, 0.5f}));

}

void Player::Update(const ViewProjection& viewProj) {

	Move();

	/*Rotate();*/


	worldTransform_.UpdateMatrix();

	UpdateReticle();

	Attack();

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3f position = worldTransform3DReticle_.GetTransform();

		Matrix4x4 viewportMatrix = Matrix::MakeViewport(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
		
		Matrix4x4 vpvMatrix = viewProj.matView * viewProj.matProjection * viewportMatrix;

		position = Matrix::Transform(position, vpvMatrix);

		sprite2DReticle_->SetPosition({position.x, position.y});
	}
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
}

void Player::DrawUI() { sprite2DReticle_->Draw(); }

void Player::Term() { sprite2DReticle_.reset(); }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {

		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);
		ImGui::DragFloat3("reticle", &worldTransform3DReticle_.translation_.x, 0.0f);

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
		
		Vector3f velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
		velocity = Vector::Normalize(velocity) * kBulletSpeed_;

		newBullet->Init(model_, Matrix::Transform(worldTransform_.translation_, worldTransform_.parent_->matWorld_), velocity);

		gameScene_->AddPlayerBullet(newBullet);
	}
}

void Player::UpdateReticle() {

	Vector3f offset = Matrix::TransformNormal({0.0f, 0.0f, 1.0f}, worldTransform_.parent_->matWorld_);
	offset = Vector::Normalize(offset) * 50.0f;

	worldTransform3DReticle_.translation_ = Matrix::Transform(offset, worldTransform_.matWorld_);
	worldTransform3DReticle_.UpdateMatrix();
}