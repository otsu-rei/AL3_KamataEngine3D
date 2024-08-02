#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Input.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class LockOn;

////////////////////////////////////////////////////////////////////////////////////////////
// FollowCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class FollowCamera {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Update();

	void Reset();

	void SetTarget(const WorldTransform* target);

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	const ViewProjection& GetViewProjection() const { return viewProj_; }

	Vector3f CalculateOffset() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//
	Input* input_ = Input::GetInstance();
	const LockOn* lockOn_ = nullptr;
	
	const WorldTransform* target_ = nullptr; //!< 追従対象

	//* menber *//

	ViewProjection viewProj_;

	Vector3f interTarget_ = {}; //!< 追従対象の残像座標
	float destinationAngleY_ = 0.0f;


	/* parameter */

	const Vector3f kOffset_ = {0.0f, 8.0f, -20.0f};
	const float kRotSpeed_ = 0.04f;
	const float kRotRate_ = 0.1f;
	const float kTracingRate_ = 0.08f;

	//=========================================================================================
	// private methods
	//=========================================================================================

};