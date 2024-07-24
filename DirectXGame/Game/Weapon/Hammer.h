#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// collider base
#include "Collider.h"

// engine
#include "Model.h"
#include "ViewProjection.h"
#include "Effect.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Hammer class
////////////////////////////////////////////////////////////////////////////////////////////
class Hammer final
	: public Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Hammer() = default;

	~Hammer() { Term(); }

	void Init(Model* model, WorldTransform* parent);

	void Term();

	void Update();

	void Draw(const ViewProjection& viewProj);

	void OnCollision([[maybe_unused]]Collider* other) override;

	const Vector3f& GetRotate() const { return localTransform_.rotation_; }

	void SetRotation(const Vector3f& rotation) { localTransform_.rotation_ = rotation; }

	void SetEffect(EffectManager* effectManager, Model* hitEffectModel);

	Vector3f GetCenterPosition() const override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Model* model_ = nullptr;
	Model* effectModel_ = nullptr;
	EffectManager* effectManager_ = nullptr;

	//* member *//

	WorldTransform localTransform_;


};