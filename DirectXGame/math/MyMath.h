#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Vector3.h"
#include "Matrix4x4.h"

////////////////////////////////////////////////////////////////////////////////////////////
// enum RotateType
////////////////////////////////////////////////////////////////////////////////////////////
enum RotateType {
	kRotateBaseX,
	kRotateBaseY,
	kRotateBaseZ,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Matrix { // 行列用関数

	Matrix4x4 MakeScale(const Vector3f& scale);

	Matrix4x4 MakeRotate(float radian, RotateType type);
	Matrix4x4 MakeRotate(const Vector3f& rotate);
	
	Matrix4x4 MakeRotateEuler(float radian, RotateType type);
	Matrix4x4 MakeRotateEuler(const Vector3f& rotate);
	
	Matrix4x4 MakeTranslate(const Vector3f& translate);
	
	Matrix4x4 MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate);
	
	Matrix4x4 Inverse(const Matrix4x4& m);
	
	Matrix4x4 Transpose(const Matrix4x4& m);

	// graphics matrix //
    Matrix4x4 MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
    Matrix4x4 MakeOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ);

    Matrix4x4 MakeViewport(float left, float top, float width, float height, float minDepth, float maxDepth);

	Vector3f Transform(const Vector3f& vector, const Matrix4x4& matrix);

	Vector3f TransformNormal(const Vector3f& v, const Matrix4x4& m);

}

////////////////////////////////////////////////////////////////////////////////////////////
// Vector namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Vector {

	/// Vector3 ///
	Vector3f Normalize(const Vector3f& v);
	
	float Length(const Vector3f& v);
	
	float Dot(const Vector3f& x, const Vector3f& y);
	
	Vector3f Cross(const Vector3f& x, const Vector3f& y);
	
	Vector3f Reflect(const Vector3f& i, const Vector3f& n);
	
	Vector3f Lerp(const Vector3f& x, const Vector3f& y, float s);

	Vector3f Slerp(const Vector3f& x, const Vector3f& y, float s);

	Vector3f Clamp(const Vector3f& v, const Vector3f& min, const Vector3f& max);

}

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////