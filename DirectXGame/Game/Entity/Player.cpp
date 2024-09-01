#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include <imgui.h>
#include <format>
#include <MyMath.h>
#include <PrimitiveDrawer.h>
#include <GameCamera.h>

#include "ColliderManager.h"
#include "AudioManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	model_.reset(Model::CreateFromOBJ("Player"));

	transform_.Initialize();
	transform_.scale_ = {1.0f, 1.0f, 1.0f};
	transform_.translation_.z = 3.0f;
	transform_.UpdateMatrix();

	Collider::SetColliderBoundingAABB({
	    .localMin = -transform_.scale_,
	    .localMax = transform_.scale_,
	});

	Collider::typeId_       = kPlayer;
	Collider::targetTypeId_ = kBox;

	//* UI *//

	plane_.reset(Model::CreateFromOBJ("buttonB"));
	planeTransform_.Initialize();
	planeTransform_.SetParent(&transform_);

	demoTransform_.Initialize();
	demoColor_.Initialize();
	demoColor_.SetColor({1.0f, 1.0f, 1.0f, 0.5f});
	demoColor_.TransferMatrix();
}

void Player::Term() {}

void Player::Update() {

	Action();

	//DebugCommand();

	acceleration_ = kGravity * gravityDireciton_;

	velocity_ += acceleration_ * deltaTime;
	transform_.translation_ += velocity_ * deltaTime;

	transform_.UpdateMatrix();

	//* UI *//
	
	planeTransform_.matWorld_ = Matrix::MakeTranslate(planeOffset_) * Matrix::MakeRotate(gameCamera_->GetQuaternion());
	planeTransform_.UpdateBaseMat();
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(transform_, viewProj);

	if (canStick_) {
		plane_->Draw(planeTransform_, viewProj);
		model_->Draw(demoTransform_, viewProj, &demoColor_);
	}
	
}

void Player::DrawLine() {
	auto drawer = PrimitiveDrawer::GetInstance();
	drawer->DrawLine3d(GetColliderPosition(), GetColliderPosition() + direction_ * stickLength, {0.0f, 1.0f, 0.0f, 1.0f});
}

void Player::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {

	// 押し戻し処理
	// 面の上側に当たっているか確認
	// HACK: 他のtarget追加した場合, 型不明で例外となる
	
	// otherのboundingの取得
	const auto& otherBounding = other->GetBounding();

	if (std::holds_alternative<CollisionBoundings::AABB>(otherBounding)) {
		const auto& otherAABB = std::get<CollisionBoundings::AABB>(otherBounding);

		PushBackTrnaslate(other->GetColliderPosition(), otherAABB);
		transform_.UpdateMatrix();
	}

	SetCollisionState(other, false);
}

void Player::SetGrabity(const Vector3f& gravity) {
	gravityDireciton_ = gravity;
}

void Player::DebugImGui() const {

	if (ImGui::TreeNode("player")) {

		{
			std::string txt
				= std::format("gravityDirection_ x: {}, y: {}, z: {}, isTouchingGround_: {}", gravityDireciton_.x, gravityDireciton_.y, gravityDireciton_.z, isTouchingGround_);

			ImGui::Text(txt.c_str());

			ImGui::Text("direction_ x: %f, y: %f, z: %f", direction_.x, direction_.y, direction_.z);
		}

		ImGui::TreePop();
	}

}

void Player::Action() {
	
	XINPUT_STATE preState = {};
	XINPUT_STATE state    = {};

	if (!input_->GetJoystickState(0, state) || !input_->GetJoystickStatePrevious(0, preState)) { //!< コントローラーが接続されてない場合,
		return;
	}

	Move(state);
	Jump(state);
	Stick(state, preState);
}

void Player::Move(const XINPUT_STATE& state) {

	Vector2f stickL = {
	    static_cast<float>(state.Gamepad.sThumbLX) / SHRT_MAX,
	    static_cast<float>(state.Gamepad.sThumbLY) / SHRT_MAX,
	};

	stickL = Vector::Normalize(stickL);

	//!< カメラの位置によるカメラの移動

	/*velocity_.x = stickL.x * moveSpeed;
	velocity_.z = stickL.y * moveSpeed;*/

	Vector3f move = {stickL.x * moveSpeed, 0.0f, stickL.y * moveSpeed};

	if (gameCamera_ == nullptr) { //!< gameCameraが設定されてない場合
		return;
	}

	move = Matrix::TransformNormal(move, Matrix::MakeRotate(gameCamera_->GetTargetQuaternion()));

	/*velocity_ = move;*/

	if (gravityDireciton_.x == 0.0f) {
		velocity_.x = move.x;
	}

	if (gravityDireciton_.y == 0.0f) {
		velocity_.y = move.y;
	}

	if (gravityDireciton_.z == 0.0f) {
		velocity_.z = move.z;
	}

	direction_ = Vector::Normalize(move);
}

void Player::Jump(const XINPUT_STATE& state) {
	//!< ジャンプ処理
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		if (isTouchingGround_) { //!< 地面についている状態の時
			velocity_ += jumpPower * -gravityDireciton_;
			isTouchingGround_ = false;
		}
	}
}



void Player::Stick(const XINPUT_STATE& state, const XINPUT_STATE& preState) {

	canStick_ = false;
	demoTransform_.translation_ = {};

	if (All(direction_ == Vector3f{0.0f, 0.0f, 0.0f})) { //!< コントローラ操作がない
		return;
	}

	if (!isTouchingGround_) { //!< 地面についてないので判定を行わない
		return;
	}

	MathCollider::Segment segment = {
		.origin = GetColliderPosition(),
		.diff = direction_ * stickLength
	};

	auto manager = ColliderManager::GetInstance();

	// result
	Vector3f hitNormal = {};

	for (const auto& collider : manager->GetColliders()) {
		if (collider->GetID() != kBox) { //!< 障害物ではなので次
			continue;
		}

		//!< kBoxはAABBである決まり
		CollisionBoundings::AABB colliderAABB = std::get<CollisionBoundings::AABB>(collider->GetBounding());

		//!< mathColliderに変換
		MathCollider::AABB aabb = {
			.min = colliderAABB.localMin + collider->GetColliderPosition(),
			.max = colliderAABB.localMax + collider->GetColliderPosition()
		};

		bool isCollision = CheckStickSegmentToAABB(segment, aabb, demoTransform_.translation_, hitNormal);

		if (isCollision) {
			canStick_ = true; //!< stickできる状態
			demoTransform_.translation_ = demoTransform_.translation_ + hitNormal * transform_.scale_;
			demoTransform_.UpdateMatrix();
			break;
		}
	}

	bool isTriggerB = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B);

	if (isTriggerB) {
		if (canStick_) {
			transform_.translation_ = demoTransform_.translation_;
			gravityDireciton_ = -hitNormal;
			AudioManager::GetInstance()->PlayAudio("rotation");

		} else {
			AudioManager::GetInstance()->PlayAudio("cannot");
		}
	}
}

void Player::PushBackTrnaslate(const Vector3f& otherPosition, const CollisionBoundings::AABB& otherAABB) {

	isTouchingGround_ = false; //!< 地面についているかどうかの判定もここで取る
	
	Vector3f pushback = {};
	
	const auto& thisAABB     = std::get<CollisionBoundings::AABB>(Collider::GetBounding());
	const Vector3f& position = transform_.translation_;
	
	Vector3f overlap = {
		(std::min)(thisAABB.localMax.x + position.x, otherAABB.localMax.x + otherPosition.x) - (std::max)(thisAABB.localMin.x + position.x, otherAABB.localMin.x + otherPosition.x),
		(std::min)(thisAABB.localMax.y + position.y, otherAABB.localMax.y + otherPosition.y) - (std::max)(thisAABB.localMin.y + position.y, otherAABB.localMin.y + otherPosition.y),
		(std::min)(thisAABB.localMax.z + position.z, otherAABB.localMax.z + otherPosition.z) - (std::max)(thisAABB.localMin.z + position.z, otherAABB.localMin.z + otherPosition.z),
	};

	if (overlap.x < overlap.y && overlap.x < overlap.z) {
		if (thisAABB.localMax.x + position.x > otherAABB.localMax.x + otherPosition.x) {
			pushback.x = overlap.x; //!< 右面

			if (gravityDireciton_.x == -1.0f) { //!< 地面が右面の場合
				isTouchingGround_ = true;
			}

		} else {
			pushback.x = -overlap.x; //!< 左面
			
			if (gravityDireciton_.x == 1.0f) { //!< 地面が左面の場合
				isTouchingGround_ = true;
			}
		}

		velocity_.x = 0.0f;

	} else if (overlap.y < overlap.x && overlap.y < overlap.z) {
		if (thisAABB.localMax.y + position.y > otherAABB.localMax.y + otherPosition.y) {
			pushback.y = overlap.y; //!< 下面

			if (gravityDireciton_.y == -1.0f) { //!< 地面が下面の場合
				isTouchingGround_ = true;
			}

		} else {
			pushback.y = -overlap.y; //!< 上面

			if (gravityDireciton_.y == 1.0f) { //!< 地面が上面の場合
				isTouchingGround_ = true;
			}
		}

		velocity_.y = 0.0f;

	} else {
		if (thisAABB.localMax.z + position.z > otherAABB.localMax.z + otherPosition.z) {
			pushback.z = overlap.z; //!< 裏面

			if (gravityDireciton_.z == -1.0f) {
				isTouchingGround_ = true;
			}

		} else {
			pushback.z = -overlap.z; //!< 前面

			if (gravityDireciton_.z == 1.0f) { //!< 前面が上面の場合
				isTouchingGround_ = true;
			}
		}

		velocity_.z = 0.0f;
	}

	transform_.translation_ = transform_.translation_ + pushback;
}

void Player::DebugCommand() {
	if (input_->PushKey(DIK_W)) {
		gravityDireciton_ = {0.0f, 1.0f, 0.0f};
	}

	if (input_->PushKey(DIK_S)) {
		gravityDireciton_ = {0.0f, -1.0f, 0.0f};
	}

	if (input_->PushKey(DIK_A)) {
		gravityDireciton_ = {-1.0f, 0.0f, 0.0f};
	}

	if (input_->PushKey(DIK_D)) {
		gravityDireciton_ = {1.0f, 0.0f, 0.0f};
	}

	if (input_->PushKey(DIK_UP)) {
		gravityDireciton_ = {0.0f, 0.0f, 1.0f};
	}

	if (input_->PushKey(DIK_DOWN)) {
		gravityDireciton_ = {0.0f, 0.0f, -1.0f};
	}
}

bool Player::CheckStickSegmentToAABB(
	const MathCollider::Segment& segment,
	const MathCollider::AABB& aabb,
	Vector3f& outHitPoint, Vector3f& outHitNormal) {

	Vector3f tmin = (aabb.min - segment.origin) / segment.diff; // aabbのminのt
	Vector3f tmax = (aabb.max - segment.origin) / segment.diff; // aabbのmaxのt

	if (Vector::IsNaN(tmin) || Vector::IsNaN(tmax)) {
		return false; // NaNが含まれている場合
	}

	Vector3f near3 = {
	    (std::min)(tmin.x, tmax.x),
	    (std::min)(tmin.y, tmax.y),
	    (std::min)(tmin.z, tmax.z),
	};

	Vector3f far3 = {
	    (std::max)(tmin.x, tmax.x),
	    (std::max)(tmin.y, tmax.y),
	    (std::max)(tmin.z, tmax.z),
	};

	float tNear = (std::max)((std::max)(near3.x, near3.y), near3.z);
	float tFar = (std::min)((std::min)(far3.x, far3.y), far3.z);

	if (tFar < 0.0f || tNear > 1.0f) {
		return false; // SegmentがAABBの外にある場合
	}

	
	 // SegmentとAABBの交点を計算
	float tHit = (std::max)(0.0f, tNear); // Segmentの始点からの距離を考慮
	outHitPoint = segment.origin + segment.diff * tHit;

	// AABBの法線を取得
	if (outHitPoint.x == aabb.min.x) {
		 outHitNormal = {-1.0f, 0.0f, 0.0f};

	} else if (outHitPoint.x == aabb.max.x) {
		outHitNormal = {1.0f, 0.0f, 0.0f};

	} else if (outHitPoint.y == aabb.min.y) {
		outHitNormal = {0.0f, -1.0f, 0.0f};

	} else if (outHitPoint.y == aabb.max.y) {
		outHitNormal = {0.0f, 1.0f, 0.0f};

	} else if (outHitPoint.z == aabb.min.z) {
		outHitNormal = {0.0f, 0.0f, -1.0f};

	} else if (outHitPoint.z == aabb.max.z) {
		outHitNormal = {0.0f, 0.0f, 1.0f};
	}

	return true;
	
}
