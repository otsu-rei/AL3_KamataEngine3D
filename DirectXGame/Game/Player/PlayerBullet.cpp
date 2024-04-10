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
