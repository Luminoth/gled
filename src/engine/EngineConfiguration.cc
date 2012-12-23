#include "src/pch.h"
#include "EngineConfiguration.h"

namespace energonsoftware {

EngineConfiguration* EngineConfiguration::configuration = NULL;

EngineConfiguration& EngineConfiguration::instance()
{
    return *configuration;
}

EngineConfiguration::EngineConfiguration()
    : Configuration()
{
    configuration = this;

    set_default("renderer", "mode", "bump");
    set_default("renderer", "shadows", "true");

    set_default("memory", "pool", "50");

    set_default("video", "sync", "false");
    set_default("video", "maxfps", "-1");

    // debugging
    set_default("debug", "wireframe", "false");
    set_read_only("debug", "wireframe");

    set_default("debug", "skeleton", "false");
    set_read_only("debug", "skeleton");

    set_default("debug", "normals", "false");
    set_read_only("debug", "normals");

    set_default("debug", "bounds", "false");
    set_read_only("debug", "bounds");

    set_default("debug", "lights", "false");
    set_read_only("debug", "lights");

set_default("debug", "rotate_actors", "false");
set_read_only("debug", "rotate_actors");
}

EngineConfiguration::~EngineConfiguration() throw()
{
}

void EngineConfiguration::validate() const throw(ConfigurationError)
{
    Configuration::validate();

    if(!is_int(get("memory", "pool"))) {
        throw ConfigurationError("Memory pool must be an integer");
    }

    if(video_maxfps() > 0 && video_maxfps() < 30) {
        throw ConfigurationError("Video maxfps must be at least 30!");
    }

    if(!render_mode_vertex() && !render_mode_bump()) {
        throw ConfigurationError("Invalid render mode: " + render_mode());
    }
}

}
