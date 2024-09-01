#include "Background.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <TextureManager.h>
#include <WinApp.h>
#include <MyMath.h>
#include <Input.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Background class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Background::Init() {

	white1x1TextureHandle_ = TextureManager::Load("white1x1.png");

	backSprite_.reset(Sprite::Create(white1x1TextureHandle_, {WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}));
	backSprite_->SetSize({WinApp::kWindowWidth, WinApp::kWindowHeight});
	backSprite_->SetAnchorPoint({0.5f, 0.5f});
}

void Background::Term() {}

void Background::Update() {

	//* emit *//
	if (++currentTime_ >= emitTime_) { //!< 現在時間がemitTimeに到達していたら,
		currentTime_ -= emitTime_;     // 次のemitTimeに備える

		// particleの発射
		for (int i = 0; i < 3; ++i) {
			particles_.emplace_back();
			particles_.back().Create(white1x1TextureHandle_);
		}
	}

	XINPUT_STATE state = {};

	auto input = Input::GetInstance();
	if (input->GetJoystickState(0, state)) {
		Vector2f stickR = {
		    static_cast<float>(state.Gamepad.sThumbRX) / SHRT_MAX,
		    static_cast<float>(state.Gamepad.sThumbRY) / SHRT_MAX,
		};

		/*velocityX_ = (std::max)(velocityX_, stickR.x);*/

		if (std::abs(velocityX_) < std::abs(stickR.x)) {
			velocityX_ = stickR.x;
		}
	}

	for (auto& particle : particles_) {
		particle.Update({velocityX_, 2.0f});
	}

	velocityX_ *= 0.96f;

	particles_.remove_if([](auto& particle) {
		return particle.isDelete;
	});


}

void Background::Draw() {
	backSprite_->Draw();

	for (const auto& particle : particles_) {
		particle.Draw();
	}
}
