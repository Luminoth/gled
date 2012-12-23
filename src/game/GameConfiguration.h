#if !defined __GAMECONFIGURATION_H__
#define __GAMECONFIGURATION_H__

#include "src/engine/EngineConfiguration.h"

class GameConfiguration : public energonsoftware::EngineConfiguration
{
public:
    static GameConfiguration& instance();

public:
    virtual ~GameConfiguration() throw();

public:
    bool load_config();
    bool save_config();
    bool config_exists() const;

public:
    int video_width() const { return std::atoi(get("video", "width").c_str()); }
    int video_height() const { return std::atoi(get("video", "height").c_str()); }
    int video_depth() const { return std::atoi(get("video", "depth").c_str()); }
    bool video_fullscreen() const { return energonsoftware::to_boolean(get("video", "fullscreen").c_str()); }

    float game_fov() const { return std::atof(get("game", "fov").c_str()); }

    float input_sensitivity() const { return std::atof(get("input", "sensitivity").c_str()); }

public:
    virtual void validate() const throw(energonsoftware::ConfigurationError);

private:
    virtual void on_save();

private:
    GameConfiguration();
    DISALLOW_COPY_AND_ASSIGN(GameConfiguration);
};

#endif
