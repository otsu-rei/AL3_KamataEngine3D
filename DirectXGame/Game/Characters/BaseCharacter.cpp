#include "BaseCharacter.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseCharacter Base class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f BaseCharacter::GetCenterPosition() const { 
	Vector3f result;

	result.x = worldTransform_.matWorld_.m[3][0];
	result.y = worldTransform_.matWorld_.m[3][1];
	result.z = worldTransform_.matWorld_.m[3][2];

	return result;
}