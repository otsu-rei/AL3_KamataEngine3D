#include "Skydome.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// Skydome class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skydome::Init(Model* model) {
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.scale_ = { 100.0f, 100.0f, 100.0f };
	worldTransform_.UpdateMatrix();

}

void Skydome::Term() { model_ = nullptr; }

void Skydome::Update() {
	/*worldTransform_.UpdateMatrix();*/
	//
}

void Skydome::Draw(const ViewProjection& viewProj) {
	model_->Draw(worldTransform_, viewProj);
	//
}
