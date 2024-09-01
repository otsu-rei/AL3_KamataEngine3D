#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <list>
#include <memory>

//* engine
#include <Model.h>
#include <ViewProjection.h>
#include <WorldTransform.h>

//* Game
#include <GameCamera.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TitleModels class
////////////////////////////////////////////////////////////////////////////////////////////
class TitleModels {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TitleModels() = default;
	~TitleModels() = default;

	void Init();

	void Term();

	void Update(GameCamera* camera);

	void Draw(const ViewProjection& viewProj);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ModelType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ModelType {
		JYUU,
		RYOKU,
		MENN,
		TAI,

		kCountOfModelType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* IA *//

	std::unique_ptr<Model> models_[kCountOfModelType];
	WorldTransform         transforms_[kCountOfModelType];

	Quaternion quaternions_[kCountOfModelType];
	Quaternion targetQuaterion_;

	//* parameter *//

	const float kDistance_ = 8.0f;

};