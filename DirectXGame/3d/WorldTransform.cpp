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
