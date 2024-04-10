#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include <cassert>

//=========================================================================================
// static variables
//=========================================================================================

void (Enemy::*Enemy::Action[])() = {
	&Enemy::Approach, //!< 接近する
	&Enemy::Leave,    //!< 離脱する
};

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, uint32_t textureHandle) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 5.0f, 5.0f}; //!< 初期座標の設定

	worldTransform_.UpdateMatrix();

}

void Enemy::Update() {

	// 関数ptr
	(this->*Action[static_cast<size_t>(phase_)])();

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
}

void Enemy::SetOnImGui() {
	if (ImGui::CollapsingHeader("Enemy")) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
	}
}

//=========================================================================================
// private methods
//=========================================================================================

void Enemy::Approach() {
	worldTransform_.translation_ += { 0.0f, 0.0f, -kMoveSpeed_ / 2.0f };

	// 規定位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave() {
	worldTransform_.translation_ += { 0.0f, kMoveSpeed_, kMoveSpeed_ };
}
