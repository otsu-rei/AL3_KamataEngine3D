#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include "GameScene.h"
#include <cassert>

#include "MyMath.h"
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init(Model* model, uint32_t textureHandle, const Vector3f& pos) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos; //!< 初期座標の設定

	worldTransform_.UpdateMatrix();

	ChangeState(std::make_unique<EnemyStateApproach>(this));

}

void Enemy::Update() {

	Action();

	worldTransform_.UpdateMatrix();

}

void Enemy::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
}

void Enemy::Term() {
}

void Enemy::SetOnImGui(const std::string& id) {
	std::string label = "Enemy" + id;

	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.1f);
		ImGui::TreePop();
	}
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	//
}

void Enemy::OnCollision() { isDead_ = true; }

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
	newBullet->SetPlayer(player_);

	gameScene_->AddEnemyBullet(newBullet);
}

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class methods
////////////////////////////////////////////////////////////////////////////////////////////

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState(enemy) { Init(); }

EnemyStateApproach::~EnemyStateApproach() { timeCalls_.clear(); }

void EnemyStateApproach::Init() {
	SetFire();
	stateChangeTime_ = 50;
}

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
	stateChangeTime_--;
	if (stateChangeTime_ <= 0) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}

	/*if (enemy_->GetWorldPosition().z <= 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}*/
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
