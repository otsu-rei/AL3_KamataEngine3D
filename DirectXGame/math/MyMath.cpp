#include "MyMath.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 Matrix::MakeScale(const Vector3f& scale) {
	return {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotateX(float radian) {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, std::cos(radian), std::sin(radian), 0.0f,
		0.0f, -std::sin(radian), std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotateY(float radian) {
	return {
		std::cos(radian), 0.0f, -std::sin(radian), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sin(radian), 0.0f, std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix::MakeRotateZ(float radian) {
	return {
		std::cos(radian), std::sin(radian), 0.0f, 0.0f,
		-std::sin(radian), std::cos(radian), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}


Matrix4x4 Matrix::MakeRotate(const Vector3f& rotate) {
	return MakeRotateX(rotate.x) * MakeRotateY(rotate.y) * MakeRotateZ(rotate.z);
}

Matrix4x4 Matrix::MakeRotate(const Quaternion& q) {
	return {
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z, 2.0f * (q.x * q.y + q.w * q.z), 2.0f * (q.x * q.z - q.w * q.y), 0.0f,
		2.0f * (q.x * q.y - q.w * q.z), q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z, 2.0f * (q.y * q.z + q.w * q.x), 0.0f,
		2.0f * (q.x * q.z + q.w * q.y), 2.0f * (q.y * q.z - q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeTranslate(const Vector3f& translate) {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};
}

Matrix4x4 Matrix::MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) {
	return MakeScale(scale) * MakeRotate(rotate) * MakeTranslate(translate);
}

Matrix4x4 Matrix::MakeAffine(const Vector3f& scale, const Quaternion& rotate, const Vector3f& translate) {
	return MakeScale(scale) * MakeRotate(rotate) * MakeTranslate(translate);
}

Matrix4x4 Matrix::MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	return {
		(1.0f / (aspectRatio * std::tan(fovY / 2.0f))), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / std::tan(fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
		0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip), 0.0f
	};
}

Matrix4x4 Matrix::MakeOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ) {
	return {
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearZ / (nearZ - farZ), 1.0f
	};
}

Matrix4x4 Matrix::MakeViewport(float left, float top, float width, float height, float minDepth, float maxDepth) {
	return {
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -height / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, maxDepth - minDepth, 0.0f,
		left + (width / 2.0f), top + (height / 2.0f), minDepth, 1.0f,
	};
}

Vector3f Matrix::Transform(const Vector3f& vector, const Matrix4x4& matrix) {
	Vector3f result;
	float w;
	
	result.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
	result.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
	result.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);
	w        = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);
	
	assert(w != 0.0f);
	
	result.x /= w;
	result.y /= w;
	result.z /= w;
	
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// Vector namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f Vector::Normalize(const Vector3f& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	if (length == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}

	return {v.x / length, v.y / length, v.z / length};
}

float Vector::Length(const Vector3f& v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

float Vector::Dot(const Vector3f& x, const Vector3f& y) { return x.x * y.x + x.y * y.y + x.z * y.z; }

Vector3f Vector::Cross(const Vector3f& x, const Vector3f& y) { return {x.y * y.z - x.z * y.y, x.z * y.x - x.x * y.z, x.x * y.y - x.y * y.x}; }

Vector3f Vector::Reflect(const Vector3f& i, const Vector3f& n) {
	float dot = Vector::Dot(i, n);
	Vector3f scalerNormal = n * (2.0f * dot);
	return i - scalerNormal;
}

Vector3f Vector::Lerp(const Vector3f& x, const Vector3f& y, float s) { return {x.x + s * (y.x - x.x), x.y + s * (y.y - x.y), x.z + s * (y.z - x.z)}; }

Vector3f Vector::Slerp(const Vector3f& x, const Vector3f& y, float s) {

	float dot = Vector::Dot(x, y);
	dot = std::clamp(dot, -1.0f, 1.0f);

	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);

	if (sinTheta == 0.0f) { //!< 0除算避け
		return x * (1.0f - s) + y * s;
	}

	float w1 = std::sin((1.0f - s) * theta) / sinTheta;
	float w2 = std::sin(s * theta) / sinTheta;

	Vector3f result = x * w1 + y * w2;

	return result;
}

Vector3f Vector::Clamp(const Vector3f& v, const Vector3f& min, const Vector3f& max) {

	Vector3f result = {
	    std::clamp(v.x, min.x, max.x),
	    std::clamp(v.y, min.y, max.y),
	    std::clamp(v.z, min.z, max.z),
	};

	return result;

}

Vector3f CatmullRomInterpolation(
	const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float t) {

	float t2 = t * t;
	float t3 = t2 * t;
	
	Vector3f e3 = -p0 + (p1 - p2) * 3.0f + p3; // t^3
	Vector3f e2 = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3; // t^2
	Vector3f e1 = -p0 + p2; // t
	Vector3f e0 = p1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;

}

Vector3f CatmullRomPosition(const std::vector<Vector3f>& points, float t) {

	assert(points.size() >= 4); //!< CatmullRomは4点以上必要

	// 区間数
	size_t division = points.size() - 1;

	// 1区間の長さ
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f, 終点を1.0fとしたときの現在の位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限と上限の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	size_t zero = 0; //!< size_tの0表記がわからなかったので変数に0を持たせる
	index = std::clamp(index, zero, points.size() - 2);

	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	if (index == 0) {
		index0 = index1;
	}

	if (index3 >= points.size()) {
		index3 = index2;
	}

	const Vector3f& p0 = points[index0];
	const Vector3f& p1 = points[index1];
	const Vector3f& p2 = points[index2];
	const Vector3f& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);

}

float LerpShortAngle(float a, float b, float t) {

	float diff = b - a;

	// clamp -2pi ~ 2pi
	diff = std::fmod(diff, pi_v * 2.0f);

	// clamp -pi ~ pi
	if (diff < -pi_v) {
		diff += pi_v * 2.0f;

	} else if (diff > pi_v) {
		diff -= pi_v * 2.0f;
	}

	return a + diff * t;

}

Vector3f CalculateDireciton(const Vector3f& velocity) {
	Vector3f result = {0.0f};

	// ロール, ピッチ, ロー回転
	result.y = std::atan2(velocity.x, velocity.z);

	float length = Vector::Length({velocity.x, 0.0f, velocity.z});
	result.x = std::atan2(-velocity.y, length);

	return result;
}
