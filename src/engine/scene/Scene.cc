#include "src/pch.h"
#include <iostream>
#include "src/core/common.h"
#include "src/core/math/math_util.h"
#include "src/core/util/util.h"
#include "src/engine/DoomLexer.h"
#include "src/engine/Engine.h"
#include "src/engine/EngineConfiguration.h"
#include "src/engine/State.h"
#include "src/engine/gui/Window.h"
#include "src/engine/renderer/Animation.h"
#include "src/engine/renderer/Light.h"
#include "src/engine/renderer/ModelManager.h"
#include "src/engine/renderer/Renderer.h"
#include "Actor.h"
#include "D3Map.h"
#include "Static.h"
#include "Scene.h"

//#include "Q3BSP.h"

namespace energonsoftware {

Logger& Scene::logger(Logger::instance("gled.engine.scene.Scene"));

Scene::Scene()
    : _loaded(false)
{
    // TODO: split the sizes of the pools into two config options
    const EngineConfiguration& config(EngineConfiguration::instance());
    _allocator = MemoryAllocator::new_allocator(MemoryAllocator::AllocatorTypeStack, config.memory_pool() * 1024 * 1024);
}

Scene::~Scene() throw()
{
    unload();
}

bool Scene::load(const boost::filesystem::path& filename)
{
    unload();

    LOG_INFO("Loading scene from '" << filename << "'\n");

    DoomLexer lexer;
    if(!lexer.load(filename)) {
        return false;
    }

    callback(0.0, "Loading map...");
    if(!scan_map(lexer)) {
        return false;
    }

/*callback(10.0, "Loading Q3BSP...");
_bsp.reset(new(16, *_allocator) Q3BSP("sq3t2"), boost::bind(&Q3BSP::destroy, _1, _allocator.get()));
if(!_bsp->load("")) {
    LOG_ERROR("Error loading Q3BSP!\n");
}

if(!_bsp->load_material(material_dir(), "scene")) {
    LOG_ERROR("Error loading Q3BSP material!\n");
}*/

    if(!scan_global_ambient_color(lexer)) {
        return false;
    }

    callback(50.0, "Loading models...");
    if(!scan_models(lexer)) {
        return false;
    }

    callback(75.0, "Loading actors...");
    if(!scan_renderables(lexer)) {
        return false;
    }

    callback(90.0, "Loading lights...");
    if(!scan_lights(lexer)) {
        return false;
    }

    _loaded = true;
    return true;
}

void Scene::unload()
{
    _loaded = false;

//_bsp.reset();
    _map.reset();

    _physicals.clear();
    _renderables.clear();
    _windows.clear();

    _allocator->reset();
}

bool Scene::renderables_ready() const
{
    BOOST_FOREACH(boost::shared_ptr<Renderable> renderable, _renderables) {
        if(!renderable->ready()) {
            return false;
        }
    }
    return true;
}

void Scene::init_renderables()
{
    BOOST_FOREACH(boost::shared_ptr<Renderable> renderable, _renderables) {
        renderable->init_textures();
    }
}

void Scene::update()
{
    _camera.simulate();
    BOOST_FOREACH(boost::shared_ptr<Physical> physical, _physicals) {
        physical->simulate();
    }
}

void Scene::create_scene_graph()
{
    _visible_renderables.clear();
    _pickable_renderables.clear();
_lit_renderables.clear();

    BOOST_FOREACH(boost::shared_ptr<Renderable> renderable, _renderables) {
        if(_camera.visible(renderable->absolute_bounds())) {
            // TODO: add transparent renderables to a separate list
            if(renderable->is_transparent()) {
                LOG_ERROR("TODO: Transparent renderables not supported!\n");
                continue;
            }

            _visible_renderables.push_back(renderable);

            boost::shared_ptr<Pickable> pickable(boost::dynamic_pointer_cast<Pickable, Renderable>(renderable));
            if(pickable) {
                _pickable_renderables.push_back(renderable);
            }
        }

        // TODO: find all of the lights that intersect this and add it to the per-light renderables
_lit_renderables.push_back(renderable);
    }

    // sort the renderables for "efficient" rendering (lol)
    std::sort(_visible_renderables.begin(), _visible_renderables.end(), CompareRenderablesOpaque(_camera.position()));
}

void Scene::render_geometry()
{
    if(!_loaded) {
        Engine::instance().renderer().render(_camera);
        return;
    }

    // TODO: we should call map->render() here and let it decide which actors to register
    BOOST_FOREACH(boost::shared_ptr<Renderable> renderable, _renderables) {
        if(!renderable->is_static()) {
            boost::dynamic_pointer_cast<Actor, Renderable>(renderable)->animate();
        }
        Engine::instance().renderer().register_renderable(_camera, renderable);
    }

    // TODO: same for the lights
    /*if(Engine::instance().state().render_lights()) {
        BOOST_FOREACH(boost::shared_ptr<Light> light, _map->lights()) {
            if(light->enabled()) {
                Engine::instance().renderer().register_renderable(_camera, light);
            }
        }
    }*/

    // TODO: and obviously this would change as well
    Engine::instance().renderer().render(_camera, *_map);
}

void Scene::render_2d()
{
    glDisable(GL_DEPTH_TEST);

    Renderer& renderer(Engine::instance().renderer());
    renderer.push_mvp_matrix();
    renderer.mvp_identity();
    renderer.orthographic(0.0f, renderer.viewport_width(), 0.0f, renderer.viewport_height());

    BOOST_FOREACH(boost::shared_ptr<Window> window, _windows) {
        window->render();
    }

    const State& state(Engine::instance().state());
    renderer.render_text(state.font(), Position(10.0f, 10.0f), state.display_text());

    renderer.pop_mvp_matrix();

    glEnable(GL_DEPTH_TEST);
}

void Scene::callback(float percent, const std::string& status)
{
    if(!_callback) {
        return;
    }
    _callback(percent, status);
}

bool Scene::scan_map(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::MAP)) {
        return false;
    }

    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    _map.reset(new(16, *_allocator) D3Map(name), boost::bind(&D3Map::destroy, _1, _allocator.get()));
    if(!_map->load("")) {
        return false;
    }

    if(!_map->load_material("scene")) {
        return false;
    }

    return true;
}

bool Scene::scan_global_ambient_color(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::GLOBAL_AMBIENT_COLOR)) {
        return false;
    }

    float r;
    if(!lexer.float_literal(r)) {
        return false;
    }

    float g;
    if(!lexer.float_literal(g)) {
        return false;
    }

    float b;
    if(!lexer.float_literal(b)) {
        return false;
    }

    float a;
    if(!lexer.float_literal(a)) {
        return false;
    }

    Color global_ambient_color(r, g, b, a);
    LOG_DEBUG("Setting global ambient color to " << global_ambient_color.str() << "\n");
    Light::global_ambient_color(global_ambient_color);

    return true;
}

bool Scene::scan_models(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::MODELS)) {
        return false;
    }

    if(!lexer.match(Lexer::OPEN_BRACE)) {
        return false;
    }

    while(!lexer.check_token(Lexer::CLOSE_BRACE)) {
        std::string path;
        if(!lexer.string_literal(path)) {
            return false;
        }

        std::string name;
        if(!lexer.string_literal(name)) {
            return false;
        }

        if(!Engine::instance().model_manager().load_model(path, name)) {
            return false;
        }

        size_t acount;
        if(!lexer.size_literal(acount)) {
            return false;
        }

        for(size_t i=0; i<acount; ++i) {
            std::string anim;
            if(!lexer.string_literal(anim)) {
                return false;
            }

            if(!Engine::instance().model_manager().load_animation(path, name, anim)) {
                return false;
            }
        }
    }

    if(!lexer.match(Lexer::CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool Scene::scan_renderables(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::RENDERABLES)) {
        return false;
    }

    if(!lexer.match(Lexer::OPEN_BRACE)) {
        return false;
    }

    while(!lexer.check_token(Lexer::CLOSE_BRACE)) {
        std::string type;
        if(!lexer.string_literal(type)) {
            return false;
        }

        if("static" == type) {
            if(!scan_static(lexer)) {
                return false;
            }
        } else {
            if(!scan_actor(lexer, type)) {
                return false;
            }
        }
    }

    if(!lexer.match(Lexer::CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool Scene::scan_static(Lexer& lexer)
{
    std::string model;
    if(!lexer.string_literal(model)) {
        return false;
    }

    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    boost::shared_ptr<Static> renderable(new(16, *_allocator) Static(name), boost::bind(&Static::destroy, _1, _allocator.get()));
    renderable->model(Engine::instance().model_manager().model(model));
    if(!renderable->has_model()) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }

    renderable->position(Position(x, y, z));

    if(!renderable->load_material()) {
        return false;
    }

    _renderables.push_back(renderable);
    return true;
}

bool Scene::scan_actor(Lexer& lexer, const std::string& type)
{
    std::string model;
    if(!lexer.string_literal(model)) {
        return false;
    }

    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    boost::shared_ptr<Actor> actor(Actor::new_actor(type, name));
    if(!actor) {
        return false;
    }

    actor->model(Engine::instance().model_manager().model(model));
    if(!actor->has_model()) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }

    actor->position(Position(x, y, z));

    std::string anim;
    if(!lexer.string_literal(anim)) {
        return false;
    }

    boost::shared_ptr<Animation> animation(Engine::instance().model_manager().animation(model, anim));
    if(!animation) {
        return false;
    }
    actor->animation(animation);

    size_t cframe;
    if(!lexer.size_literal(cframe)) {
        return false;
    }
    actor->current_frame(cframe);

    if(!actor->load_material()) {
        return false;
    }

    LOG_INFO("Pick id=" << actor->pick_id() << ", color=" << actor->pick_color().str() << "\n");
    _physicals.push_back(actor);
    _renderables.push_back(actor);
    return true;
}

bool Scene::scan_lights(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::LIGHTS)) {
        return false;
    }

    if(!lexer.match(Lexer::OPEN_BRACE)) {
        return false;
    }

    while(!lexer.check_token(Lexer::CLOSE_BRACE)) {
        std::string type;
        if(!lexer.string_literal(type)) {
            return false;
        }

        float x;
        if(!lexer.float_literal(x)) {
            return false;
        }

        float y;
        if(!lexer.float_literal(y)) {
            return false;
        }

        float z;
        if(!lexer.float_literal(z)) {
            return false;
        }

        std::string colordef;
        if(!lexer.string_literal(colordef)) {
            return false;
        }

        boost::shared_ptr<Light> light;
        if(type == "directional") {
            boost::shared_ptr<DirectionalLight> directional(new(16, *_allocator) DirectionalLight(),
                boost::bind(&DirectionalLight::destroy, _1, _allocator.get()));
            directional->direction(Direction(x, y, z));

            light = directional;
        } else if(type == "positional") {
            boost::shared_ptr<PositionalLight> positional(new(16, *_allocator) PositionalLight(),
                boost::bind(&PositionalLight::destroy, _1, _allocator.get()));
            positional->position(Position(x, y, z));

            if(!scan_light_positional(lexer, positional)) {
                return false;
            }
            light = positional;
        } else if(type == "spot") {
            boost::shared_ptr<SpotLight> spot(new(16, *_allocator) SpotLight(),
                boost::bind(&SpotLight::destroy, _1, _allocator.get()));
            spot->position(Position(x, y, z));

            if(!scan_light_spotlight(lexer, spot)) {
                return false;
            }
            light = spot;
        } else {
            LOG_ERROR("Invalid light type '" << type << "'\n");
            return false;
        }

        if(!light->load_colordef(colordef)) {
            return false;
        }

        light->enable();
        _map->add_light(light);
    }

    if(_map->light_count() > Map::MAX_LIGHTS) {
        LOG_CRITICAL("Only " << Map::MAX_LIGHTS << " lights allowed per-scene!\n");
        return false;
    }

    // fill in any remaining lights and disable them
    for(size_t i=_map->light_count(); i<Map::MAX_LIGHTS; ++i) {
        boost::shared_ptr<Light> light(new(16, *_allocator) DirectionalLight(),
            boost::bind(&DirectionalLight::destroy, _1, _allocator.get()));
        light->enable(false);
        _map->add_light(light);
    }

    return true;
}

bool Scene::scan_light_positional(Lexer& lexer, boost::shared_ptr<PositionalLight> positional)
{
    float constant_atten;
    if(!lexer.float_literal(constant_atten)) {
        return false;
    }
    positional->constant_attenuation(constant_atten);

    float linear_atten;
    if(!lexer.float_literal(linear_atten)) {
        return false;
    }
    positional->linear_attenuation(linear_atten);

    float quadratic_atten;
    if(!lexer.float_literal(quadratic_atten)) {
        return false;
    }
    positional->quadratic_attenuation(quadratic_atten);

    return true;
}

bool Scene::scan_light_spotlight(Lexer& lexer, boost::shared_ptr<SpotLight> spot)
{
    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }
    spot->direction(Direction(x, y, z));

    float cutoff;
    if(!lexer.float_literal(cutoff)) {
        return false;
    }
    spot->cutoff(cutoff);

    float exponent;
    if(!lexer.float_literal(exponent)) {
        return false;
    }
    spot->exponent(exponent);

    return scan_light_positional(lexer, spot);
}

}
