#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Collider.h>

// engine
#include <WorldTransform.h>
#include <Model.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Box class
////////////////////////////////////////////////////////////////////////////////////////////
class Box
	: public Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector3f& scale, const Vector3f& position);

	void Draw(const ViewProjection& viewProj);

	const Vector3f& GetColliderPosition() const override { return transform_.translation_; }

	const Vector3f& GetScale() const { return transform_.scale_; }

	void SetImGuiCommand();

	bool IsDelete() const { return isDelete_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Model> model_;
	WorldTransform transform_;

	bool isDelete_ = false;
	
};