#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <list>
#include <unordered_set>

// engine
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameScene;

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
	void Init(Model* model, const Vector3f& pos);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	//! @brief 終了処理
	void Term();

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	//! @brief ImGuiに設定
	void SetOnImGui();

	/* setter */

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetParent(const WorldTransform* parent) {
		worldTransform_.parent_ = parent;
	}

	void SetViewProj(const ViewProjection* viewProj) {
		viewProj_ = viewProj;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	/* externals */
	Input* input_ = Input::GetInstance();
	Model* model_ = nullptr;
	GameScene* gameScene_ = nullptr;
	const ViewProjection* viewProj_ = nullptr;


	/* parameters */
	const float kMoveSpeed_ = 0.2f;
	const Vector3f kMoveLimit_ = {100.0f, 100.0f, 100.0f};
	const float kDeadZone_ = 0.7f;
	const float kRotateRate_ = 0.12f;

	/* data */
	WorldTransform worldTransform_;
	float targetAngle_ = 0.0f;
	

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

	void MoveController();

};