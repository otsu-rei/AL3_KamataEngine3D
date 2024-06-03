#include "RailCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MyMath.h"
#include <imgui.h>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// RailCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

//void RailCamera::Init(const Vector3f& pos, const Vector3f& rotate) {
//	worldTransform_.translation_ = pos;
//	worldTransform_.rotation_ = rotate;
//	worldTransform_.matWorld_ = Matrix::MakeAffine({1.0f, 1.0f, 1.0f}, rotate, pos);
//
//	viewProj_.Initialize();
//}

void RailCamera::Init() {
	/*controllPoints_ = {
		{0,  0,  20},
		{10, 10, 5},
		{10, 15, 10},
		{20, 15, 0},
		{20, 0,  10},
		{30, 0,  10},
	};*/

	controllPoints_ = {
	    {0, 0, -20},
	    {0, 2, -10},
	    {0, 4, 0},
	    {0, 8, 10},
	};

	viewProj_.Initialize();
}

void RailCamera::Term() { controllPoints_.clear(); }

void RailCamera::Update() {

	if (t_ >= 1.0f - eyeToTargetDistance_) { //!< t + eyeToTargetDistanceが1.0fである必要があるため
		return;
	}

	t_ += 0.001f;
	t_ = std::clamp(t_, 0.0f, 1.0f - eyeToTargetDistance_);

	CalculateMatrix();
	
}

void RailCamera::SetOnImGui() {
	if (ImGui::TreeNode("railCamera")) {

		ImGui::DragFloat("t", &t_, 0.01f, 0.0f, 1.0f);
		ImGui::Text("[position] x: %.3f, y: %.3f, z: %.3f", viewProj_.translation_.x, viewProj_.translation_.y, viewProj_.translation_.z);
		ImGui::Text("[rotate]   x: %.3f, y: %.3f, z: %.3f", viewProj_.rotation_.x, viewProj_.rotation_.y, viewProj_.rotation_.z);

		ImGui::Spacing();

		ImGui::Text("[eye]    x: %.3f, y: %.3f, z: %.3f", eye_.x, eye_.y, eye_.z);
		ImGui::Text("[target] x: %.3f, y: %.3f, z: %.3f", target_.x, target_.y, target_.z);

		ImGui::TreePop();
	}
}

void RailCamera::CalculateMatrix() {
	eye_ = CatmullRomPosition(controllPoints_, t_);
	target_ = CatmullRomPosition(controllPoints_, t_ + eyeToTargetDistance_);

	Vector3f forward = target_ - eye_;

	viewProj_.translation_ = eye_;

	viewProj_.rotation_.y = std::atan2(forward.x, forward.z);

	float length = Vector::Length({forward.x, 0.0f, forward.z});
	viewProj_.rotation_.x = std::atan2(-forward.y, length);

	worldTransform_.matWorld_ = Matrix::MakeAffine({1.0f, 1.0f, 1.0f}, viewProj_.rotation_, viewProj_.translation_);
	viewProj_.UpdateViewMatrix();
}
