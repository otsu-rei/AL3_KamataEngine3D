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

void Player::Init(Model* head, Model* body, Model* lArm, Model* rArm, const Vector3f& pos) {
	assert(head && body && lArm && rArm);

	parts_.head = head;
	parts_.body = body;
	parts_.lArm = lArm;
	parts_.rArm = rArm;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	parts_.InitWorldTranform(worldTransform_);

	InitFloatingGimmick();
}

void Player::Update() {

	Move();

	/*MoveController();*/

	/*Rotate();*/

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit_ * -1, kMoveLimit_);
	worldTransform_.UpdateMatrix();

	UpdateFloatingGimmick();
}

void Player::Draw(const ViewProjection& viewProj) {
	parts_.Draw(viewProj);
}

void Player::Term() {  }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {

		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);

		ImGui::Text("parts parmeter");
		ImGui::DragFloat3("lArm translation", &parts_.lArmTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("rArm translation", &parts_.rArmTransform.translation_.x, 0.01f);

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

			targetAngle_ = std::atan2(velocity.x, velocity.z);
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

void Player::InitFloatingGimmick() {

	floatingParameter_ = 0.0f;

}

void Player::UpdateFloatingGimmick() {
	// パラメーター分増加
	floatingParameter_ += floatingStep_;

	// 0 ~ 2pi範囲内で抑える
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * pi_v);

	// 浮遊させる
	parts_.bodyTransform.translation_.y = std::sin(floatingParameter_) * floatingRange_;

	// 手の動き
	parts_.lArmTransform.rotation_.x = std::sin(floatingParameter_) * 0.1f;
	parts_.rArmTransform.rotation_.x = std::sin(floatingParameter_) * 0.1f;

	parts_.UpdateMatrix();

}
