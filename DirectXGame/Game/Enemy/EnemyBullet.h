#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Model.h"
#include "WorldTransform.h"

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

	bool IsDead() const { return isDead_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_           = nullptr;
	uint32_t textureHandle_ = 0;

	// parameter //
	const int32_t kLifeTime_ = 60 /*_frame*/ * 2 /*_s*/;

	// info //
	bool isDead_ = false;
	int32_t deathTimer_ = kLifeTime_;

	Vector3f velocity_;
	WorldTransform worldTransform_;

};