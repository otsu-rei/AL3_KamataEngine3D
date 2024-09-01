#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <imgui.h>

// c++
#include <list>
#include <format>

// Game
#include <Box.h>
#include <Goal.h>
#include <Player.h>
#include <GameCamera.h>

// Json
#include "Adapter/Json.h"

////////////////////////////////////////////////////////////////////////////////////////////
// GrabityType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum GravityType {
	mX, //!< -x
	pX, //!< +x
	mY, //!< -y
	pY, //!< +y
	mZ, //!< -z
	pZ, //!< +z

	kCountOfGrabityType
};

////////////////////////////////////////////////////////////////////////////////////////////
// StageEditer class
////////////////////////////////////////////////////////////////////////////////////////////
class StageEditer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(GameCamera* camera);

	void Term();

	void Update();

	void ReactionUpdate();

	void Draw(const ViewProjection& viewProj);

	void UpdateImGui();

	//* Getter *//

	Player* GetPlayer() const { return player_.get(); }

	void OutputJson(const std::string& path);
	void LoadJson(const std::string& path);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BoxData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BoxData {
		Vector3f scale;
		Vector3f position;

		Json ToJson() {
			return Json{
			    {"scale",    JsonAdapter::ToJson(scale)},
			    {"position", JsonAdapter::ToJson(position)}
			};
		}

	};

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	Vector3f playerStartPosition_ = {0.0f, 0.0f, 0.0f};
	std::unique_ptr<Player> player_;

	std::list<std::unique_ptr<Box>> boxes_;

	std::unique_ptr<Goal> goal_;
	GravityType goalGrabityType_ = mY;

	static const Vector3f kGrabitys_[kCountOfGrabityType];
	static const LPCSTR kGrabityNames_[kCountOfGrabityType];

	//=========================================================================================
	// private methods
	//=========================================================================================

	


};