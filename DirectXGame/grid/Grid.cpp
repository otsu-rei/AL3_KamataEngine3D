#include "Grid.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

void DrawGrid(
	const Vector3f& center,
	float gridLength, const uint32_t kSubdivision,
	const Vector4& color, const ViewProjection& viewproj) {

	if (kSubdivision == 0) { return; } //!< 描画しないので早期return
	// todo: kSubdivision = 1 の時の処理の追加

	// drawerInstanceの取り出し
	auto drawerIns = PrimitiveDrawer::GetInstance();
	drawerIns->SetViewProjection(&viewproj);

	// grid一本の長さ
	float kGridLength = gridLength / static_cast<float>(kSubdivision - 1);

	// grid半分の長さ
	float kGridHalfLength = gridLength / 2.0f;
	
	for (uint32_t index = 0; index < kSubdivision; ++index) {

		float localGridBorder = kGridHalfLength - (index * kGridLength);
		
		// z軸方向へのgrid線
		drawerIns->DrawLine3d(
			{center.x + localGridBorder, center.y, center.z - kGridHalfLength},
			{center.x + localGridBorder, center.y, center.z + kGridHalfLength},
			color
		);

		// x軸方法へのgrid線
		drawerIns->DrawLine3d(
			{center.x - kGridHalfLength, center.y, center.z + localGridBorder },
			{center.x + kGridHalfLength, center.y, center.z + localGridBorder },
			color
		);
	}

}
