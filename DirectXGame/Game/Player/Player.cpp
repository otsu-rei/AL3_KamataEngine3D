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

void Player::Init(Model* model, const Vector3f& pos) {
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

}

void Player::Update() {

	Move();

	/*MoveController();*/

	/*Rotate();*/

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit_ * -1, kMoveLimit_);
	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj);
}

void Player::Term() {  }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {

		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);

		ImGui::TreePop();
	}

}

//=========================================================================================
// private
//=========================================================================================

void Player::Move() {

	XINPUT_STATE joyState;

	// コントローラーでの移動
	if (input_->GetJoystickState(0, joyState)) { //!< コントローラーが接続されている場合
		// Stickの移動量を取得
		Vector3f move = {
			static_cast<float>(joyState.Gamepad.sThumbLX) / SHRT_MAX,
			0.0f,
			static_cast<float>(joyState.Gamepad.sThumbLY) / SHRT_MAX
		};

		// デッドゾーンの確認
		if (Vector::Length(move) > kDeadZone_) {
			// 移動処理
			Vector3f velocity = Vector::Normalize(move) * kMoveSpeed_;
			velocity = Matrix::TransformNormal(velocity, Matrix::MakeRotate(viewProj_->rotation_.y, kRotateBaseY));
			worldTransform_.translation_ += velocity;

			targetAngle_ = std::atan2(move.x, move.z);
		}
	}

	
	worldTransform_.rotation_.y = LerpShortAngle(worldTransform_.rotation_.y, targetAngle_, kRotateRate_);
	
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