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

//-----------------------------------------------------------------------------------------
// consept
//-----------------------------------------------------------------------------------------
class IScene;

template<class T>
concept DerivedFormIScene = std::is_base_of_v<IScene, T>; //!< ISceneに継承してるクラスのみのtemplate

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

	template <DerivedFormIScene T>
	void SetNextScene() {
		nextScene_ = std::make_unique<T>();
		nextScene_->Init();
	}

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<IScene> nextScene_ = nullptr;

	//* external *// //<! staticにしてもいいかも

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	Input* input_ = Input::GetInstance();
	Audio* audio_ = Audio::GetInstance();

	//* camera *//
	ViewProjection viewProjection_;

	bool isDebugCameraActive_ = false;
	std::unique_ptr<DebugCamera> debugCamera_;

};
