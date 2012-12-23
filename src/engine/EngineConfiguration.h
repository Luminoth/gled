#if !defined __ENGINECONFIGURATION_H__
#define __ENGINECONFIGURATION_H__

#include "src/core/configuration/Configuration.h"
#include "src/core/util/string_util.h"
#include "src/core/util/util.h"

namespace energonsoftware {

class EngineConfiguration : public Configuration
{
public:
    // NOTE: this does *not* instantiate a new configuration
    // a subclass must be created with its own instance() method
    // that creates the proper config object, which must call
    // this classes constructor, which in turn saves a pointer
    // to that as the engine config instance
    static EngineConfiguration& instance();

private:
    static EngineConfiguration* configuration;

public:
    virtual ~EngineConfiguration() throw();

public:
    void render_mode(const std::string& mode) { set("renderer", "mode", mode); }
    std::string render_mode() const { return get("renderer", "mode"); }
    bool render_mode_vertex() const { return "vertex" == get("renderer", "mode"); }
    bool render_mode_bump() const { return "bump" == get("renderer", "mode"); }

    void render_shadows(bool enable) { set("renderer", "shadows", to_string(enable)); }
    bool render_shadows() const { return to_boolean(get("renderer", "shadows").c_str()); }

    int memory_pool() const { return std::atoi(get("memory", "pool").c_str()); }

    bool video_sync() const { return to_boolean(get("video", "sync").c_str()); }
    int video_maxfps() const { return std::atoi(get("video", "maxfps").c_str()); }

    // debugging
    void render_wireframe(bool enable) { set("debug", "wireframe", to_string(enable)); }
    bool render_wireframe() const { return to_boolean(get("debug", "wireframe")); }

    void render_skeleton(bool enable) { set("debug", "skeleton", to_string(enable)); }
    bool render_skeleton() const { return to_boolean(get("debug", "skeleton")); }

    void render_normals(bool enable) { set("debug", "normals", to_string(enable)); }
    bool render_normals() const { return to_boolean(get("debug", "normals")); }

    void render_bounds(bool enable) { set("debug", "bounds", to_string(enable)); }
    bool render_bounds() const { return to_boolean(get("debug", "bounds")); }

    void render_lights(bool enable) { set("debug", "lights", to_string(enable)); }
    bool render_lights() const { return to_boolean(get("debug", "lights")); }

void rotate_actors(bool enable) { set("debug", "rotate_actors", to_string(enable)); }
bool rotate_actors() const { return to_boolean(get("debug", "rotate_actors")); }

public:
    virtual void validate() const throw(ConfigurationError);

protected:
    EngineConfiguration();

private:
    DISALLOW_COPY_AND_ASSIGN(EngineConfiguration);
};

}

#endif
