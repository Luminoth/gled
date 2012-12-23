#include "src/pch.h"
#include "src/core/common.h"
#include "GameConfiguration.h"

GameConfiguration& GameConfiguration::instance()
{
    static boost::shared_ptr<GameConfiguration> configuration;
    if(!configuration) {
        // NOTE: this doesn't go on an allocator
        configuration.reset(new GameConfiguration());
    }
    return *configuration;
}

GameConfiguration::GameConfiguration()
    : energonsoftware::EngineConfiguration()
{
    set_default("video", "width", "1280");
    set_default("video", "height", "720");
    set_default("video", "depth", "32");
    set_default("video", "fullscreen", "false");

    set_default("game", "fov", "75.0");

    set_default("input", "sensitivity", "1.0");

    set_default("logging", "filename", (energonsoftware::home_conf_dir() / "gled.log").string());

    load_defaults();

    generate_header("Game configuration file");
}

GameConfiguration::~GameConfiguration() throw()
{
}

bool GameConfiguration::load_config()
{
    return load(energonsoftware::game_conf());
}

bool GameConfiguration::save_config()
{
    return save(energonsoftware::game_conf());
}

bool GameConfiguration::config_exists() const
{
    return exists(energonsoftware::game_conf());
}

void GameConfiguration::validate() const throw(energonsoftware::ConfigurationError)
{
    energonsoftware::EngineConfiguration::validate();

    if(!energonsoftware::is_int(get("video", "width"))) {
        throw energonsoftware::ConfigurationError("Video width must be an integer");
    }

    if(!energonsoftware::is_int(get("video", "height"))) {
        throw energonsoftware::ConfigurationError("Video height must be an integer");
    }

    if(!energonsoftware::is_int(get("video", "depth"))) {
        throw energonsoftware::ConfigurationError("Video depth must be an integer");
    }

    if(!energonsoftware::is_double(get("game", "fov"))) {
        throw energonsoftware::ConfigurationError("Game fov must be a float");
    }

    if(!energonsoftware::is_double(get("input", "sensitivity"))) {
        throw energonsoftware::ConfigurationError("Input sensitivity must be a float");
    }
}

void GameConfiguration::on_save()
{
}
