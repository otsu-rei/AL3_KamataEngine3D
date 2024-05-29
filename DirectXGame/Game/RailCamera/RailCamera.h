#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <WorldTransform.h>
#include <ViewProjection.h>

#include <vector>
#include <Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RailCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class RailCamera {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief デストラクタ
	~RailCamera() { Term(); }

	void Init();

	//! @brief 終了処理
	void Term();

	//! @brief 更新処理
	void Update();

	void SetOnImGui();

	const ViewProjection& GetViewProjection() const { return viewProj_; }

	const WorldTransform& GetWorldTranform() const { return worldTransform_; }

	const std::vector<Vector3f>& GetControllPoints() const { return controllPoints_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WorldTransform worldTransform_;
	ViewProjection viewProj_;

	// railCamera
	std::vector<Vector3f> controllPoints_;

	float t_ = 0.0f;
	const float eyeToTargetDistance_ = 0.02f;

	// debug

	Vector3f eye_;
	Vector3f target_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CalculateMatrix();

};