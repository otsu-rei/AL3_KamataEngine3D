#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include <cassert>

#include "MyMath.h"
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, uint32_t textureHandle) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 5.0f, 20.0f}; //!< 初期座標の設定

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
	if (ImGui::TreeNode("Enemy")) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
		ImGui::TreePop();
	}
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	//
}

void Enemy::OnCollision() {}

Vector3f Enemy::GetWorldPosition() const {
	Vector3f result;

	result.x = worldTransform_.matWorld_.m[3][0];
	result.y = worldTransform_.matWorld_.m[3][1];
	result.z = worldTransform_.matWorld_.m[3][2];

	return result;
}

void Enemy::Action() { state_->Update(); }

void Enemy::Fire() {

	assert(player_);
	
	Vector3f direction = player_->GetWorldPosition() - GetWorldPosition();
	direction = Vector::Normalize(direction);

	Vector3f velocity = direction * kBulletSpeed_;
	
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();

	newBullet->Init(model_, worldTransform_.translation_, velocity);

	bullets_.push_back(std::move(newBullet));
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class methods
////////////////////////////////////////////////////////////////////////////////////////////

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState(enemy) { Init(); }

EnemyStateApproach::~EnemyStateApproach() { timeCalls_.clear(); }

void EnemyStateApproach::Init() { SetFire(); }

void EnemyStateApproach::Update() {
	// Approach Update
	// 敵機の移動
	Vector3f enemyPos = enemy_->GetWorldPosition();
	enemyPos += {0.0f, 0.0f, -enemy_->kMoveSpeed_ / 2.0f};
	enemy_->SetPos(enemyPos);

	for (auto& timedCall : timeCalls_) {
		timedCall->Update();
	}

	// 次のstateの遷移条件
	if (enemy_->GetWorldPosition().z < 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}

	
}

void EnemyStateApproach::SetFire() {
	enemy_->Fire();
	std::unique_ptr<TimedCall> newTimedCall
		= std::make_unique<TimedCall>(std::bind(&EnemyStateApproach::SetFire, this), kFireInterval);

	timeCalls_.push_back(std::move(newTimedCall));
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

	Vector3f enemyPos = enemy_->GetWorldPosition();

	enemyPos += {0.0f, enemy_->kMoveSpeed_, enemy_->kMoveSpeed_};

	enemy_->SetPos(enemyPos);
}
