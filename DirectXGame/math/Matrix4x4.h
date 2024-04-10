#pragma once
/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final {
	float m[4][4];

	//=========================================================================================
	// operator
	//=========================================================================================

	/* Add */
	Matrix4x4 operator+(const Matrix4x4& other) const {
		Matrix4x4 result;

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				result.m[row][column] = m[row][column] + other.m[row][column];
			}
		}

		return result;
	}
	void operator+=(const Matrix4x4& other) {
		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				m[row][column] += other.m[row][column];
			}
		}
	}

	/* Subtract */
	Matrix4x4 operator-(const Matrix4x4& other) const {
		Matrix4x4 result;

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				result.m[row][column] = m[row][column] - other.m[row][column];
			}
		}

		return result;
	}
	void operator-=(const Matrix4x4& other) {

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				m[row][column] -= other.m[row][column];
			}
		}
	}

	/* Multiply */
	Matrix4x4 operator*(const Matrix4x4& other) const {
		Matrix4x4 result = {0.0f};

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				for (int i = 0; i < 4; i++) {
					result.m[row][column] += m[row][i] * other.m[i][column];
				}
			}
		}

		return result;
	}
	void operator*=(const Matrix4x4& other) {
		Matrix4x4 result = {0.0f};

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				for (int i = 0; i < 4; i++) {
					result.m[row][column] += m[row][i] * other.m[i][column];
				}
			}
		}

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				m[row][column] = result.m[row][column];
			}
		}
	}

};

