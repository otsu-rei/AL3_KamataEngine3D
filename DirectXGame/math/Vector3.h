#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;
};

using Vector3f = Vector3;
// 自作のほうでVector3fで慣れているため