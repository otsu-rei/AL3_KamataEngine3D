#include "Collider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Init() { worldTransform_.Initialize(); }

void Collider::Draw(Model* model, const ViewProjection& viewProj) {
	// debug用途で使うはず...
	model->Draw(worldTransform_, viewProj);
}

void Collider::UpdateWorldTransform() {
	worldTransform_.translation_ = GetCenterPosition();
	worldTransform_.scale_ = {collisionRadius_, collisionRadius_, collisionRadius_};
	worldTransform_.UpdateMatrix();
}
