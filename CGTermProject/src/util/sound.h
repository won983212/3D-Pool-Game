#pragma once
#include <irrklang/irrKlang.h>

#define SOUND_BGM "res/sound/bgm.wav"
#define SOUND_BALL_COLLIDE(power_str) ("res/sound/ball_" + std::to_string(power_str) + ".wav").c_str()
#define SOUND_CUE_PUSH(power_str) ("res/sound/cue_" + std::to_string(power_str) + ".wav").c_str()

irrklang::ISoundEngine* GetSoundEngine();
void DestroySoundEngine();
void InitSoundEngine();
