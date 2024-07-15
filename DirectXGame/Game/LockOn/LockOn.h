#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include "ViewProjection.h"
#include "Sprite.h"
#include "Input.h"

// c++
#include <memory>
#include <list>
#include <optional>

// Game
#include "Enemy.h"

// GlobalVariable
#include "GlobalVariables.h"

////////////////////////////////////////////////////////////////////////////////////////////
// LockOn class
////////////////////////////////////////////////////////////////////////////////////////////
class LockOn {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LockOn() = default;

	~LockOn() { Term(); }

	void Init();

	void Term();

	void Update(
		const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProj
	);

	void Draw();

	std::optional<Vector3f> GetTargetPosition() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const Enemy* target_ = nullptr;
	Input* input_ = Input::GetInstance();

	//* parameter *//
	const float kDigreeToRadian = (pi_v * 2.0f) / 360.0f; //!< 弧度法への変換用

	// todo: globalVarialeクラスに持たせる
	float minDistance_ = 10.0f;
	float maxDistance_ = 50.0f;
	float digreeAngle_ = 180.0f;

	float angleRange_ = digreeAngle_ * kDigreeToRadian;

	const std::string kGroupName_ = "LockOn";

	//* data *//

	std::unique_ptr<Sprite> lockOnMark_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SearchTargetEnemy(
		const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProj
	);

	//! @brief ロックオン対象が範囲内かどうか
	//! @retval ture  範囲内
	//! @retval false 範囲外
	bool IsTargetInRange(const ViewProjection& viewProj);

	void ApplyGlobalVariables();

};