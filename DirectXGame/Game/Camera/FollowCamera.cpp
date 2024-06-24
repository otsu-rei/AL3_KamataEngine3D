#include "FollowCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FollowCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	viewProj_.Initialize();
}

void FollowCamera::Update() {
	//!< 関数分けをしておく
	
	Rotate();

	viewProj_.UpdateMatrix();
}

void FollowCamera::Rotate() {
	// rotate処理
	XINPUT_STATE joyState;

	// コントローラーでの移動
	if (!input_->GetJoystickState(0, joyState)) { //!< コントローラーが接続されてない場合
		return;
	}

	viewProj_.rotation_.y += static_cast<float>(joyState.Gamepad.sThumbRX) / SHRT_MAX * kRotSpeed_;

	Matrix4x4 rotateMatrix = Matrix::MakeRotate(viewProj_.rotation_.y, kRotateBaseY);
	Vector3f offset = Matrix::TransformNormal(kOffset_, rotateMatrix);

	// translateの処理
	if (target_) { //!< 追従対象がいるかどうか
		// 座標をコピーしてオフセット分ずらす
		viewProj_.translation_ = target_->translation_ + offset;
	}
}
