#include "TitleModels.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TitleModels class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TitleModels::Init() {

	models_[JYUU].reset(Model::CreateFromOBJ("JYUU"));
	models_[RYOKU].reset(Model::CreateFromOBJ("RYOKU"));
	models_[MENN].reset(Model::CreateFromOBJ("MENN"));
	models_[TAI].reset(Model::CreateFromOBJ("TAI"));

	const float kMaxLeft = -kDistance_ * static_cast<float>(kCountOfModelType) / 2.0f + kDistance_ / 2.0f;

	for (int i = 0; i < kCountOfModelType; ++i) {
		transforms_[i].Initialize();
		transforms_[i].translation_.x = kMaxLeft + kDistance_ * i;
		transforms_[i].UpdateMatrix();

		quaternions_[i] = Quaternion::Identity();
	}

	
	targetQuaterion_ = Quaternion::Identity();
}

void TitleModels::Term() {}

void TitleModels::Update(GameCamera* camera) {

	targetQuaterion_ = camera->GetTargetQuaternion();

	for (int i = 0; i < kCountOfModelType; ++i) {

		quaternions_[i] = Slerp(quaternions_[i], targetQuaterion_, std::lerp(0.03f, 0.02f, i / static_cast<float>(kCountOfModelType - 1)));

		transforms_[i].matWorld_ = Matrix::MakeAffine({1.0f, 1.0f, 1.0f}, quaternions_[i], transforms_[i].translation_);
		transforms_[i].UpdateBaseMat();
	}
}

void TitleModels::Draw(const ViewProjection& viewProj) {
	for (int i = 0; i < kCountOfModelType; ++i) {
		models_[i]->Draw(transforms_[i], viewProj);
	}
}
