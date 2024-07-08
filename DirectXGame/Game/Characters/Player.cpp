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

void Player::Init(const std::vector<Model*>& models) {

	assert(models.size() == kCountOfModelType);

	// worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 30.0f};

	models_ = models;

	
	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].Initialize();
	}

	// parts位置調整
	modelTransforms_[MODEL_BODY].SetParent(&worldTransform_); //!< world -> this

	modelTransforms_[MODEL_HEAD].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this

	modelTransforms_[MODEL_LARM].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this
	modelTransforms_[MODEL_LARM].translation_ = {-1.4f, 2.5f, 0.0f};

	modelTransforms_[MODEL_RARM].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this
	modelTransforms_[MODEL_RARM].translation_ = {1.4f, 2.5f, 0.0f};

	InitFloatingGimmick();
}

void Player::Update() {

	Move();

	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit_ * -1, kMoveLimit_);
	worldTransform_.UpdateMatrix();

	UpdateFloatingGimmick();
}

void Player::Draw(const ViewProjection& viewProj) {
	for (int i = 0; i < kCountOfModelType; ++i) {
		models_[i]->Draw(modelTransforms_[i], viewProj);
	}
}

void Player::Term() {  }

void Player::SetOnImGui() {

	if (ImGui::TreeNode("player")) {

		ImGui::DragFloat3("pos",    &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x,    0.01f);

		ImGui::Text("parts parmeter");
		ImGui::DragFloat3("lArm translation", &modelTransforms_[MODEL_LARM].translation_.x, 0.01f);
		ImGui::DragFloat3("rArm translation", &modelTransforms_[MODEL_RARM].translation_.x, 0.01f);

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
	modelTransforms_[MODEL_BODY].translation_.y = std::sin(floatingParameter_) * floatingRange_;

	// 手の動き
	modelTransforms_[MODEL_LARM].rotation_.x = std::sin(floatingParameter_) * 0.1f;
	modelTransforms_[MODEL_RARM].rotation_.x = std::sin(floatingParameter_) * 0.1f;

	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].UpdateMatrix();
	}
}
