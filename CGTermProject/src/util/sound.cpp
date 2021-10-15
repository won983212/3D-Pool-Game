#include "sound.h"

irrklang::ISoundEngine* soundEngine = nullptr;

irrklang::ISoundEngine* getSoundEngine()
{
	if (soundEngine == nullptr)
		soundEngine = irrklang::createIrrKlangDevice();
	return soundEngine;
}

void destroySoundEngine()
{
	if (soundEngine == nullptr)
		return;

	soundEngine->drop();
	soundEngine = nullptr;
}