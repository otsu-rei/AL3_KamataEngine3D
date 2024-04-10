#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Vector3.h"
#include "Model.h"
#include "WorldTransform.h"


////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBullet {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	void Init(Model* model, const Vector3f& position, const Vector3f& velocity);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	bool IsDead() const { return isDead_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	WorldTransform worldTransform_;

	// parameter //
	static const int32_t kLifeTime_ = 60 /*_frame*/ * 5 /*_s*/;

	// info //
	Vector3f velocity_;
	int32_t deathTimer_ = kLifeTime_;
	bool isDead_ = false;

	

};