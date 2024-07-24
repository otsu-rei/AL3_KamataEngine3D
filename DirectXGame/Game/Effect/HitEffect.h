#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include "Effect.h"

// engine
#include "Model.h"
#include "WorldTransform.h"

////////////////////////////////////////////////////////////////////////////////////////////
// HitEffect class
////////////////////////////////////////////////////////////////////////////////////////////
class HitEffect
	: public Effect {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	HitEffect() = default;

	void Init(Model* model, const Vector3f& pos);

	void Term();

	void Update() override;

	void Draw(const ViewProjection& viewProj) override;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Model* model_ = nullptr;

	//* parameter *//
	const float kDeltaT_ = 1.0f / 60.0f/*frame*/;

	//* member *//

	float t_ = 0.0f;
	float radius_; //!< scale
	Vector4f color_ = {1.0f, 0.1f, 0.1f, 1.0f};

	WorldTransform worldTransform_;
	ObjectColor    objectColor_;

};