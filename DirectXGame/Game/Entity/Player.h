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

// c++

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

	const Vector3f& GetColliderPosition() const override { return transform_.translation_; }

	void OnCollisionEnter(MAYBE_UNUSED Collider* const other) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	Input* input_ = Input::GetInstance();

	Vector3f beforeTranslation_ = {0.0f};
	WorldTransform transform_;

	std::unique_ptr<Model> model_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Action();

};