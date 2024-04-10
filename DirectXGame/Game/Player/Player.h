#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>

// engine
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player {
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

	//! @brief 終了処理
	void Term();

	//! @brief ImGuiに設定
	void SetOnImGui();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Input* input_ = Input::GetInstance();

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	// parameters
	float speed_;

	const Vector3f kMoveLimit = {10.0f, 10.0f, 10.0f};

	WorldTransform worldTransform_;

};