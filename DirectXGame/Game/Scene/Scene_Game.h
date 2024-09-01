#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include <IScene.h>

// Game
#include "Box.h"
#include "Field.h"
#include "Goal.h"

#include "StageEditer.h"
#include "StageManager.h"

#include "Player.h"
#include "GameCamera.h"

#include "Background.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Game class
////////////////////////////////////////////////////////////////////////////////////////////
class Scene_Game
	: public IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Term() override;

	void Update() override;

	void DrawBackSprite() override;

	void Draw3D() override;

	void DrawLine() override;

	void DrawFrontSprite() override;

private:
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Field> field_;

	std::unique_ptr<StageManager> stageManager_;
	std::unique_ptr<StageEditer> stageEditor_;
	std::unique_ptr<GameCamera> gameCamera_;

};