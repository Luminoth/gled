#include "src/pch.h"
#include "src/core/util/util.h"
#include "ResourceManager.h"
#include "State.h"
#include "audio/Audio.h"
#include "gui/Window.h"
#include "renderer/Camera.h"
#include "renderer/ModelManager.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"
#include "ui/InputState.h"
#include "ui/UIController.h"
#include "EngineConfiguration.h"
#include "UpdateThread.h"
#include "Engine.h"

namespace energonsoftware {

Logger& Engine::logger(Logger::instance("gled.engine.Engine"));

Engine& Engine::instance()
{
    static boost::shared_ptr<Engine> engine;
    if(!engine) {
        // NOTE: this doesn't go on an allocator
        engine.reset(new Engine());
    }
    return *engine;
}

Engine::Engine()
    : _ready(false), _quit(false),
        _start_time(0.0), _frame_count(0), _last_frame(0.0)
{
}

Engine::~Engine() throw()
{
}

bool Engine::init(int video_width, int video_height, int video_depth)
{
    std::srand(std::time(NULL));

    if(ready()) {
        shutdown();
    }

    const EngineConfiguration& config(EngineConfiguration::instance());

    // TODO: the allocators shouldn't be dynamically allocated like this!
    // TODO: split the sizes of the pools into two config options
    LOG_INFO("Allocating memory pool (system=" << config.memory_pool() << "MB, frame=" << config.memory_pool() << "MB)...\n");
    _system_allocator = MemoryAllocator::new_allocator(MemoryAllocator::AllocatorTypeStack, config.memory_pool() * 1024 * 1024);
    _frame_allocator = MemoryAllocator::new_allocator(MemoryAllocator::AllocatorTypeStack, config.memory_pool() * 1024 * 1024);

    _update_thread.reset(new(*_system_allocator) UpdateThread(), boost::bind(&UpdateThread::destroy, _1, _system_allocator.get()));

    _renderer.reset(new(16, *_system_allocator) Renderer(), boost::bind(&Renderer::destroy, _1, _system_allocator.get()));
    if(!_renderer->init(video_width, video_height, video_depth)) {
        return false;
    }

_state.reset(new(*_system_allocator) State(), boost::bind(&State::destroy, _1, _system_allocator.get()));
    _input_state.reset(new(16, *_system_allocator) InputState(), boost::bind(&InputState::destroy, _1, _system_allocator.get()));

    LOG_INFO("Initializing engine...\n");

    _resource_manager.reset(new(*_system_allocator) ResourceManager(), boost::bind(&ResourceManager::destroy, _1, _system_allocator.get()));
    if(!_resource_manager->init()) {
        return false;
    }
    _resource_manager->dump();

    if(!Window::init()) {
        return false;
    }

    _model_manager.reset(new(*_system_allocator) ModelManager(), boost::bind(&ModelManager::destroy, _1, _system_allocator.get()));

if(!_state->load_font("courier", 24, Color(1.0f, 1.0f, 1.0f, 1.0f))) {
    return false;
}

    if(!Audio::init()) {
        LOG_CRITICAL("Unable to initialize audio!\n");
        return false;
    }

    print_memory_details();

    _update_thread->start();
    LOG_INFO(_update_thread->str() << "\n");

    _start_time = _last_frame = get_time();

    _ready = true;
    return true;
}

void Engine::shutdown()
{
    _ready = false;
    _update_thread.reset();

    LOG_INFO("Runtime statistics:\n"
        << "Frames Rendered: " << _frame_count << "\n"
        << "Runtime: " << runtime() << "s\n"
        << "Average FPS: " << average_fps() << "\n");

    Audio::shutdown();

    _resource_manager->shutdown();

    UIController::release_controllers();
}

void Engine::print_memory_details()
{
    LOG_INFO("System Allocator Details:\n");
    LOG_INFO("    Total Allocated: " << (_system_allocator->total() / 1024.0f / 1024.0f) << "MB\n");
    LOG_INFO("    Used: " << (_system_allocator->used() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_system_allocator->used()) / static_cast<float>(_system_allocator->total()))) << "%)\n");
    LOG_INFO("    Available: " << (_system_allocator->unused() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_system_allocator->unused()) / static_cast<float>(_system_allocator->total()))) << "%)\n");
    LOG_INFO("    Allocation Count: " << _system_allocator->allocation_count() << "\n");
    LOG_INFO("    Bytes Allocated: " << _system_allocator->allocation_bytes() << "\n");

    LOG_INFO("Scene Allocator Details:\n");
    LOG_INFO("    Total Allocated: " << (_state->scene().allocator().total() / 1024.0f / 1024.0f) << "MB\n");
    LOG_INFO("    Used: " << (_state->scene().allocator().used() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_state->scene().allocator().used()) / static_cast<float>(_state->scene().allocator().total()))) << "%)\n");
    LOG_INFO("    Available: " << (_state->scene().allocator().unused() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_state->scene().allocator().unused()) / static_cast<float>(_state->scene().allocator().total()))) << "%)\n");
    LOG_INFO("    Allocation Count: " << _state->scene().allocator().allocation_count() << "\n");
    LOG_INFO("    Bytes Allocated: " << _state->scene().allocator().allocation_bytes() << "\n");

    LOG_INFO("Frame Allocator Details:\n");
    LOG_INFO("    Total Allocated: " << (_frame_allocator->total() / 1024.0f / 1024.0f) << "MB\n");
    LOG_INFO("    Used: " << (_frame_allocator->used() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_frame_allocator->used()) / static_cast<float>(_frame_allocator->total()))) << "%)\n");
    LOG_INFO("    Available: " << (_frame_allocator->unused() / 1024.0f / 1024.0f) << "MB"
        << " (" << (100.0f * (static_cast<float>(_frame_allocator->unused()) / static_cast<float>(_frame_allocator->total()))) << "%)\n");
    LOG_INFO("    Allocation Count: " << _frame_allocator->allocation_count() << "\n");
    LOG_INFO("    Bytes Allocated: " << _frame_allocator->allocation_bytes() << "\n");

    _renderer->print_video_memory_details();
}

void Engine::start_frame()
{
//    _state->scene().render();

    std::stringstream txt;
    txt << "Current FPS: " << current_fps() << ", Average FPS: " << average_fps();
    _state->display_text(txt.str());

//    _renderer->run();

    // check for any untrapped exceptions
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {
        LOG_ERROR("Untrapped OpenGL error: " << gluErrorString(error) << "\n");
    }

    //glFlush();

    _last_frame = get_time();
}

void Engine::finish_frame(bool rate_limit)
{
    if(rate_limit) {
        this->rate_limit();
    }

    _frame_allocator->reset();
    _frame_count++;
}

double Engine::runtime() const
{
    return get_time() - _start_time;
}

double Engine::frame_time() const
{
    return get_time() - _last_frame;
}

double Engine::average_fps() const
{
    return _frame_count / runtime();
}

double Engine::current_fps() const
{
    return 1.0 / frame_time();
}

void Engine::rate_limit()
{
    const EngineConfiguration& config(EngineConfiguration::instance());
    if(config.video_maxfps() <= 0) {
        return;
    }

    const double target_framespan = 1.0 / config.video_maxfps();
    const double frame_time = this->frame_time();

    // TODO: if config.video_sync() is true,
    // we should sync to the refresh rather than the framespan
    if(frame_time < target_framespan) {
        double sleep_time = (target_framespan - frame_time) * 1000000.0;
        usleep(static_cast<int>(sleep_time));
    }
}

}
