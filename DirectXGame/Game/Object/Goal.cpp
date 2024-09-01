#include "Goal.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// player
#include <Player.h>
#include <Easing.h>
#include <AudioManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Goal class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Goal::Init(const Vector3f& position, const Vector3f& goalGravity) {

	// positionの保存
	transform_.Initialize();
	transform_.translation_ = position;
	transform_.scale_ = {0.4f, 0.4f, 0.4f};

	// ゴールできる重力値
	goalGravity_ = goalGravity;

	CalculateRotate();
	transform_.UpdateMatrix();

	// todo: goalモデルの回転
	model_.reset(Model::CreateFromOBJ("Goal"));

	Collider::SetColliderBoundingAABB();

	Collider::typeId_       = kGoal;
	Collider::targetTypeId_ = kPlayer;

	color_.Initialize();
	color_.SetColor({0.1f, 0.1f, 0.1f, 1.0f});
	color_.TransferMatrix();

	effectTransform_.Initialize();
	effectTransform_.translation_ = position;
	effectTransform_.rotation_ = transform_.rotation_;

	effectTransform_.UpdateMatrix();
	effectModel_.reset(Model::CreateFromOBJ("GoalEffect"));

	effectColor_.Initialize();
	effectColor_.SetColor({1.0f, 1.0f, 0.0f, 0.0f});
	effectColor_.TransferMatrix();
}

void Goal::Update() {

	if (!isGoal_) {
		return;
	}

	if (goalAftreTime_ < (60.0f * 3.0f)) {
		goalAftreTime_++;
	}

	float t = EaseOutQuart(goalAftreTime_ / (60.0f * 3.0f));

	effectColor_.SetColor({1.0f, 1.0f, 0.0f, 1.0f - t});
	effectColor_.TransferMatrix();

	effectTransform_.scale_ = {t * 4.0f, t * 4.0f, t * 4.0f};
	effectTransform_.UpdateMatrix();


}

void Goal::Draw(const ViewProjection& viewProj) {
	model_->Draw(transform_, viewProj, &color_);

	if (isGoal_) {
		effectModel_->Draw(effectTransform_, viewProj, &effectColor_);
	}
}

void Goal::UpdateImGui() {

	ImGui::DragFloat3("position", &transform_.translation_.x, 0.01f);

	if (ImGui::IsItemFocused()) {
		effectTransform_.translation_ = transform_.translation_;
	}
	
	transform_.UpdateMatrix();
	effectTransform_.UpdateMatrix();
}

void Goal::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {

	// other == kPlayer

	auto player = dynamic_cast<Player*>(other);
	assert(player != nullptr); //!< player以外に接触

	if (All(goalGravity_ == player->GetGravityDirection())) {
		if (!isGoal_) {
			AudioManager::GetInstance()->PlayAudio("clear");
		}

		isGoal_ = true;
		color_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		color_.TransferMatrix();
	}

	
}

void Goal::SetGravity(const Vector3f& goalGravity) {
	goalGravity_ = goalGravity;

	CalculateRotate();
	transform_.UpdateMatrix();
	effectTransform_.UpdateMatrix();
}

void Goal::SetPosition(const Vector3f& position) { 
	transform_.translation_ = position;
	effectTransform_.translation_ = position;
	transform_.UpdateMatrix();
	effectTransform_.UpdateMatrix();
}

void Goal::ResetFlag() {
	effectTransform_.rotation_ = transform_.rotation_;
	effectTransform_.UpdateMatrix();

	color_.SetColor({0.1f, 0.1f, 0.1f, 1.0f});
	color_.TransferMatrix();

	effectColor_.SetColor({1.0f, 1.0f, 0.0f, 0.0f});
	effectColor_.TransferMatrix();

	goalAftreTime_ = 0;
	isGoal_ = false;
}

void Goal::CalculateRotate() {

	transform_.rotation_ = {};

	if (goalGravity_.x != 0.0f) {
		transform_.rotation_.z = (pi_v / 2.0f) * goalGravity_.x;

	} else if (goalGravity_.y != 0.0f) {
		if (goalGravity_.y == 1.0f) {
			transform_.rotation_.z = pi_v;
		}

		//!< gravityDireciton.y == -1.0f の時は何もしない

	} else if (goalGravity_.z != 0.0f) {
		transform_.rotation_.x = (pi_v / 2.0f) * -goalGravity_.z;
	}
}
