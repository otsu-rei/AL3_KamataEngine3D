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
#include "Easing.h"

#include "GlobalVariables.h"
#include "LockOn.h"

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

	modelTransforms_[MODEL_WEAPON].SetParent(&worldTransform_); //!< world -> this
	modelTransforms_[MODEL_WEAPON].translation_ = {0.0f, 2.4f, 0.0f};

	// グループの追加
	const std::string groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "lArm translation", modelTransforms_[MODEL_LARM].translation_);
	globalVariables->AddItem(groupName, "rArm translation", modelTransforms_[MODEL_RARM].translation_);
	globalVariables->AddItem(groupName, "weapon translation", modelTransforms_[MODEL_WEAPON].translation_);
	globalVariables->AddItem(groupName, "move speed", moveSpeed_);

	InitFloatingGimmick();
}

void Player::Update() {

	ApplyGlobalVariables();

	// 移動方向へのvelocityのリセット
	velocity_.x = 0.0f;
	velocity_.z = 0.0f;

	if (behaviorRequest_) { //!< 次行動へのリクエストがある場合
		// ふるまいの変更
		behavior_ = behaviorRequest_.value();

		// ふるまいが変わるので一度初期化
		switch (behavior_) {
		    case Behavior::kRoot:
			default:
			    BehaviorRootInit();
			    break;

			case Behavior::kAttack:
			    BehaviorAttackInit();
			    break;

			case Behavior::kJump:
			    BehaviorJumpInit();
			    break;
		}

		behaviorRequest_ = std::nullopt;
	}
	
	switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootUpdate();
			break;

		case Behavior::kAttack:
			BehaviorAttackUpdate();
			break;

		case Behavior::kJump:
		    BehaviorJampUpdate();
		    break;
	}

	// 移動
	worldTransform_.translation_ += velocity_;

	// プレイヤーの視点を移動方向に
	targetAngle_ = std::atan2(direction_.x, direction_.z);
	worldTransform_.rotation_.y = LerpShortAngle(worldTransform_.rotation_.y, targetAngle_, kRotateRate_);

	// transformの更新
	worldTransform_.translation_ = Vector::Clamp(worldTransform_.translation_, kMoveLimit_ * -1, kMoveLimit_);
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].UpdateMatrix();
	}
}

void Player::Draw(const ViewProjection& viewProj) {
	for (int i = 0; i < kCountOfModelType; ++i) {
		if (i == MODEL_WEAPON && behavior_ != Behavior::kAttack) { //!< 攻撃中でしか描画しない
			continue;
		}

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

		ImGui::DragFloat3("weapon translate", &modelTransforms_[MODEL_WEAPON].translation_.x, 0.01f);

		ImGui::TreePop();
	}

}

//=========================================================================================
// private
//=========================================================================================

void Player::ApplyGlobalVariables() {

	const std::string groupName = "Player";

	modelTransforms_[MODEL_LARM].translation_ = globalVariables->GetValue<Vector3f>(groupName, "lArm translation");
	modelTransforms_[MODEL_RARM].translation_ = globalVariables->GetValue<Vector3f>(groupName, "rArm translation");
	modelTransforms_[MODEL_WEAPON].translation_ = globalVariables->GetValue<Vector3f>(groupName, "weapon translation");
	moveSpeed_ = globalVariables->GetValue<float>(groupName, "move speed");

}

void Player::Move() {

	XINPUT_STATE joyState;

	// コントローラーでの移動
	if (input_->GetJoystickState(0, joyState)) { //!< コントローラーが接続されている場合

		Vector3f move = {
			static_cast<float>(joyState.Gamepad.sThumbLX) / SHRT_MAX,
			0.0f,
			static_cast<float>(joyState.Gamepad.sThumbLY) / SHRT_MAX
		};

		// デッドゾーンの確認
		if (Vector::Length(move) > kDeadZone_) {

			move = Vector::Normalize(move) * moveSpeed_;

			// 移動処理
			velocity_ = Matrix::TransformNormal(move, Matrix::MakeRotate(viewProj_->rotation_.y, kRotateBaseY));

			// 移動方向を代入
			direction_ = velocity_;

		} else if (lockOn_ && lockOn_->GetTargetPosition().has_value()) { //!< スティックによる移動入力がない時 && ロックオンの対象がいる場合
		
			Vector3f lockOnPos = lockOn_->GetTargetPosition().value();

			direction_ = lockOnPos - worldTransform_.translation_;

		}

		// hack: 別関数に分けたらよくなる...?
		// コントローラーでの攻撃
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) { //!< Bが押された場合
			behaviorRequest_ = Behavior::kAttack; //!< 攻撃状態へのリクエスト
		}

		// コントローラーでのジャンプ
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) { //!< Aが押されたとき
			behaviorRequest_ = Behavior::kJump; //!< ジャンプ状態へのリクエスト
		}
	}
	
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

}

void Player::BehaviorRootInit() {
	
}

void Player::BehaviorRootUpdate() {

	Move();

	UpdateFloatingGimmick();
}

void Player::BehaviorAttackInit() {
	attackParameter_ = 0.0f;
	attackMoveSpeed_ = 0.8f;
}

void Player::BehaviorAttackUpdate() {

	attackParameter_++;

	if (attackParameter_ >= kAttackTime_) { //!< 攻撃の挙動が終了した時の処理
		behaviorRequest_ = Behavior::kRoot; //!< 元の状態に戻る
		return;
	}

	float t = attackParameter_ / kAttackTime_; //!< 媒介変数化
	float easeT = EaseOutBounce(t);
	//!< todo: easingを入れてそれっぽく見せる

	modelTransforms_[MODEL_WEAPON].rotation_.x = std::lerp(0.0f, pi_v / 2.0f, easeT);

	modelTransforms_[MODEL_LARM].rotation_.x = pi_v + std::lerp(0.0f, pi_v / 2.0f, easeT);
	modelTransforms_[MODEL_RARM].rotation_.x = pi_v + std::lerp(0.0f, pi_v / 2.0f, easeT);

	// ロックオン中, directionをlockOnしてる敵のほうに向かせる
	if (lockOn_ && lockOn_->GetTargetPosition().has_value()) {
		Vector3f lockOnPos = lockOn_->GetTargetPosition().value();
		Vector3f sub = lockOnPos - worldTransform_.translation_;

		// 距離
		float distance = Vector::Length({sub.x, 0.0f, sub.z});

		// 距離のしきい値
		const float threshold = 1.0f;

		if (distance > threshold) { //!< しきい値より離れてる場合のみ

			direction_ = sub;

			if (attackMoveSpeed_ > distance - threshold) { //!< しきい値を超える速さなら補正
				attackMoveSpeed_ = 0.0f;
			}
		}
	}

	// 向いてる方向に少しずつ移動
	velocity_ += Vector::Normalize({direction_.x, 0.0f, direction_.z}) * attackMoveSpeed_;

}

void Player::BehaviorJumpInit() {

	modelTransforms_[MODEL_BODY].translation_.y = 0.0f;
	modelTransforms_[MODEL_LARM].rotation_.x    = 0.0f;
	modelTransforms_[MODEL_RARM].rotation_.x    = 0.0f;

	// ジャンプの初速
	const float kJumpFirstSpeed = 1.0f;

	// ジャンプの初速を与える
	velocity_.y = kJumpFirstSpeed;
}

void Player::BehaviorJampUpdate() {

	// todo: ジャンプ中の移動を入れる

	// 移動
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y <= 0.0f) { //!< 地面(y = 0)地点についたらジャンプの終了
		worldTransform_.translation_.y = 0.0f;
		velocity_.y = 0.0f;
		behaviorRequest_ = Behavior::kRoot; //!< ジャンプの終了
		return;
	}

	// 重力加速度
	const float kGravityAcceleration = 0.05f;

	// 加速度ベクトル
	Vector3f acceleration = {0.0f, -kGravityAcceleration, 0.0f};

	velocity_ += acceleration;
}
