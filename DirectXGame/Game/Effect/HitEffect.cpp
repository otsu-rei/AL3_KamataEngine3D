#include "HitEffect.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// HitEffect class methods
////////////////////////////////////////////////////////////////////////////////////////////

void HitEffect::Init(Model* model, const Vector3f& pos) {
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	objectColor_.Initialize();
	objectColor_.SetColor(color_);
}

void HitEffect::Term() {}

void HitEffect::Update() {
	t_ += kDeltaT_;

	if (t_ >= 1.0f) { //!< effectの終了
		isAlive_ = false;
		return;
	}

	radius_ = std::lerp(0.1f, 6.0f, t_);
	color_.w = std::lerp(1.0f, 0.0f, t_);

	worldTransform_.scale_ = {radius_, radius_, radius_};
	worldTransform_.UpdateMatrix();

	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();
}

void HitEffect::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj, &objectColor_);
}
