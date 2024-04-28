#include "WorldTransform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MyMath.h"

////////////////////////////////////////////////////////////////////////////////////////////
// WorldTransform class user methods
////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::UpdateMatrix() {
	matWorld_ = Matrix::MakeAffine(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	TransferMatrix();
}

Vector3f WorldTransform::GetTransform() const {
	Vector3f result;
	result.x = matWorld_.m[3][0];
	result.y = matWorld_.m[3][1];
	result.z = matWorld_.m[3][2];

	return result;
}
