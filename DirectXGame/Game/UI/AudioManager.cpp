#include "AudioManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SoundManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioManager::Init() {
	LoadAudio("bgm", "sounds/bgm.mp3");

	LoadAudio("rotation", "sounds/rotation.mp3");
	LoadAudio("cannot", "sounds/cannot.mp3");
	LoadAudio("clear", "sounds/clear.mp3");
}

void AudioManager::Term() {}

void AudioManager::LoadAudio(const std::string& name, const std::string& filepath) {

	uint32_t handle = audio_->LoadWave(filepath);
	handles_.emplace(name, handle);
}

void AudioManager::PlayAudio(const std::string& name, bool loop, float volume) {
	audio_->PlayWave(handles_.at(name), loop, volume);
}

AudioManager* AudioManager::GetInstance() {
	static AudioManager instance;
	return &instance;
}