#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include <cassert>

#include "MyMath.h"

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

	// deathフラグの立った弾の削除
	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			return true;
		}

		return false;
	});

	Action();

	worldTransform_.UpdateMatrix();

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

}

void Enemy::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);

	for (auto& bullet : bullets_) {
		bullet->Draw(viewProj);
	}



}

void Enemy::Term() {
	bullets_.clear();
	//
}

void Enemy::SetOnImGui() {
	if (ImGui::CollapsingHeader("Enemy")) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
	}
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	//
}

void Enemy::Action() { state_->Update(); }

void Enemy::Fire() {
	
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();

	// 弾の進行方向の設定
	Vector3f velocity = {0.0f, 0.0f, -kBulletSpeed_};
	velocity = Matrix::TransformNormal(velocity, worldTransform_.matWorld_); //!< 向きに合わせる

	newBullet->Init(model_, worldTransform_.translation_, velocity);

	bullets_.push_back(std::move(newBullet));
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class methods
////////////////////////////////////////////////////////////////////////////////////////////

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState(enemy) { Init(); }

void EnemyStateApproach::Init() {
	fireTimer_ = kFireInterval;
	//
}

void EnemyStateApproach::Update() {
	// Approach Update
	// 敵機の移動
	Vector3f enemyPos = enemy_->GetPos();

	enemyPos += {0.0f, 0.0f, -enemy_->kMoveSpeed_ / 2.0f};

	enemy_->SetPos(enemyPos);

	// 弾の発射
	if (--fireTimer_ <= 0) {
		enemy_->Fire();
		Init();
	}

	// 次のstateの遷移条件
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
