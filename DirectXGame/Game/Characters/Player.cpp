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

//=========================================================================================
// static variables
//=========================================================================================

const std::array<PlayerBehaviorAttack::ConstAttack, PlayerBehaviorAttack::kComboNum_> PlayerBehaviorAttack::kConstAttacks_ = {{
	{0, 0, 20, 10, 0.0f, 0.0f, 0.15f}, 
	{0, 0, 25, 10, 0.2f, 0.0f, 0.15f},
	{0, 20, 15, 30, 0.2f, 0.0f, 0.0f}
	/* 振りかぶりの時間<frame>, 溜め時間<frame>, 攻撃振り時間<frame>, 硬直時間<frame>, 振りかぶりの移動速度, 溜めの移動速度, 攻撃振り移動速度 */
}};

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBehavior inheritance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBehaviorRoot::Init() {
	behavior_ = Behavior::kRoot;

	InitFloatingGimmick();
	player_->InitModelTransfomrs();
}

void PlayerBehaviorRoot::Update() {
	ApplyGlobalVariables();
	Action();
	UpdateFloatingGimmick();
}

void PlayerBehaviorRoot::ApplyGlobalVariables() {
	moveSpeed_ = globalVariables->GetValue<float>(groupName_, "move speed");
}

void PlayerBehaviorRoot::InitFloatingGimmick() {
	floatingParameter_ = 0.0f;
}

void PlayerBehaviorRoot::UpdateFloatingGimmick() {
	// パラメーター分増加
	floatingParameter_ += floatingStep_;

	// 0 ~ 2pi範囲内で抑える
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * pi_v);

	// 浮遊させる
	player_->modelTransforms_[Player::MODEL_BODY].translation_.y = std::sin(floatingParameter_) * floatingRange_;

	// 手の動き
	player_->modelTransforms_[Player::MODEL_LARM].rotation_.x = std::sin(floatingParameter_) * 0.1f;
	player_->modelTransforms_[Player::MODEL_RARM].rotation_.x = std::sin(floatingParameter_) * 0.1f;
}

void PlayerBehaviorRoot::Action() {

	XINPUT_STATE joyState;

	// コントローラーでの移動
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) { //!< コントローラーが接続されてない場合
		return;
	}
		

	Vector3f move = {
		static_cast<float>(joyState.Gamepad.sThumbLX) / SHRT_MAX,
		0.0f,
		static_cast<float>(joyState.Gamepad.sThumbLY) / SHRT_MAX
	};

	// デッドゾーンの確認
	if (Vector::Length(move) > kDeadZone_) {

		move = Vector::Normalize(move) * moveSpeed_;

		// 移動処理
		player_->velocity_ = Matrix::TransformNormal(move, Matrix::MakeRotate(player_->viewProj_->rotation_.y, kRotateBaseY));

		// 移動方向を代入
		player_->direction_ = player_->velocity_;

	} else if (player_->lockOn_ && player_->lockOn_->GetTargetPosition().has_value()) { //!< スティックによる移動入力がない時 && ロックオンの対象がいる場合
	
		Vector3f lockOnPos = player_->lockOn_->GetTargetPosition().value();

		player_->direction_ = lockOnPos - player_->worldTransform_.translation_;

	}

	// hack: 別関数に分けたらよくなる...?
	// コントローラーでの攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) { //!< Bが押された場合
		player_->behaviorRequest_ = Behavior::kAttack; //!< 攻撃状態へのリクエスト
	}

	// コントローラーでのジャンプ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) { //!< Aが押されたとき
		player_->behaviorRequest_ = Behavior::kJump; //!< ジャンプ状態へのリクエスト
	}

	// コントローラでのダッシュ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) { //!< Yが押されたとき
		player_->behaviorRequest_ = Behavior::kDash;
	}
	
}

void PlayerBehaviorAttack::Init() {
	behavior_ = Behavior::kAttack;

	attackParameter_ = 0.0f;
	comboIndex_  = 0;
	inComboPhase = 0;
	isNextCombo  = false;

	InitCombo();

	player_->hammer_->ClearRecord();
}

void PlayerBehaviorAttack::Update() {

	UpdateCombo();

	// inputの取得
	auto input = Input::GetInstance();

	// 次のコンボに進むかどうか
	if (comboIndex_ < kComboNum_ - 1) { //!< コンボの上限に達していない場合

		//! [pair]
		//! first : 現在のゲームパッド状態
		//! second: 前フレームのゲームパッド状態
		std::pair<XINPUT_STATE, XINPUT_STATE> joyState;

		//!< 現在と前フレームのゲームパッドの状態取得
		if (input->GetJoystickState(0, joyState.first) && input->GetJoystickStatePrevious(0, joyState.second)) {
			//!< 現在と前フレームのゲームパッドの状態が取得できた場合
			
			bool isTriggerAttackButton
				= (joyState.first.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(joyState.second.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			//!< attackButton(Bボタン)がtriggerかどうかの確認.
			
			if (isTriggerAttackButton) { //!< triggerだった場合
				// コンボ有効
				isNextCombo = true;
			}
		}
	}

	uint32_t totalAttickTime
		= kConstAttacks_[comboIndex_].anticipationTime
		+ kConstAttacks_[comboIndex_].chargeTime
		+ kConstAttacks_[comboIndex_].swingTime
		+ kConstAttacks_[comboIndex_].recoveryTime;

	// 規定の時間経過で通常行動に戻る
	if (++attackParameter_ >= totalAttickTime) {
		// コンボ継続なら次のコンボに進む
		if (isNextCombo) {
			// 攻撃にかかわる変数のリセット
			isNextCombo      = false;
			attackParameter_ = 0.0f;
			comboIndex_++; //!< コンボ数の加算

			// この瞬間だけ方向転換できる
			XINPUT_STATE joyState;

			if (input->GetJoystickState(0, joyState)) {
				Vector3f move = {
					static_cast<float>(joyState.Gamepad.sThumbLX) / SHRT_MAX,
					0.0f,
					static_cast<float>(joyState.Gamepad.sThumbLY) / SHRT_MAX
				};

				player_->direction_ = Matrix::TransformNormal(move, Matrix::MakeRotate(player_->viewProj_->rotation_.y, kRotateBaseY));
			}

			// TODO: 次のコンボ用に各パーツの角度をここでリセット
			InitCombo();

		} else {
			// コンボに進まないので通常状態に戻る
			player_->behaviorRequest_ = Behavior::kRoot;
		}
	}

}

void PlayerBehaviorAttack::InitCombo() {
	switch (comboIndex_) {
		case 0:
			break;

		case 1:
			break;

		case 2:
		default:

			break;
	}
}

void PlayerBehaviorAttack::UpdateCombo() {

	//* combo 0: 右から左に振り
	//* combo 1: 左から一周振り
	//* combo 2: 上から振り

	// 攻撃情報を参照取得
	const auto& kConstAttack = kConstAttacks_[comboIndex_];

	if (attackParameter_ < kConstAttack.anticipationTime) {
		ComboAnticipation(attackParameter_ / kConstAttack.anticipationTime);

	} else if (attackParameter_ - kConstAttack.anticipationTime < kConstAttack.chargeTime) {
		ComboCharge((attackParameter_ - kConstAttack.anticipationTime) / kConstAttack.chargeTime);

	} else if (attackParameter_ - kConstAttack.anticipationTime - kConstAttack.chargeTime < kConstAttack.swingTime) {
		ComboSwing((attackParameter_ - kConstAttack.anticipationTime - kConstAttack.chargeTime) / kConstAttack.swingTime);

	} else if (attackParameter_ - kConstAttack.anticipationTime - kConstAttack.chargeTime - kConstAttack.swingTime < kConstAttack.recoveryTime) {
		ComboRecovery((attackParameter_ - kConstAttack.anticipationTime - kConstAttack.chargeTime - kConstAttack.swingTime) / kConstAttack.recoveryTime);
	}
}

void PlayerBehaviorAttack::ComboAnticipation([[maybe_unused]]float t) {
	switch (comboIndex_) {
		case 0:
			break;

		case 1:
			break;

		case 2:
		default:

			break;
	}
}

void PlayerBehaviorAttack::ComboCharge([[maybe_unused]]float t) {
	switch (comboIndex_) {
		case 0:
			break;

		case 1:
			break;

		case 2:
	    default: {
				
			}
			break;
	}
}

void PlayerBehaviorAttack::ComboSwing([[maybe_unused]]float t) {

	if (t == 0.0f) { //!< hack: 最初の1frameだけ通る
		attackMoveSpeed_ = kConstAttacks_[comboIndex_].swingSpeed;
	}

	switch (comboIndex_) {
		case 0: {
				Vector3f rotation = {0.0f};
				rotation.y = std::lerp(pi_v / 2.0f, -pi_v / 2.0f, t);
				rotation.x = pi_v / 2.0f;

				player_->hammer_->SetRotation(rotation);

				for (auto& transform : player_->modelTransforms_) {
			        transform.rotation_.y = rotation.y;
		        }
			}
			break;

		case 1: {
				Vector3f rotation = {0.0f};
				rotation.y = -pi_v / 2.0f + std::lerp(0.0f, pi_v * 2.0f, t); //!< 一周
				rotation.x = pi_v / 2.0f;

				player_->hammer_->SetRotation(rotation);

		        for (auto& transform : player_->modelTransforms_) {
			        transform.rotation_.y = rotation.y;
		        }
			}
			break;

		case 2:
		default: {

				float easeT = EaseOutBounce(t);

				Vector3f rotation = {0.0f};
		        rotation.x = std::lerp(0.0f, pi_v / 2.0f, easeT);

				player_->hammer_->SetRotation(rotation);

				for (auto& transform : player_->modelTransforms_) {
			        transform.rotation_.y = 0.0f;
		        }

				player_->modelTransforms_[Player::MODEL_LARM].rotation_.x = pi_v + rotation.x;
		        player_->modelTransforms_[Player::MODEL_RARM].rotation_.x = pi_v + rotation.x;
			}
			break;
	}

	player_->hammer_->Update();

	// ロックオン中, directionをlockOnしてる敵のほうに向かせる
	if (player_->lockOn_ && player_->lockOn_->GetTargetPosition().has_value()) {
		Vector3f lockOnPos = player_->lockOn_->GetTargetPosition().value();
		Vector3f sub = lockOnPos - player_->worldTransform_.translation_;

		// 距離
		float distance = Vector::Length({sub.x, 0.0f, sub.z});

		// 距離のしきい値
		const float threshold = 1.0f;

		if (distance > threshold) { //!< しきい値より離れてる場合のみ

			player_->direction_ = sub;

			if (attackMoveSpeed_ > distance - threshold) { //!< しきい値を超える速さなら補正
				attackMoveSpeed_ = 0.0f;
			}
		}
	}

	// 向いてる方向に少しずつ移動
	player_->velocity_ += Vector::Normalize({player_->direction_.x, 0.0f, player_->direction_.z}) * attackMoveSpeed_;
}

void PlayerBehaviorAttack::ComboRecovery([[maybe_unused]]float t) {
	switch (comboIndex_) {
		case 0:
			break;

		case 1:
			break;

		case 2:
		default:

			break;
	}
}

void PlayerBehaviorJump::Init() {
	behavior_ = Behavior::kJump;

	player_->modelTransforms_[Player::MODEL_BODY].translation_.y = 0.0f;
	player_->modelTransforms_[Player::MODEL_LARM].rotation_.x = 0.0f;
	player_->modelTransforms_[Player::MODEL_RARM].rotation_.x = 0.0f;

	// ジャンプの初速
	const float kJumpFirstSpeed = 1.0f;

	// ジャンプの初速を与える
	player_->velocity_.y = kJumpFirstSpeed;
	player_->worldTransform_.translation_ += player_->velocity_;
	// HACK: Update後に "translation += velocity"があるので始め1frameだけ加算しておく
}

void PlayerBehaviorJump::Update() {
	// todo: ジャンプ中の移動を入れる

	if (player_->worldTransform_.translation_.y <= 0.0f) { //!< 地面(y = 0)地点についたらジャンプの終了
		player_->worldTransform_.translation_.y = 0.0f;
		player_->velocity_.y = 0.0f;
		player_->behaviorRequest_ = Behavior::kRoot; //!< ジャンプの終了
		return;
	}

	// 重力加速度
	const float kGravityAcceleration = 0.05f;

	// 加速度ベクトル
	Vector3f acceleration = {0.0f, -kGravityAcceleration, 0.0f};

	player_->velocity_ += acceleration;
}

void PlayerBehaviorDash::Init() {
	behavior_ = Behavior::kDash;

	dashParameter_ = 0;

	// ダッシュするので強制的に自機視点に向ける
	player_->worldTransform_.rotation_.y = player_->targetAngle_;
}

void PlayerBehaviorDash::Update() {

	// 自キャラの向いてる方向に移動する処理
	Vector3f move = {0.0f, 0.0f, 1.0f}; // z方向にダッシュ
	move *= kDashSpeed_;

	// 移動処理
	player_->velocity_ = Matrix::TransformNormal(move, Matrix::MakeRotate(player_->worldTransform_.rotation_.y, kRotateBaseY));

	// 移動方向を代入
	player_->direction_ = player_->velocity_;

	// 規定の時間となったら通常攻撃に戻る
	if (++dashParameter_ >= kDashTime_) {
		player_->behaviorRequest_ = Behavior::kRoot;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(const std::vector<Model*>& models) {
	Collider::Init();
	Collider::SetTypeId(CollisionTypeIdDef::kPlayer);

	assert(models.size() == kCountOfModelType);

	// worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 30.0f};

	models_ = models;

	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].Initialize();
	}

	InitModelTransfomrs();

	// グループの追加
	const std::string groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "lArm translation", modelTransforms_[MODEL_LARM].translation_);
	globalVariables->AddItem(groupName, "rArm translation", modelTransforms_[MODEL_RARM].translation_);

	hammer_ = std::make_unique<Hammer>();

}

void Player::Term() {}

void Player::Update() {

	ApplyGlobalVariables();

	// 移動方向へのvelocityのリセット
	velocity_.x = 0.0f;
	velocity_.z = 0.0f;

	// behaviorによる更新
	UpdateBehavior();

	// 移動
	worldTransform_.translation_ += velocity_;

	direction_ = Vector::Normalize(direction_);

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
		models_[i]->Draw(modelTransforms_[i], viewProj);
	}

	if (behavior_->GetBehavior() == Behavior::kAttack) { //!< 攻撃中の場合
		hammer_->Draw(viewProj);
	}
}

void Player::OnCollision([[maybe_unused]]Collider* other) {
	behaviorRequest_ = Behavior::kJump;
}

Vector3f Player::GetCenterPosition() const {
	const Vector3f offset = {0.0f, 1.5f, 0.0f};
	return Matrix::Transform(offset, worldTransform_.matWorld_);
}

void Player::UpdateBehavior() {

	if (behaviorRequest_) { //!< 次行動へのリクエストがある場合
		switch (behaviorRequest_.value()) {
			case Behavior::kRoot:
			default:
				behavior_ = std::make_unique<PlayerBehaviorRoot>(this);
				break;

			case Behavior::kAttack:
				behavior_ = std::make_unique<PlayerBehaviorAttack>(this);
				break;

			case Behavior::kJump:
				behavior_ = std::make_unique<PlayerBehaviorJump>(this);
				break;

			case Behavior::kDash:
				behavior_ = std::make_unique<PlayerBehaviorDash>(this);
				break;

		}

		behaviorRequest_ = std::nullopt;
	}

	behavior_->Update();
}

void Player::ApplyGlobalVariables() {

	const std::string groupName = "Player";

	modelTransforms_[MODEL_LARM].translation_ = globalVariables->GetValue<Vector3f>(groupName, "lArm translation");
	modelTransforms_[MODEL_RARM].translation_ = globalVariables->GetValue<Vector3f>(groupName, "rArm translation");

}

void Player::InitModelTransfomrs() {

	for (auto& transform : modelTransforms_) {
		transform.scale_ = {1.0f, 1.0f, 1.0f};
		transform.rotation_ = {};
		transform.translation_ = {};
	}

	// parts位置調整
	modelTransforms_[MODEL_BODY].SetParent(&worldTransform_); //!< world -> this

	modelTransforms_[MODEL_HEAD].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this

	modelTransforms_[MODEL_LARM].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this
	modelTransforms_[MODEL_LARM].translation_ = {-1.4f, 2.5f, 0.0f};

	modelTransforms_[MODEL_RARM].SetParent(&modelTransforms_[MODEL_BODY]); //!< world -> body -> this
	modelTransforms_[MODEL_RARM].translation_ = {1.4f, 2.5f, 0.0f};
}


