#include "PlayerBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init(Model* model, const Vector3f& position, const Vector3f& velocity) {
	assert(model);
	
	model_ = model;
	textureHandle_ = TextureManager::Load("white1x1.png");
	
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	velocity_ = velocity;

	SetRadius(kCollisionRadius_);
}

void PlayerBullet::Update() {
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translation_ += velocity_;

	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProj) { model_->Draw(worldTransform_, viewProj, textureHandle_); }

void PlayerBullet::OnCollision() { isDead_ = true; }

Vector3f PlayerBullet::GetWorldPosition() const {
	Vector3f result;
	result.x = worldTransform_.matWorld_.m[3][0];
	result.y = worldTransform_.matWorld_.m[3][1];
	result.z = worldTransform_.matWorld_.m[3][2];

	return result;
}
