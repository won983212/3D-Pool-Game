#include <iostream>
#include "sound.h"

irrklang::ISoundEngine* soundEngine = nullptr;


void initSoundEngine()
{
	soundEngine = irrklang::createIrrKlangDevice();
	if (soundEngine == nullptr)
	{
		std::cout << "Error: Can't initialize sound engine." << std::endl;
		return;
	}

	// caching sounds
	soundEngine->play2D(SOUND_BGM);
	soundEngine->play2D(SOUND_BALL_COLLIDE(0));
	soundEngine->play2D(SOUND_BALL_COLLIDE(1));
	soundEngine->play2D(SOUND_BALL_COLLIDE(2));
	soundEngine->play2D(SOUND_CUE_PUSH(0));
	soundEngine->play2D(SOUND_CUE_PUSH(1));
	soundEngine->stopAllSounds();
}

irrklang::ISoundEngine* getSoundEngine()
{
	return soundEngine;
}

void destroySoundEngine()
{
	if (soundEngine == nullptr)
		return;

	soundEngine->drop();
	soundEngine = nullptr;
}