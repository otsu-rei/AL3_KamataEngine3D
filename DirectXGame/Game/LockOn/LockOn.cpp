#include "LockOn.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include "TextureManager.h"
#include "WinApp.h"

////////////////////////////////////////////////////////////////////////////////////////////
// LockOn class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LockOn::Init() {

	lockOnMark_.reset(
		Sprite::Create(
			TextureManager::Load("reticle.png"),                         //!< textureHandle
			{WinApp::kWindowWidth / 2.0f, WinApp::kWindowHeight / 2.0f}, //!< position
			{1.0f, 0.0f, 0.0f, 1.0f},                                    //!< color
			{0.5f, 0.5f}                                                 //!< anchorpoint
		)
	);

	// sizeの調整
	lockOnMark_->SetSize({100.0f, 100.0f});

	globalVariables->CreateGroup(kGroupName_);
	globalVariables->AddItem(kGroupName_, "min distance", minDistance_);
	globalVariables->AddItem(kGroupName_, "max distance", maxDistance_);
	globalVariables->AddItem(kGroupName_, "digree angle", digreeAngle_);

}

void LockOn::Term() {}

void LockOn::Update(
	const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProj) {

	XINPUT_STATE joyState;
	
	// コントローラーでのlockOn処理
	if (!input_->GetJoystickState(0, joyState)) { //!< コントローラーが接続されてない場合
		return;
	}

	bool isPushTargetButton = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X); //!< xが押されてるかどうか
	
	if (target_) { //!< ロックオンされてるとき
		if (isPushTargetButton || !IsTargetInRange(viewProj)) { //!< ボタンが押された || targetが範囲外場合
			target_ = nullptr;
			// ロックオンされてるときにxを再度押すとロックオンを解除する
		}

	} else {
		if (isPushTargetButton) {
			SearchTargetEnemy(enemies, viewProj);
		}
	}
	
	
	if (target_) { //!< ロックオンされてる(継続)場合
		// 敵のロックオン座標の取得
		Vector3f worldPos = target_->GetWorldCenterPosition();

		// world -> screen 座標変換
		Matrix4x4 vpvMatrix
			= viewProj.matView * viewProj.matProjection * Matrix::MakeViewport(0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), 0.0f, 1.0f);

		Vector3f screenPos = Matrix::Transform(worldPos, vpvMatrix); //!< zは使わないこと

		lockOnMark_->SetPosition({screenPos.x, screenPos.y});
	}
}

void LockOn::Draw() {

	if (target_ == nullptr) { //!< 敵を目標にしてないので描画しない
		return;
	}

	lockOnMark_->Draw();

}

void LockOn::SearchTargetEnemy(
	const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProj) {

	// 目標
	std::list<std::pair<float, const Enemy*>> targets;

	// すべての敵に対してロックオン判定
	for (auto& enemy : enemies) {

		// 中心座標の取得
		Vector3f worldPos = enemy->GetWorldCenterPosition();

		// world -> view 座標
		Vector3f viewPos = Matrix::Transform(worldPos, viewProj.matView);

		// ロックオンにできる距離かどうかの確認
		if (viewPos.z >= minDistance_ && viewPos.z <= maxDistance_) { //!< ロックオンできる範囲内にいるとき

			// カメラ前方との角度の計算
			float arcTangent = std::atan2(
				std::sqrt(viewPos.x * viewPos.x + viewPos.y * viewPos.y),
				viewPos.z
			);
			
			if (std::abs(arcTangent) <= angleRange_) { //!< ロックオンできる角度かどうか
				targets.emplace_back(std::make_pair(viewPos.z, enemy.get()));
			}
		}
	}

	// ロックオン対象のリセット
	target_ = nullptr;

	if (!targets.empty()) { //!< ロックオンできる敵がいるかどうか
		// 距離で昇順にソート
		targets.sort([](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });

		// ソート結果で一番近い敵をロックオン対象とする
		target_ = targets.front().second;
	}
}

bool LockOn::IsTargetInRange(const ViewProjection& viewProj) { 

	if (target_ == nullptr) { //!< ロックオン対象が設定されてない
		return false;
	}

	// 中心座標の取得
	Vector3f worldPos = target_->GetWorldCenterPosition();

	// world -> view 座標
	Vector3f viewPos = Matrix::Transform(worldPos, viewProj.matView);

	// ロックオンにできる距離かどうかの確認
	if (viewPos.z >= minDistance_ && viewPos.z <= maxDistance_) { //!< ロックオンできる範囲内にいるとき

		// カメラ前方との角度の計算
		float arcTangent = std::atan2(std::sqrt(viewPos.x * viewPos.x + viewPos.y * viewPos.y), viewPos.z);

		if (std::abs(arcTangent) <= angleRange_) { //!< ロックオンできる角度かどうか
			return true;
		}
	}

	return false;
}

void LockOn::ApplyGlobalVariables() {

	minDistance_ = globalVariables->GetValue<float>(kGroupName_, "min distance");
	maxDistance_ = globalVariables->GetValue<float>(kGroupName_, "max distance");
	digreeAngle_ = globalVariables->GetValue<float>(kGroupName_, "digree angle");

	angleRange_ = digreeAngle_ * kDigreeToRadian;

}
