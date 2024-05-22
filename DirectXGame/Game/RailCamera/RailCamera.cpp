#include "RailCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MyMath.h"
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RailCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RailCamera::Init(const Vector3f& pos, const Vector3f& rotate) {
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = rotate;
	worldTransform_.matWorld_ = Matrix::MakeAffine({1.0f, 1.0f, 1.0f}, rotate, pos);

	viewProj_.Initialize();
}

void RailCamera::Term() {
}

void RailCamera::Update() {

	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_ += addRotate_;
	/*worldTransform_.rotation_.y += 0.001f;*/

	worldTransform_.matWorld_ = Matrix::MakeAffine(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	viewProj_.matView = Matrix::Inverse(worldTransform_.matWorld_);
	//
}

void RailCamera::SetOnImGui() {
	if (ImGui::TreeNode("railCamera")) {

		ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate",    &worldTransform_.rotation_.x,    0.01f);

		ImGui::Spacing();

		ImGui::DragFloat3("velocity",  &velocity_.x,  0.002f);
		ImGui::DragFloat3("addRotate", &addRotate_.x, 0.002f);

		ImGui::TreePop();
	}
}
