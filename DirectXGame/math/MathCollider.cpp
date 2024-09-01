#include "MathCollider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f MathCollider::Project(const Vector3f& v1, const Vector3f& v2) {
	Vector3f result;

	Vector3f normalv2 = Vector::Normalize(v2);
	result = normalv2 * Vector::Dot(v1, normalv2);

	return result;
}

Vector3f MathCollider::ClosestPoint(const Vector3f& point, const Segment& segment) {
	Vector3f result;

	result = segment.origin + Project(point - segment.origin, segment.diff);

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Collider methods
////////////////////////////////////////////////////////////////////////////////////////////

bool MathCollider::SphereTo(const Sphere& a, const Sphere& b) {
	float distance = Vector::Length(a.center - b.center);

	if (distance <= a.radius + b.radius) {
		return true;
	}

	return false;
}

bool MathCollider::PlaneToSphere(const Plane& plane, const Sphere& sphere) {

	float distance = Vector::Dot(plane.normal, sphere.center) - plane.distance;
	distance = distance < 0.0f ? -distance : distance; // 絶対値

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool MathCollider::PlaneToLine(const Plane& plane, const Line& line) {
	float dot = Vector::Dot(line.diff, plane.normal);

	if (dot == 0.0f) {
		return false;
	}

	return true;
}

bool MathCollider::PlaneToSegment(const Plane& plane, const Segment& segment) {
	float dot = Vector::Dot(segment.diff, plane.normal);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Vector::Dot(segment.origin, plane.normal)) / dot;

	if (t < 0.0f || t > 1.0f) {
		return false; // segment(線分)なのでtが0~1
	}

	return true;
}

bool MathCollider::SegmentToTriangle(const Segment& segment, const Triangle& triangle) {

	Vector3f n = Vector::Cross(triangle.virtices[1] - triangle.virtices[0], triangle.virtices[2] - triangle.virtices[1]);
	n = Vector::Normalize(n);

	float dot = Vector::Dot(segment.diff, n);

	if (dot == 0.0f) {
		return false; // 平行なので
	}

	float d = Vector::Dot(triangle.virtices[0], n);
	d = d < 0.0f ? d * -1 : d;

	float t = d - Vector::Dot(segment.origin, n) / dot;

	if (t < 0.0f || t > 1.0f) {
		return false; // segment(線分)なのでtが0~1
	}

	Vector3f p = segment.origin + segment.diff * t;

	for (int i = 0; i < 3; ++i) {
		int next = (i + 1) % 3;
		Vector3f cross = Vector::Cross(triangle.virtices[next] - triangle.virtices[i], p - triangle.virtices[next]);

		if (Vector::Dot(cross, n) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool MathCollider::AABBTo(const AABB& a, const AABB& b) {
	if (All(a.min <= b.max) && All(a.max >= b.min)) {
		return true;
	}

	return false;
}

bool MathCollider::AABBToSphere(const AABB& aabb, const Sphere& sphere) {

	Vector3f closestPoint = Vector::Clamp(sphere.center, aabb.min, aabb.max);

	float distance = Vector::Length(closestPoint - sphere.center);

	if (distance <= sphere.radius) {
		return true;
	}

	return false;
}

bool MathCollider::AABBToSegment(const AABB& aabb, const Segment& segment) {

	Vector3f tmin = (aabb.min - segment.origin) / segment.diff; // aabbのminのt
	Vector3f tmax = (aabb.max - segment.origin) / segment.diff; // aabbのmaxのt

	if (Vector::IsNaN(tmin) || Vector::IsNaN(tmax)) { // NaN
		return false;
	}

	Vector3f near = {
	    std::min(tmin.x, tmax.x),
	    std::min(tmin.y, tmax.y),
	    std::min(tmin.z, tmax.z),
	};

	Vector3f far = {
	    std::max(tmin.x, tmax.x),
	    std::max(tmin.y, tmax.y),
	    std::max(tmin.z, tmax.z),
	};

	float min = std::max(std::max(near.x, near.y), near.z);
	float max = std::min(std::min(far.x, far.y), far.z);

	if (max < 0.0f || min > 1.0f) {
		return false; //!< segmentなので
	}

	if (min <= max) {
		return true;
	}

	return false;
}

bool MathCollider::AABBToRay(const AABB& aabb, const Ray& ray) {

	Vector3f tmin = (aabb.min - ray.origin) / ray.diff; // aabbのminのt
	Vector3f tmax = (aabb.max - ray.origin) / ray.diff; // aabbのmaxのt

	if (Vector::IsNaN(tmin) || Vector::IsNaN(tmax)) { // NaN
		return false;
	}

	Vector3f near = {
	    std::min(tmin.x, tmax.x),
	    std::min(tmin.y, tmax.y),
	    std::min(tmin.z, tmax.z),
	};

	Vector3f far = {
	    std::max(tmin.x, tmax.x),
	    std::max(tmin.y, tmax.y),
	    std::max(tmin.z, tmax.z),
	};

	float min = std::max(std::max(near.x, near.y), near.z);
	float max = std::min(std::min(far.x, far.y), far.z);

	if (max < 0.0f) {
		return false; //!< rayなので
	}

	if (min <= max) {
		return true;
	}

	return false;
}

bool MathCollider::AABBToLine(const AABB& aabb, const Line& line) {

	Vector3f tmin = (aabb.min - line.origin) / line.diff; // aabbのminのt
	Vector3f tmax = (aabb.max - line.origin) / line.diff; // aabbのmaxのt

	if (Vector::IsNaN(tmin) || Vector::IsNaN(tmax)) { // NaN
		return false;
	}

	Vector3f near = {
	    std::min(tmin.x, tmax.x),
	    std::min(tmin.y, tmax.y),
	    std::min(tmin.z, tmax.z),
	};

	Vector3f far = {
	    std::max(tmin.x, tmax.x),
	    std::max(tmin.y, tmax.y),
	    std::max(tmin.z, tmax.z),
	};

	float min = std::max(std::max(near.x, near.y), near.z);
	float max = std::min(std::min(far.x, far.y), far.z);

	//!< lineなので例外なし

	if (min <= max) {
		return true;
	}

	return false;
}

bool MathCollider::OBBToSphere(const OBB& obb, const Sphere& sphere) {

	Vector3f centerInOBBLocal = Matrix::Transform(sphere.center, (obb.orientation * Matrix::MakeTranslate(obb.center)).Inverse());

	AABB aabbInOBBLocal = {
	    .min = -obb.size,
	    .max = obb.size,
	};

	Sphere sphereInOBBLocal = {.center = centerInOBBLocal, .radius = sphere.radius};

	return AABBToSphere(aabbInOBBLocal, sphereInOBBLocal);
}

bool MathCollider::OBBToSegment(const OBB& obb, const Segment& segment) {

	Matrix4x4 obbWorldInverse = (obb.orientation * Matrix::MakeTranslate(obb.center)).Inverse();

	Vector3f obbLocalOrigin = Matrix::Transform(segment.origin, obbWorldInverse);
	Vector3f obbLocalEnd = Matrix::Transform(segment.origin + segment.diff, obbWorldInverse);

	AABB obbLocalAABB = {
	    .min = -obb.size,
	    .max = obb.size,
	};

	Segment obbLocalSegment = {.origin = obbLocalOrigin, .diff = obbLocalEnd - obbLocalOrigin};

	return AABBToSegment(obbLocalAABB, obbLocalSegment);
}

bool MathCollider::OBBToRay(const OBB& obb, const Ray& ray) {
	Matrix4x4 obbWorldInverse = (obb.orientation * Matrix::MakeTranslate(obb.center)).Inverse();

	Vector3f obbLocalOrigin = Matrix::Transform(ray.origin, obbWorldInverse);
	Vector3f obbLocalEnd = Matrix::Transform(ray.origin + ray.diff, obbWorldInverse);

	AABB obbLocalAABB = {
	    .min = -obb.size,
	    .max = obb.size,
	};

	Ray obbLocalRay = {.origin = obbLocalOrigin, .diff = obbLocalEnd - obbLocalOrigin};

	return AABBToRay(obbLocalAABB, obbLocalRay);
}

bool MathCollider::OBBTo(const OBB& a, const OBB& b) {

	Vector3f axes[3 + 3 + 9];

	Vector3f obbAxesA[3];
	Vector3f obbAxesB[3];

	obbAxesA[0] = Matrix::Transform({1.0f, 0.0f, 0.0f}, a.orientation);
	obbAxesA[1] = Matrix::Transform({0.0f, 1.0f, 0.0f}, a.orientation);
	obbAxesA[2] = Matrix::Transform({0.0f, 0.0f, 1.0f}, a.orientation);

	obbAxesB[0] = Matrix::Transform({1.0f, 0.0f, 0.0f}, b.orientation);
	obbAxesB[1] = Matrix::Transform({0.0f, 1.0f, 0.0f}, b.orientation);
	obbAxesB[2] = Matrix::Transform({0.0f, 0.0f, 1.0f}, b.orientation);

	int k = 0;

	axes[k++] = obbAxesA[0];
	axes[k++] = obbAxesA[1];
	axes[k++] = obbAxesA[2];

	axes[k++] = obbAxesB[0];
	axes[k++] = obbAxesB[1];
	axes[k++] = obbAxesB[2];

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[k++] = Vector::Cross(obbAxesA[i], obbAxesB[j]);
		}
	}

	for (int i = 0; i < 15; ++i) {

		Vector3f axisNormalize = Vector::Normalize(axes[i]);

		float obbAProjection =
		    std::abs(a.size.x * Vector::Dot(obbAxesA[0], axisNormalize)) + std::abs(a.size.y * Vector::Dot(obbAxesA[1], axisNormalize)) + std::abs(a.size.z * Vector::Dot(obbAxesA[2], axisNormalize));

		float obbBProjection =
		    std::abs(b.size.x * Vector::Dot(obbAxesB[0], axisNormalize)) + std::abs(b.size.y * Vector::Dot(obbAxesB[1], axisNormalize)) + std::abs(b.size.z * Vector::Dot(obbAxesB[2], axisNormalize));

		float disatnce = std::abs(Vector::Dot(b.center - a.center, axisNormalize));

		if (disatnce > obbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}