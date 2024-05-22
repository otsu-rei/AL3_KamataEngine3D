#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Model.h"
#include "WorldTransform.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Player;

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyBullet class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyBullet {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	EnemyBullet() {}

	//! @brief デストラクタ
	~EnemyBullet() { }

	//! @brief 初期化処理
	void Init(Model* model, const Vector3f& position, const Vector3f& velocity);

	void Update();

	void Draw(const ViewProjection& viewProj);

	//! @brief 衝突したら呼び出される関数
	void OnCollision();

	bool IsDead() const { return isDead_; }

	Vector3f GetWorldPosition() const;

	const float GetCollisionRadius() const { return kCollisionRadius_; }

	void SetPlayer(Player* player) { player_ = player; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_           = nullptr;
	uint32_t textureHandle_ = 0;

	Player* player_ = nullptr;

	// parameter //
	const int32_t kLifeTime_ = 60 /*_frame*/ * 10 /*_s*/;
	const float kCollisionRadius_ = 1.0f;

	const float kBulletSpeed_ = 0.6f;
	const float kHomingRaito_ = 0.02f; // 1 ~ 0

	// info //
	bool isDead_ = false;
	int32_t deathTimer_ = kLifeTime_;

	Vector3f velocity_;
	WorldTransform worldTransform_;

};