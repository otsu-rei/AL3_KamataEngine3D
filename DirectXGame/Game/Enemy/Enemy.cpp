#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include <cassert>

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

	worldTransform_.translation_ += {0.0f, 0.0f, -kMoveSpeed_};
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
