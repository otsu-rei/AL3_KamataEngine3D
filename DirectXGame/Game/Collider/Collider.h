#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum CollisionType {
	PLAYER        = 1 << 0,
	PLAYER_BULLET = 1 << 1,
	ENEMY         = 1 << 2,
	ENEMY_BULLET  = 1 << 3,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Collider class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 衝突したら呼び出される関数
	virtual void OnCollision() = 0;

	//! @brief ワールド座標を取得
	virtual Vector3f GetWorldPosition() const = 0;

	float GetRadius() const { return radius_; }

	void SetRadius(float radius) { radius_ = radius; }

	//! @brief 衝突属性の取得
	uint32_t GetCollisionAttribute() { return collisionAttribute_; }

	//! @brief 衝突属性の設定
	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; }

	//! @brief 衝突マスクの取得
	uint32_t GetCollisionMask() { return collisionMask_; }

	//! @brief 衝突マスクの設定
	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	float radius_ = 0.0f; // default

	// 衝突属性 (自分)
	uint32_t collisionAttribute_ = UINT32_MAX;

	// 衝突マスク (相手)
	uint32_t collisionMask_ = UINT32_MAX;

};