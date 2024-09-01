#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <memory>
#include <array>

// engine
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

// collider
#include <Collider.h>

// Game
#include "FieldFace.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FieldFaceType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum FieldFaceType : uint32_t { //!< view +z
	kLeftFace,
	kRightFace,
	kTopFace,
	kBottomFace,
	kFrontFace,
	kBackFace,

	kCountOfFieldFaceType 
};

////////////////////////////////////////////////////////////////////////////////////////////
// Field class
////////////////////////////////////////////////////////////////////////////////////////////
class Field {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Field() = default;
	~Field() = default;

	void Init();

	void Draw(const ViewProjection& viewProj);

	void DrawShadow(const ViewProjection& viewProj);


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Model> model_;
	WorldTransform transform_;
	
	std::array<std::unique_ptr<FieldFace>, kCountOfFieldFaceType> faces_;

	//* 影用
	WorldTransform shadowTransform_;
	ViewProjection viewProj_;

};