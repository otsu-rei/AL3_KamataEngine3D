#include "FieldFace.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// FieldFace class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FieldFace::Init(const Vector3f& normal, const Vector3f& fieldScale) {
	const float space = 4.0f; //!< parameters

	const Vector3f inverseNormal = normal * -1;

	transform_.translation_ = fieldScale * inverseNormal;

	Vector3f min = -fieldScale * 2.0f;
	Vector3f max = fieldScale * 2.0f;
	

	if (inverseNormal.x != 0.0f) { //!< left || right
		min.x = max.x = fieldScale.x * inverseNormal.x;

		min.x = (std::min)(min.x, min.x + space * inverseNormal.x);
		max.x = (std::max)(max.x, max.x + space * inverseNormal.x);
	
	} else if (inverseNormal.y != 0.0f) { //!< top || bottom
		min.y = max.y = fieldScale.y * inverseNormal.y;

		min.y = (std::min)(min.y, min.y + space * inverseNormal.y);
		max.y = (std::max)(max.y, max.y + space * inverseNormal.y);
	
	} else if (inverseNormal.z != 0.0f) { //!< front || back
		min.z = max.z = fieldScale.z * inverseNormal.z;

		min.z = (std::min)(min.z, min.z + space * inverseNormal.z);
		max.z = (std::max)(max.z, max.z + space * inverseNormal.z);
	}

	Collider::Init();
	Collider::SetColliderBoundingAABB({min, max});
	Collider::typeId_ = kBox;
}
