#include "Box.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <imgui.h>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Box class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Box::Init(const Vector3f& scale, const Vector3f& position) {

	model_.reset(Model::CreateFromOBJ("Box"));

	transform_.Initialize();
	transform_.scale_       = scale;
	transform_.translation_ = position;
	transform_.UpdateMatrix();

	Collider::SetColliderBoundingAABB({
		.localMin = -transform_.scale_,
	    .localMax = transform_.scale_,
	});

	Collider::typeId_ = kBox;
}

void Box::Draw(const ViewProjection& viewProj) { model_->Draw(transform_, viewProj); }

void Box::SetImGuiCommand() {
	ImGui::DragFloat3("scale",    &transform_.scale_.x, 0.01f);

	if (ImGui::IsItemFocused()) {
		Collider::SetColliderBoundingAABB({
		    .localMin = -transform_.scale_,
		    .localMax = transform_.scale_,
		});
	}

	ImGui::DragFloat3("position", &transform_.translation_.x, 0.01f);

	transform_.UpdateMatrix();

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}
