#include "CollisionManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyMath.h>
#include "GlobalVariables.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::Init() {
	debugModel_.reset(Model::CreateFromOBJ("sphere"));
	//

	// グループの登録
	const std::string groupName = "collisionManager";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "is display", isDisplay_);
}

void CollisionManager::Draw(const ViewProjection& viewProj) {

	if (!isDisplay_) { //!< 非表示なら抜ける
		return;
	}

	for (auto& collider : colliders_) {
		collider->Draw(debugModel_.get(), viewProj);
	}
}

void CollisionManager::Reset() {
	colliders_.clear();
	//
}

void CollisionManager::UpdateWorldTransform() {

	// glovalVariablesから値の取得
	const std::string groupName = "collisionManager";
	isDisplay_ = globalVariables->GetValue<bool>(groupName, "is display");

	if (!isDisplay_) { //!< 非表示なら抜ける
		return;
	}

	for (auto& collider : colliders_) {
		collider->UpdateWorldTransform();
	}
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
		a->OnCollision(b);
		b->OnCollision(a);
	}

}
