#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include "WorldTransform.h"
#include "Model.h"
#include "Input.h"

// collider
#include <Collider.h>
#include <MathCollider.h>

// c++

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameCamera;

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw(const ViewProjection& viewProj);

	void DrawLine();

	const Vector3f& GetColliderPosition() const override { return transform_.translation_; }

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

	//* getter *//

	const Vector3f& GetGravityDirection() const { return gravityDireciton_; }

	//* setter *//

	void SetGameCamera(const GameCamera* camera) { gameCamera_ = camera; }

	void SetPosition(const Vector3f& position) { transform_.translation_ = position; }

	void SetGrabity(const Vector3f& gravity);

	//* imgui *//

	void DebugImGui() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	Input* input_ = Input::GetInstance();
	const GameCamera* gameCamera_ = nullptr;

	//* parameter *//

	const float kGravity = 12.0f;
	const float deltaTime = 1.0f / 60.0f; // todo: 動的frameへの対応
	const float jumpPower = 10.0f;
	const float moveSpeed = 6.0f;
	const float stickLength = 2.0f;

	//* member *//

	WorldTransform transform_;

	Vector3f velocity_     = {};
	Vector3f acceleration_ = {};
	Vector3f direction_    = {};

	std::unique_ptr<Model> model_; //!< IA

	Vector3f gravityDireciton_ = {0.0f, -1.0f, 0.0f};

	bool isTouchingGround_ = false;

	//* UI *//

	bool     canStick_ = false;
	
	std::unique_ptr<Model> plane_;
	WorldTransform         planeTransform_;
	Vector3f               planeOffset_ = {0.0f, 2.0f, 0.0f};

	WorldTransform demoTransform_;
	ObjectColor    demoColor_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Action();
	void Move(const XINPUT_STATE& state);
	void Jump(const XINPUT_STATE& state);
	void Stick(const XINPUT_STATE& state, const XINPUT_STATE& preState);

	void PushBackTrnaslate(const Vector3f& otherPosition, const CollisionBoundings::AABB& otherAABB);

	void DebugCommand();

	static bool CheckStickSegmentToAABB(
		const MathCollider::Segment& segment,
		const MathCollider::AABB& aabb,
		Vector3f& outHitPosition, Vector3f& outHitNormal
	);

};