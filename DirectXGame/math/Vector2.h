#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	//=========================================================================================
	// operator
	//=========================================================================================
	/* Add */
	Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
	void operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
	}

	/* Subtruct */
	Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
	void operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
	}

	/* Multiply */
	Vector2 operator*(const Vector2& other) const { return {x * other.x, y * other.y}; }
	void operator*=(const Vector2& other) {
		x *= other.x;
		y *= other.y;
	}
	// scalar
	Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
	void operator*=(float scalar) {
		x *= scalar;
		y += scalar;
	}

};
