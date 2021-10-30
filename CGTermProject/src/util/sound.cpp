#include <iostream>
#include <string>
#include "sound.h"

irrklang::ISoundEngine* sound_engine = nullptr;


void InitSoundEngine()
{
	sound_engine = irrklang::createIrrKlangDevice();
	if (sound_engine == nullptr)
	{
		std::cout << "Error: Can't initialize sound engine." << std::endl;
		return;
	}

	// caching sounds
	sound_engine->play2D(SOUND_BGM);
	sound_engine->play2D(SOUND_BALL_COLLIDE(0));
	sound_engine->play2D(SOUND_BALL_COLLIDE(1));
	sound_engine->play2D(SOUND_BALL_COLLIDE(2));
	sound_engine->play2D(SOUND_CUE_PUSH(0));
	sound_engine->play2D(SOUND_CUE_PUSH(1));
	sound_engine->stopAllSounds();
}

irrklang::ISoundEngine* GetSoundEngine()
{
	return sound_engine;
}

void DestroySoundEngine()
{
	if (sound_engine == nullptr)
		return;

	sound_engine->drop();
	sound_engine = nullptr;
}
