#include "Effect.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EffectManager class method
////////////////////////////////////////////////////////////////////////////////////////////

void EffectManager::Init() {}

void EffectManager::Term() {}

void EffectManager::Update() {
	for (auto& effect : effects_) {
		effect->Update();
	}

	effects_.remove_if([](auto& effect) {
		if (!effect->IsAlive()) {
			return true;
		}

		return false;
	});
}

void EffectManager::Draw(ViewProjection& viewProj) {
	for (auto& effect : effects_) {
		effect->Draw(viewProj);
	}
}

void EffectManager::AddEffect(std::unique_ptr<Effect>& effect) {
	effects_.push_back(std::move(effect));
}
