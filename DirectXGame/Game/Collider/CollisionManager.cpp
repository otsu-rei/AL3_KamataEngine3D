#include "CollisionManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::AddCollider(Collider* collider) {
	colliders_.push_back(collider);
}

void CollisionManager::CheckAllCollision() {

	// リスト内のペアの総当たり
	auto itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {

		auto itrB = itrA; //!< 自身に当たることを避けるため
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			CheckCollisionPair(*itrA, *itrB);
		}
	}

	// すべての衝突判定が終了したので一度clear
	colliders_.clear();
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask())
		|| !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return; //!< 属性(自分)とマスク(判定先)が一致しない場合
	}

	float lenght = Vector::Length(colliderA->GetWorldPosition() - colliderB->GetWorldPosition());

	if (lenght <= colliderA->GetRadius() + colliderB->GetRadius()) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}
