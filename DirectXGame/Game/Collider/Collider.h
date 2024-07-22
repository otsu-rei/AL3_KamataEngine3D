#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// math
#include <Vector3.h>

// engine
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~Collider() = default;

	void Init();

	void Draw(Model* model, const ViewProjection& viewProj);

	void UpdateWorldTransform();

	//! @brief 当たった時の処理
	virtual void OnCollision() {}

	//! @brief 中心座標の取得
	virtual Vector3f GetCenterPosition() const = 0;

	//! @brief 当たり判定の判定円を取得
	float GetRadius() const { return collisionRadius_; }

	//! @brief 当たり判定の判定円の設定
	void SetRadius(float radius) { collisionRadius_ = radius; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	float collisionRadius_ = 2.0f; //!< 判定円
	// todo: 円の範囲をworldTransformのscaleにする

	WorldTransform worldTransform_;

};