#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// collider
#include <Collider.h>

// engine
#include <WorldTransform.h>
#include <Model.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Goal class
////////////////////////////////////////////////////////////////////////////////////////////
class Goal
	: public Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Goal() = default;
	~Goal() = default;

	void Init(const Vector3f& position, const Vector3f& goalGravity);

	void Update();

	void Draw(const ViewProjection& viewProj);

	void UpdateImGui();

	const Vector3f& GetColliderPosition() const override { return transform_.translation_; }

	void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) override;

	/*void OnCollisionExit(MAYBE_UNUSED Collider* const other) override {}*/

	void SetGravity(const Vector3f& goalGravity);

	void SetPosition(const Vector3f& position);

	void ResetFlag();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WorldTransform transform_;

	Vector3f goalGravity_ = {};

	// model
	std::unique_ptr<Model> model_;

	bool isGoal_ = false;
	ObjectColor color_;

	WorldTransform effectTransform_;
	std::unique_ptr<Model> effectModel_;
	ObjectColor effectColor_;

	float goalAftreTime_ = 0.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CalculateRotate();

};