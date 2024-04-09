#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>

// math
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4x4.h>

// engine
#include "PrimitiveDrawer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief グリッド線の描画
//! 
//! @param[in] center       gridの中心座標
//! @param[in] gridLength   グリッド線の長さ
//! @param[in] kSubdivision グリッド線の分割数
//! @param[in] color        グリッド線の色
//! @param[in] viewproj     描画するカメラのviewproj
void DrawGrid(
	const Vector3f& center,
	float gridLength, const uint32_t kSubdivision, const Vector4& color,
	const ViewProjection& viewproj
);