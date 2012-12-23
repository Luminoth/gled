#if !defined __AUDIO_H__
#define __AUDIO_H__

namespace energonsoftware {

class Audio
{
private:
    static bool init();
    static void shutdown();

private:
    static Logger& logger;

public:
    virtual ~Audio() throw();

private:
    friend class Engine;

private:
    Audio();
    DISALLOW_COPY_AND_ASSIGN(Audio);
};

}

#endif
