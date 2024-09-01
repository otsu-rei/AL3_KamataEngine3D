#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include <IScene.h>

// c++
#include <memory>

// engine
#include <Sprite.h>
#include <Model.h>
#include <WorldTransform.h>
#include <DebugCamera.h>

// Game
#include <Player.h>
#include <Field.h>
#include <GameCamera.h>
#include <TitleModels.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class
////////////////////////////////////////////////////////////////////////////////////////////
class Scene_Title
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

	/*std::unique_ptr<Model> model_;
	WorldTransform transform_;*/

	std::unique_ptr<TitleModels> models_;

	std::unique_ptr<Field> field_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<GameCamera> gameCamera_;

};