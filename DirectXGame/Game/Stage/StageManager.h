#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <string>

// adapter
#include <Json.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StageManager class
////////////////////////////////////////////////////////////////////////////////////////////
class StageManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StageManager() = default;
	~StageManager() = default;

	bool IsNextStage() const;
	void SetNextStageNum();

	bool IsPreStage() const;
	void SetPreStageNum();

	const std::string& GetStageFilename() const { return kStageFilenames_[stageNum_]; }

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	//* parameter *//

	static const uint32_t    kMaxStageNum_ = 4;
	static const std::string kStageFilenames_[kMaxStageNum_];

	//* data *//

	uint32_t stageNum_ = 0;

};