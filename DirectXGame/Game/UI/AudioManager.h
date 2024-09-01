#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Audio.h>

// c++
#include <unordered_map>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// SoundManager class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* handles option *//

	void LoadAudio(const std::string& name, const std::string& filepath);

	void PlayAudio(const std::string& name, bool loop = false, float volume = 1.0f);

	//* singleton *//

	static AudioManager* GetInstance();
	AudioManager() = default;
	~AudioManager() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	Audio* audio_ = Audio::GetInstance();

	//* handles *//

	//! [unordered_map]
	//! key:   name
	//! value: audio handle
	std::unordered_map<std::string, uint32_t> handles_;

};