#include "src/pch.h"
#include "Audio.h"

namespace energonsoftware {

Logger& Audio::logger(Logger::instance("gled.engine.audio.Audio"));

bool Audio::init()
{
    LOG_INFO("Initializing audio...\n");

    alutInit(NULL, NULL);

    return true;
}

void Audio::shutdown()
{
    alutExit();
}

Audio::Audio()
{
}

Audio::~Audio() throw()
{
}

}
