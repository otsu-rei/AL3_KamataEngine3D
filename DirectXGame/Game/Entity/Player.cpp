#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	model_.reset(Model::CreateFromOBJ("Cube"));

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
}

void Player::Term() {}

void Player::Update() {
	Action();

	transform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProj) {
	model_->Draw(transform_, viewProj);
}

void Player::OnCollisionEnter(MAYBE_UNUSED Collider* const other) {
	transform_.translation_ = beforeTranslation_;
	transform_.UpdateMatrix();

	//!< todo: 押し戻し処理

	SetCollisionState(other, false);
}

void Player::Action() {

	beforeTranslation_ = transform_.translation_;
	
	XINPUT_STATE state = {};

	if (!input_->GetJoystickState(0, state)) { //!< コントローラーが接続されてない場合,
		return;
	}

	Vector2f stickL = {
		static_cast<float>(state.Gamepad.sThumbLX) / SHRT_MAX,
		static_cast<float>(state.Gamepad.sThumbLY) / SHRT_MAX,
	};

	transform_.translation_.x += stickL.x;
	transform_.translation_.z += stickL.y;

}
