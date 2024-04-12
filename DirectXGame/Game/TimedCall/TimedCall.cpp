#include "TimedCall.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TimedCall class methods
////////////////////////////////////////////////////////////////////////////////////////////

TimedCall::TimedCall(const std::function<void(void)>& function, uint32_t time)
	: function_(function), time_(time) {
	//
}

void TimedCall::Update() {
	if (isFinished_) { return; } //!< 完了してるので早期return

	time_--;

	if (time_ <= 0) {
		isFinished_ = true;
		function_();
	}

}
