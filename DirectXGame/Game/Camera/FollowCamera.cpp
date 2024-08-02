#include "FollowCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>
#include <LockOn.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FollowCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	viewProj_.Initialize();
}

void FollowCamera::Update() {
	//!< 関数分けをしておく
	
	if (lockOn_->GetTargetPosition().has_value()) { //!< ロックオンする対象がいる場合
		
		// ロックオンしてるキャラの座標の取得
		Vector3f lockOnPos = lockOn_->GetTargetPosition().value();

		// 追従対象からロックオン対象へのベクトル
		Vector3f sub = lockOnPos - target_->translation_;

		viewProj_.rotation_.y = std::atan2(sub.x, sub.z);

	} else {

		XINPUT_STATE joyState;

		// コントローラーでの移動
		if (input_->GetJoystickState(0, joyState)) { //!< コントローラーが接続されてる場合
			/*viewProj_.rotation_.y += static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX * kRotSpeed_;*/
			destinationAngleY_ += static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX * kRotSpeed_;

			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) { //!< 右スティック押し込みしたとき
				// 追従対象の後ろにカメラを配置
				destinationAngleY_ = target_->rotation_.y;
				/*viewProj_.rotation_.y = target_->rotation_.y;*/
			}
		}
	}

	// translateの処理
	if (target_) { //!< 追従対象がいるかどうか

		// 追従座標の補間
		interTarget_ = Vector::Lerp(interTarget_, target_->translation_, kTracingRate_);

		Vector3f offset = CalculateOffset();

		// 座標をコピーしてオフセット分ずらす
		viewProj_.translation_ = interTarget_ + offset;
	}

	// 最短角度補間
	viewProj_.rotation_.y = LerpShortAngle(viewProj_.rotation_.y, destinationAngleY_, kRotRate_);

	viewProj_.UpdateMatrix();
}

void FollowCamera::Reset() {
	if (target_) { //!< 追従対象がいる場合
		// 追従対象, 角度の初期化
		interTarget_ = target_->translation_;
		viewProj_.rotation_.y = target_->rotation_.y;
	}
	// destinationAngleY_ = viewProjection_.rotation_.y;

	// 追従対象からのオフセット
	Vector3f offset = CalculateOffset();
	viewProj_.translation_ = interTarget_ + offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

Vector3f FollowCamera::CalculateOffset() const { 
	// rotateの値からcameraのtranslateの値を算出
	Matrix4x4 rotateMatrix = Matrix::MakeRotate(viewProj_.rotation_.y, kRotateBaseY);
	Vector3f offset = Matrix::TransformNormal(kOffset_, rotateMatrix);

	return offset;
}
