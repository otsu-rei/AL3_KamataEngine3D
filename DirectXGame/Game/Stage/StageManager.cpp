#include "StageManager.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string StageManager::kStageFilenames_[kMaxStageNum_] = {
	"stage1.json",
	"stage2.json",
	"stage3.json",
	"stage4.json",

};

////////////////////////////////////////////////////////////////////////////////////////////
// StageManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool StageManager::IsNextStage() const {
	return stageNum_ + 1 < kMaxStageNum_;
}

void StageManager::SetNextStageNum() { 
	if (stageNum_ >= kMaxStageNum_ - 1) {
		return;
	}

	stageNum_++;
}

bool StageManager::IsPreStage() const {
	return stageNum_ > 0;
}

void StageManager::SetPreStageNum() {
	if (stageNum_ <= 0) {
		return;
	}

	stageNum_--;
}
