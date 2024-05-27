#include "EnemyBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include "MyMath.h"
#include <cassert>

#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBullet::Init(Model* model, const Vector3f& position, const Vector3f& velocity) {
	assert(model);

	model_ = model;
	textureHandle_ = TextureManager::Load("sample.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	worldTransform_.scale_ = {0.5f, 0.5f, 3.0f};

	velocity_ = velocity;

	// collider
	SetRadius(kCollisionRadius_);
	SetCollisionAttribute(ENEMY_BULLET);
	SetCollisionMask(PLAYER | PLAYER_BULLET);
}

void EnemyBullet::Update() {
	if (--deathTimer_ < 0) {
		isDead_ = true;
	}

	Vector3f toPlayer = player_->GetWorldPosition() - worldTransform_.translation_;

	toPlayer = Vector::Normalize(toPlayer);
	velocity_ = Vector::Normalize(velocity_);


	velocity_ = Vector::Slerp(velocity_, toPlayer, kHomingRaito_) * kBulletSpeed_;

	worldTransform_.translation_ += velocity_;

	// ロール, ピッチ, ロー回転
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	float length = Vector::Length({velocity_.x, 0.0f, velocity_.z});
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, length);

	worldTransform_.UpdateMatrix();

}

void EnemyBullet::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
	//
}

void EnemyBullet::OnCollision() { isDead_ = true; }

Vector3f EnemyBullet::GetWorldPosition() const {
	Vector3f result;
	result.x = worldTransform_.matWorld_.m[3][0];
	result.y = worldTransform_.matWorld_.m[3][1];
	result.z = worldTransform_.matWorld_.m[3][2];

	return result;
}


