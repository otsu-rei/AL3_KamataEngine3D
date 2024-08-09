#include "Box.h"

void Box::Init() {
	
	model_.reset(Model::CreateFromOBJ("Cube"));

	transform_.Initialize();
	transform_.scale_ = {1.0f, 1.0f, 1.0f};
	transform_.UpdateMatrix();

	Collider::SetColliderBoundingAABB({
		.localMin = -transform_.scale_,
	    .localMax = transform_.scale_,
	});

	Collider::typeId_ = kBox;
}

void Box::Draw(const ViewProjection& viewProj) {
	model_->Draw(transform_, viewProj);
}
