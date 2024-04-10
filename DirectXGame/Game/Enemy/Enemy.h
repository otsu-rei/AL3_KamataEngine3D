#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>

// engine
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	void Init(Model* model, uint32_t textureHandle);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	//! @brief ImGuiに設定
	void SetOnImGui();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	// parameters //
	const float kMoveSpeed_ = 0.2f;

	WorldTransform worldTransform_;

};