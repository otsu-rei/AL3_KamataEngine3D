#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	//=========================================================================================
	// operator
	//=========================================================================================

		/* Add */
	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	/* Subtract */
	Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	/* Multiply */
	Vector3& operator*=(const Vector3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	/* Division */
	Vector3& operator/=(const Vector3& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	Vector3& operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

};

//=========================================================================================
// binary operator
//=========================================================================================

/* Add */
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }

/* Subtract */
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }

/* Multiply */
inline Vector3 operator*(const Vector3& v1, const Vector3& v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }

inline Vector3 operator*(const Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }

inline Vector3 operator*(float s, const Vector3& v) { return {s * v.x, s * v.y, s * v.z}; }

/* Division */
inline Vector3 operator/(const Vector3& v1, const Vector3& v2) { return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z}; }

inline Vector3 operator/(const Vector3& v, float s) { return {v.x / s, v.y / s, v.z / s}; }

//=========================================================================================
// unary operator
//=========================================================================================

inline Vector3 operator+(const Vector3& v) { return v; }

inline Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }

using Vector3f = Vector3;
// 自作のほうでVector3fで慣れているため