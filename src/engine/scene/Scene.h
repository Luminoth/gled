#if !defined __SCENE_H__
#define __SCENE_H__

#include "src/engine/renderer/Camera.h"

namespace energonsoftware {

class Lexer;
class Map;
class Physical;
class Renderable;
class PositionalLight;
class SpotLight;

//class Q3BSP;

class Window;

class Scene
{
public:
    typedef boost::function<void (float, const std::string&)> LoadProgressCallback;

private:
    static Logger& logger;

public:
    virtual ~Scene() throw();

public:
    void load_progress_callback(LoadProgressCallback callback) { _callback = callback; }

    bool load(const boost::filesystem::path& filename);
    void unload();

    void register_physical(boost::shared_ptr<Physical> physical) { _physicals.push_back(physical); }

    void register_renderable(boost::shared_ptr<Renderable> renderable) { _renderables.push_back(renderable); }
    bool renderables_ready() const;
    void init_renderables();

    void update();

    void create_scene_graph();
    void render_geometry();
    void render_lit();
    void render_unlit();

public:
    bool loaded() const { return _loaded; }

    MemoryAllocator& allocator() { return *_allocator; }

    const Camera& camera() const { return _camera; }
    Camera& camera() { return _camera; }

    const Map& map() const { return *_map; }
    Map& map() { return *_map; }

private:
    void callback(float percent, const std::string& status);

    bool scan_map(Lexer& lexer);
    bool scan_global_ambient_color(Lexer& lexer);
    bool scan_models(Lexer& lexer);
    bool scan_renderables(Lexer& lexer);
    bool scan_static(Lexer& lexer);
    bool scan_actor(Lexer& lexer, const std::string& type);
    bool scan_lights(Lexer& lexer);
    bool scan_light_positional(Lexer& lexer, boost::shared_ptr<PositionalLight> positional);
    bool scan_light_spotlight(Lexer& lexer, boost::shared_ptr<SpotLight> spot);

private:
    bool _loaded;
    LoadProgressCallback _callback;

    boost::shared_ptr<MemoryAllocator> _allocator;

    Camera _camera;

    boost::shared_ptr<Map> _map;

    std::vector<boost::shared_ptr<Physical> > _physicals;
    std::vector<boost::shared_ptr<Renderable> > _renderables;
    std::vector<boost::shared_ptr<Window> > _windows;

    std::vector<boost::shared_ptr<Renderable> > _visible_renderables;
    std::vector<boost::shared_ptr<Renderable> > _pickable_renderables;
std::vector<boost::shared_ptr<Renderable> > _lit_renderables;

/*public:
boost::shared_ptr<Q3BSP> _bsp;*/

private:
    friend class State;

private:
    Scene();
    DISALLOW_COPY_AND_ASSIGN(Scene);
};

}

#endif
