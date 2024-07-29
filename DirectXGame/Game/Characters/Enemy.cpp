#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

//=========================================================================================
// static variables
//=========================================================================================

uint32_t Enemy::nextSirialNumber_ = 0;

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

Enemy::Enemy() {
	serialNumber_ = nextSirialNumber_;
	nextSirialNumber_++;
}

void Enemy::Init(const std::vector<Model*>& models) {
	Collider::Init();
	Collider::SetTypeId(CollisionTypeIdDef::kEnemy);

	assert(models.size() == kCountOfModelType);

	// worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	models_ = models;

	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].Initialize();
	}

	// parts位置調整
	modelTransforms_[MODEL_BODY].SetParent(&worldTransform_); //!< world -> this
	modelTransforms_[MODEL_PROPELLER].SetParent(&worldTransform_);

}

void Enemy::Update() {

	worldTransform_.rotation_.y += 0.02f;

	Vector3f velocity = {0.0f, 0.0f, 0.2f};
	velocity = Matrix::TransformNormal(velocity, Matrix::MakeRotate(worldTransform_.rotation_));

	worldTransform_.translation_ += velocity;
	worldTransform_.UpdateMatrix();

	modelTransforms_[MODEL_PROPELLER].rotation_.y += 0.2f;
	modelTransforms_[MODEL_PROPELLER].rotation_.y = std::fmod(modelTransforms_[MODEL_PROPELLER].rotation_.y, pi_v * 2.0f);

	for (int i = 0; i < kCountOfModelType; ++i) {
		modelTransforms_[i].UpdateMatrix();
	}
}

void Enemy::Draw(const ViewProjection& viewProj) {
	for (int i = 0; i < kCountOfModelType; ++i) {
		models_[i]->Draw(modelTransforms_[i], viewProj);
	}
}

void Enemy::Term() {}

Vector3f Enemy::GetCenterPosition() const { 

	const Vector3f offset = {0.0f, 1.0f, 0.0f};

	return Matrix::Transform(offset, worldTransform_.matWorld_);
}
