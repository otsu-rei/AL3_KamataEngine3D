#include "Field.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Field class
////////////////////////////////////////////////////////////////////////////////////////////

void Field::Init() {

	transform_.Initialize();
	transform_.scale_ *= 8.0f * 2.0f;

	transform_.UpdateMatrix();

	//!< 面の初期化
	for (auto& face : faces_) {
		face = std::make_unique<FieldFace>();
	}

	faces_[kLeftFace]->Init({1.0f, 0.0f, 0.0f}, transform_.scale_ / 2.0f);
	faces_[kRightFace]->Init({-1.0f, 0.0f, 0.0f}, transform_.scale_ / 2.0f);
	faces_[kTopFace]->Init({0.0f, -1.0f, 0.0f}, transform_.scale_ / 2.0f);
	faces_[kBottomFace]->Init({0.0f, 1.0f, 0.0f}, transform_.scale_ / 2.0f);
	faces_[kFrontFace]->Init({0.0f, 0.0f, 1.0f}, transform_.scale_ / 2.0f);
	faces_[kBackFace]->Init({0.0f, 0.0f, -1.0f}, transform_.scale_ / 2.0f);

	model_.reset(Model::CreateFromOBJ("field"));

	shadowTransform_.Initialize();
	viewProj_.Initialize();

}

void Field::Draw(const ViewProjection& viewProj) {
	model_->Draw(transform_, viewProj); }

void Field::DrawShadow(const ViewProjection& viewProj) {
	shadowTransform_.matWorld_ = transform_.matWorld_ * Matrix::MakeAffine({1.0f, 1.0f, 0.0f}, {0.0f}, {0.0f, 0.0f, 10.0f});
	shadowTransform_.TransferMatrix();
	viewProj;
	model_->Draw(shadowTransform_, viewProj_);
}
