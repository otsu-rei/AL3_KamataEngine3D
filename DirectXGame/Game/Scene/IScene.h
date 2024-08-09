#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <memory>

// engine
#include "DirectXCommon.h"
#include "Input.h"
#include "Audio.h"

#include "ViewProjection.h"
#include "DebugCamera.h"

////////////////////////////////////////////////////////////////////////////////////////////
// IScene base class
////////////////////////////////////////////////////////////////////////////////////////////
class IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	IScene()          = default;
	virtual ~IScene() = default;

	//* virtual methods *//

	virtual void Init() = 0;

	virtual void Term() = 0;

	virtual void Update() = 0;

	virtual void DrawBackSprite() = 0;

	virtual void Draw3D() = 0;

	virtual void DrawLine() = 0;

	virtual void DrawFrontSprite() = 0;
	
	//* methods *//

	std::unique_ptr<IScene>& GetNextScene() { return nextScene_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<IScene> nextScene_ = nullptr;

	//* external *//

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	Input* input_ = Input::GetInstance();
	Audio* audio_ = Audio::GetInstance();

	//* camera *//
	ViewProjection viewProjection_;

	bool isDebugCameraActive_ = true;
	std::unique_ptr<DebugCamera> debugCamera_;

};
