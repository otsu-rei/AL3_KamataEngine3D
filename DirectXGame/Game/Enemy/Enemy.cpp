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
	worldTransform_.translation_ = {0.0f, 5.0f, 10.0f}; //!< 初期座標の設定

	worldTransform_.UpdateMatrix();

	ChangeState(std::make_unique<EnemyStateApproach>(this));

}

void Enemy::Update() {

	state_->Update();

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

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class methods
////////////////////////////////////////////////////////////////////////////////////////////

EnemyStateApproach::EnemyStateApproach(Enemy* enemy)
	: BaseEnemyState(enemy) {
	//
}

void EnemyStateApproach::Update() {
	// Approach Update
	Vector3f enemyPos = enemy_->GetPos();

	enemyPos += {0.0f, 0.0f, -enemy_->kMoveSpeed_ / 2.0f};

	enemy_->SetPos(enemyPos);

	if (enemy_->GetPos().z < 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateLeave class methods
////////////////////////////////////////////////////////////////////////////////////////////

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) 
	: BaseEnemyState(enemy) {
	//
}

void EnemyStateLeave::Update() {
	// Leave Update

	Vector3f enemyPos = enemy_->GetPos();

	enemyPos += {0.0f, enemy_->kMoveSpeed_, enemy_->kMoveSpeed_};

	enemy_->SetPos(enemyPos);
}
