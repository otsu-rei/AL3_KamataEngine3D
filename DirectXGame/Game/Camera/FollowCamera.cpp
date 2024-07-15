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
			viewProj_.rotation_.y += static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX * kRotSpeed_;
		}
	}

	// rotateの値からcameraのtranslateの値を算出
	Matrix4x4 rotateMatrix = Matrix::MakeRotate(viewProj_.rotation_.y, kRotateBaseY);
	Vector3f offset = Matrix::TransformNormal(kOffset_, rotateMatrix);

	// translateの処理
	if (target_) { //!< 追従対象がいるかどうか
		// 座標をコピーしてオフセット分ずらす
		viewProj_.translation_ = target_->translation_ + offset;
	}

	viewProj_.UpdateMatrix();
}
