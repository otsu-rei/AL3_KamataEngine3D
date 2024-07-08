#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <list>

// engine
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

// mylib
#include <MyMath.h>

// character
#include "BaseCharacter.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy
	: public BaseCharacter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Enemy() = default;

	~Enemy() { Term(); }

	void Init(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewProj) override;

	void Term() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ModelType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ModelType {
		MODEL_BODY,

		kCountOfModelType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	/* parameter */
	const float kRotateRate_ = pi_v / 100.0f;
	const float kRotateRadius_ = 10.0f;
	const Vector3f kRotateCenterPos_ = {10.0f, 0.0f, 0.0f};

	/* data */
	float theta_ = 0.0f;

	WorldTransform modelTransforms_[kCountOfModelType];

};