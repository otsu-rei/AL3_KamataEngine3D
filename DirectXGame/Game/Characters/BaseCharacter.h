#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <vector>

// engine
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseCharacter Base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseCharacter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual void Init(const std::vector<Model*>& models) = 0;

	virtual void Update() = 0;

	virtual void Draw(const ViewProjection& viewProj) = 0;

	virtual void Term() = 0;

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::vector<Model*> models_;
	WorldTransform      worldTransform_;

};