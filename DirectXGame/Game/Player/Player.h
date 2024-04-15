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
#include "WorldTransform.h"
#include "ViewProjection.h"

// game
#include "PlayerBullet.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~Player() { Term(); }

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

	Vector3f GetWorldPosition() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Input* input_ = Input::GetInstance();

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	// parameters
	const float kMoveSpeed_ = 0.2f;
	const float kRotSpeed_ = 0.02f; //!< radian / frame

	const Vector3f kMoveLimit = {10.0f, 10.0f, 10.0f};

	WorldTransform worldTransform_;

	/* bullet */
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	// parameters
	const float kBulletSpeed_ = 1.0f;

	

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

	void Rotate();

	void Attack();

};