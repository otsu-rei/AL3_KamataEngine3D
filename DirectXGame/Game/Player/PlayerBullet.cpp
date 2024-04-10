#include "PlayerBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TextureManager.h"
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init(Model* model, const Vector3f& position) {
	assert(model);
	
	model_ = model;
	textureHandle_ = TextureManager::Load("white1x1.png");
	
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {
	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProj) { model_->Draw(worldTransform_, viewProj, textureHandle_); }
