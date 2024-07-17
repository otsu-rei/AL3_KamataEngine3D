#include "CollisionManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::Reset() {
	colliders_.clear();
	//
}

void CollisionManager::CheckAllCollisions() {

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itA = colliders_.begin();
	for (; itA != colliders_.end(); ++itA) {
		
		// itBはAと重複してはいけないので次の要素から回す
		std::list<Collider*>::iterator itB = itA;
		itB++;

		for (; itB != colliders_.end(); ++itB) {
			CheckCollisionPair(*itA, *itB);
		}
	}

}

void CollisionManager::AddCollider(Collider* collider) {
	colliders_.push_back(collider);
}

void CollisionManager::CheckCollisionPair(Collider* a, Collider* b) {

	Vector3f positionA = a->GetCenterPosition();
	Vector3f positionB = b->GetCenterPosition();

	Vector3f subtract = positionB - positionA;
	float distance = Vector::Length(subtract);

	float radius = a->GetRadius() + b->GetRadius();

	if (distance < radius) {
		a->OnCollision();
		b->OnCollision();
	}

}
