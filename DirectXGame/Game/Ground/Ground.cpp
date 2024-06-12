#include "Ground.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ground::Init(Model* model) {
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {100.0f, 100.0f, 100.0f};
	worldTransform_.UpdateMatrix();
}

void Ground::Term() { model_ = nullptr; }

void Ground::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj);
}
