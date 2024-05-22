#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <WorldTransform.h>
#include <ViewProjection.h>

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

	//! @brief 初期化処理
	void Init(const Vector3f& pos, const Vector3f& rotate);

	//! @brief 終了処理
	void Term();

	//! @brief 更新処理
	void Update();

	void SetOnImGui();

	const ViewProjection& GetViewProjection() { return viewProj_; }

	const WorldTransform& GetWorldTranform() { return worldTransform_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WorldTransform worldTransform_;
	ViewProjection viewProj_;

	Vector3f velocity_ = {0.0f, 0.0f, -0.06f};
	Vector3f addRotate_ = {0.0f, 0.0f, 0.0f};

};