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
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

// mylib
#include <MyMath.h>

// character
#include "BaseCharacter.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameScene;

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public BaseCharacter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~Player() { Term(); }

	//! @brief 初期化処理
	void Init(const std::vector<Model*>& models) override;

	//! @brief 更新処理
	void Update() override;

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj) override;

	//! @brief 終了処理
	void Term() override;

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// ModelType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ModelType {
		MODEL_HEAD,
		MODEL_BODY,
		MODEL_LARM,
		MODEL_RARM,

		kCountOfModelType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	/* externals */
	Input* input_ = Input::GetInstance();
	GameScene* gameScene_ = nullptr;
	const ViewProjection* viewProj_ = nullptr;

	/* parameters */
	const float kMoveSpeed_ = 0.2f;
	const Vector3f kMoveLimit_ = {100.0f, 100.0f, 100.0f};
	const float kDeadZone_ = 0.7f;
	const float kRotateRate_ = 0.12f;
	const uint16_t period_ = 60; //!< 今回は1秒(60frame)周期
	const float floatingStep_ = 2.0f * pi_v / period_;
	const float floatingRange_ = 0.2f;

	/* data */
	float targetAngle_ = 0.0f;
	float floatingParameter_ = 0.0f;

	WorldTransform modelTransforms_[kCountOfModelType];

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

	void MoveController();

	void InitFloatingGimmick();

	void UpdateFloatingGimmick();

};