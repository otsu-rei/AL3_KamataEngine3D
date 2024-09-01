#include "GameCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Player.h"

#include <imgui.h>
#include <input.h>
#include <MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameCamera class
////////////////////////////////////////////////////////////////////////////////////////////

void GameCamera::Init() {
	viewProj_.Initialize();
}

void GameCamera::Term() {}

void GameCamera::Update() {

	if (player_ == nullptr) {
		return;
	}

	UpdateYaw();

	const Euler3& directionEuler = CalculateEuler(player_->GetGravityDirection());
	targetEuler_.pitch = directionEuler.pitch;
	targetEuler_.roll  = directionEuler.roll;
	// yawはplayerが変更するので

	currentEuler_ = {
	    LerpShortAngle(currentEuler_.yaw, targetEuler_.yaw, kRotateRate_),
	    LerpShortAngle(currentEuler_.pitch, targetEuler_.pitch, kRotateRate_),
	    LerpShortAngle(currentEuler_.roll, targetEuler_.roll, kRotateRate_),
	};

	CalculateView();
}

const Quaternion GameCamera::GetTargetQuaternion() const { 

	Quaternion qYaw = MakeRotateAxisAngleQuaternion({0.0f, 1.0f, 0.0f}, targetEuler_.yaw);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({1.0f, 0.0f, 0.0f}, targetEuler_.pitch);
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({0.0f, 0.0f, 1.0f}, targetEuler_.roll);

	return qRoll * qPitch * qYaw;
}

void GameCamera::UpdateImGui() {
	if (ImGui::TreeNode("GameCamera")) {
		/*ImGui::DragFloat("lon", &test_.x, 0.01f);
		ImGui::DragFloat("TestZ", &test_.z, 0.01f);
		ImGui::DragFloat("TestY", &test_.y, 0.01f);*/

		ImGui::DragFloat("yaw", &targetEuler_.yaw, 0.01f); //!< playerが操作できるのはyawのみ
		/*ImGui::DragFloat("pitch", &pitch_, 0.01f);
		ImGui::DragFloat("roll", &roll_, 0.01f);*/

		ImGui::TreePop();
	}
}

GameCamera::Euler3 GameCamera::CalculateEuler(const Vector3f& gravityDireciton) {

	Euler3 result = {};

	if (gravityDireciton.x != 0.0f) {
		result.roll = (pi_v / 2.0f) * gravityDireciton.x;

	} else if (gravityDireciton.y != 0.0f) {
		if (gravityDireciton.y == 1.0f) {
			result.roll = pi_v;
		}

		//!< gravityDireciton.y == -1.0f の時は何もしない
	
	} else if (gravityDireciton.z != 0.0f) {
		result.pitch = (pi_v / 2.0f) * -gravityDireciton.z;
	}

	return result;
}

void GameCamera::CalculateView() {

	Quaternion qYaw   = MakeRotateAxisAngleQuaternion({0.0f, 1.0f, 0.0f}, currentEuler_.yaw);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({1.0f, 0.0f, 0.0f}, currentEuler_.pitch);
	Quaternion qRoll  = MakeRotateAxisAngleQuaternion({0.0f, 0.0f, 1.0f}, currentEuler_.roll);

	quaternion_ = qRoll * qPitch * qYaw;

	Vector3f offset = {0.0f, 4.0f, distance_};
	Vector3f rotationOffset = RotateVector(offset, quaternion_);

	Vector3f finalPosition = pivot_ + rotationOffset;

	Quaternion qTest = MakeRotateAxisAngleQuaternion({1.0f, 0.0f, 0.0f}, 0.08f); //!< 少しカメラを傾ける

	Matrix4x4 worldMat = Matrix::MakeAffine({1.0f, 1.0f, 1.0f}, quaternion_ * qTest, finalPosition);
	viewProj_.matView = worldMat.Inverse();

	billboradMat_ = Matrix::MakeRotate(quaternion_.Inverse()) * worldMat;
	billboradMat_.m[3][0] = 0.0f;
	billboradMat_.m[3][1] = 0.0f;
	billboradMat_.m[3][2] = 0.0f;
}

void GameCamera::UpdateYaw() {

	XINPUT_STATE state = {};

	if (!Input::GetInstance()->GetJoystickState(0, state)) {
		return;
	}

	float move = static_cast<float>(state.Gamepad.sThumbRX) / SHRT_MAX;

	targetEuler_.yaw += move * kRotateSpeed_;
	targetEuler_.yaw = std::fmod(targetEuler_.yaw, pi_v * 2.0f);

}
