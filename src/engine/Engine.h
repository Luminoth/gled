#if !defined __ENGINE_H__
#define __ENGINE_H__

namespace energonsoftware {

class Configuration;
class InputState;
class ModelManager;
class Renderer;
class ResourceManager;
class State;
class UpdateThread;

class Engine
{
public:
    static Engine& instance();

private:
    static Logger& logger;

public:
    virtual ~Engine() throw();

public:
    // NOTE: the window needs to be created before calling this
    bool init(int video_width, int video_height, int video_depth);
    void shutdown();

    void print_memory_details();

    void start_frame();
    void finish_frame(bool rate_limit=true);

    bool ready() const { return _ready; }

    void quit() { _quit = true; }
    bool should_quit() const { return _quit; }

public:
    MemoryAllocator& system_allocator() { return *_system_allocator; }
    MemoryAllocator& frame_allocator() { return *_frame_allocator; }

const State& state() const { return *_state; }
State& state() { return *_state; }

    const InputState& input_state() const { return *_input_state; }
    InputState& input_state() { return *_input_state; }

    const ResourceManager& resource_manager() const { return *_resource_manager; }
    ResourceManager& resource_manager() { return *_resource_manager; }

    const ModelManager& model_manager() const { return *_model_manager; }
    ModelManager& model_manager() { return *_model_manager; }

    const Renderer& renderer() const { return *_renderer; }
    Renderer& renderer() { return *_renderer; }

public:
    double runtime() const;
    double frame_time() const;

    double average_fps() const;
    double current_fps() const;

private:
    void rate_limit();

private:
    boost::shared_ptr<MemoryAllocator> _system_allocator, _frame_allocator;
    boost::shared_ptr<UpdateThread> _update_thread;
boost::shared_ptr<State> _state;
    boost::shared_ptr<InputState> _input_state;
    boost::shared_ptr<ResourceManager> _resource_manager;
boost::shared_ptr<ModelManager> _model_manager;
    boost::shared_ptr<Renderer> _renderer;

    bool _ready, _quit;

    // this is set when init() is called
    // to count the complete runtime
    double _start_time;

    uint64_t _frame_count;
    double _last_frame;     // set after each call to render() for fps timing

private:
    Engine();
    DISALLOW_COPY_AND_ASSIGN(Engine);
};

}

#endif
