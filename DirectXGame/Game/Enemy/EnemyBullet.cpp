#include "EnemyBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////
void EnemyBullet::Init(Model* model, const Vector3f& position, const Vector3f& velocity) {
	assert(model);

	model_ = model;
	textureHandle_ = TextureManager::Load("sample.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	velocity_ = velocity;
}

void EnemyBullet::Update() {
	if (--deathTimer_ < 0) {
		isDead_ = true;
	}

	worldTransform_.translation_ += velocity_;

	worldTransform_.UpdateMatrix();

}

void EnemyBullet::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, textureHandle_);
	//
}


