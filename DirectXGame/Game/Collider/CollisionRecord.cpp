#include "CollisionRecord.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionRecord class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionRecord::AddRecord(uint32_t number) {
	// 履歴に登録
	record_.insert(number);
}

void CollisionRecord::ClearRecord() {
	record_.clear();
}

bool CollisionRecord::CheckRecord(uint32_t number) { 
	// numberが履歴にあるか確認
	auto it = record_.find(number);
	return it != record_.end();
}
