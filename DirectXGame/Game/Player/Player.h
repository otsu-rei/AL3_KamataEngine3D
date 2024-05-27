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

// game
#include "PlayerBullet.h"
#include "Collider.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameScene;

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public Collider {
public:
	//=========================================================================================
	// public methods
	//=========================================================================================

	~Player() { Term(); }

	//! @brief 初期化処理
	void Init(Model* model, uint32_t textureHandle, const Vector3f& pos);

	//! @brief 更新処理
	void Update(const ViewProjection& viewProj);

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	//! @brief UI描画処理
	void DrawUI();

	//! @brief 終了処理
	void Term();

	//! @brief ImGuiに設定
	void SetOnImGui();

	void SetOnDebugImGui();

	//! @brief 衝突したら呼び出される関数
	void OnCollision() override;

	Vector3f GetWorldPosition() const override;

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetParent(const WorldTransform* parent) {
		worldTransform_.parent_ = parent;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Input* input_ = Input::GetInstance();

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	GameScene* gameScene_ = nullptr;

	// parameters
	const float kMoveSpeed_ = 0.2f;
	const float kRotSpeed_ = 0.02f; //!< radian / frame
	const float kCollisionRadius_ = 1.0f;
	const float kBulletSpeed_ = 1.0f;

	const Vector3f kMoveLimit = {20.0f, 10.0f, 30.0f};

	// data //
	WorldTransform worldTransform_;
	WorldTransform worldTransform3DReticle_;

	float distanceReticleObject_ = 50.0f;

	Vector3f nearPos, farPos;

	std::unique_ptr<Sprite> sprite2DReticle_;
	

	

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

	void MoveController();

	void Rotate();

	void Attack();

	void AttackController();

	void UpdateReticle();

	void UpdateReticleMouse(const ViewProjection& viewProj);

	void UpdateReticleController(const ViewProjection& viewProj);

};