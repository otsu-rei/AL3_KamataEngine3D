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
	Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
	void operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	/* Subtract */
	Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
	void operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	/* Multiply */
	Vector3 operator*(const Vector3& other) const { return {x * other.x, y * other.y, z * other.z}; }
	void operator*=(const Vector3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}
	// scalar
	Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
	void operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

};

using Vector3f = Vector3;
// 自作のほうでVector3fで慣れているため