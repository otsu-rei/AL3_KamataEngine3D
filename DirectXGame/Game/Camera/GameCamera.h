#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include "ViewProjection.h"

// geometry
#include <Quaternion.h>

// math
#include <MyMath.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Player;

////////////////////////////////////////////////////////////////////////////////////////////
// GameCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class GameCamera {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameCamera() = default;
	~GameCamera() { Term(); }

	void Init();
	
	void Term();

	void Update();

	//* setter *//

	void SetPlayer(const Player* player) { player_ = player; }

	//* getter *//

	const ViewProjection& GetViewProjection() const { return viewProj_; }

	const Quaternion& GetQuaternion() const { return quaternion_; }

	const Quaternion GetTargetQuaternion() const;

	const Matrix4x4& GetBillboradMatrix() const { return billboradMat_; }

	//* imgui *//

	void UpdateImGui();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Euler3 structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Euler3 {
		float yaw, pitch, roll;
	};


	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const Player* player_ = nullptr;

	//* parameter *//

	const float distance_ = -50.0f;
	const float kRotateRate_ = 0.04f;
	const float kRotateSpeed_ = 0.02f;

	//* member *//

	ViewProjection viewProj_;

	Matrix4x4 billboradMat_;

	Vector3f pivot_ = {};
	Quaternion quaternion_ = {};
	
	Euler3 targetEuler_  = {};
	Euler3 currentEuler_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	static Euler3 CalculateEuler(const Vector3f& gravityDireciton);

	void CalculateView();

	void UpdateYaw();

};