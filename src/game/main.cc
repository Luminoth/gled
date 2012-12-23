#include "src/pch.h"
#include <iostream>
#include <signal.h>
#if !defined WIN32
    #include <getopt.h>
#endif
#include <SDL/SDL.h>
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/renderer/Renderer.h"
#include "EventHandler.h"
#include "GameConfiguration.h"
#include "GameUIController.h"
#include "Player.h"

#include "src/engine/State.h"
#include "src/engine/scene/Actor.h"
#include "src/engine/scene/Scene.h"
#include "src/engine/renderer/Camera.h"

boost::filesystem::path g_configfilename(energonsoftware::game_conf());
SDL_Surface* g_window = NULL;

void print_help()
{
    std::cerr << "Usage: gled [options]" << std::endl << std::endl
        << energonsoftware::window_title() << std::endl << std::endl
        << "Options:" << std::endl
        << "\t-h, --help        show this help message and exit" << std::endl;
}

bool parse_arguments(int argc, char* const argv[])
{
#if defined WIN32
    std::cerr << "Implement parse_arguments()!" << std::endl;
#else
    static struct option long_options[] =
    {
        { "help", 0, NULL, 'h' },
        { NULL, 0, NULL, 0 }
    };

    while(true) {
        int c = getopt_long(argc, argv, "h", long_options, NULL);
        if(c == -1) break;

        switch(c)
        {
        case 'h':
        case '?':
            print_help();
            return false;
        }
    }
#endif

    return true;
}

bool initialize_config_directory()
{
    if(!boost::filesystem::exists(energonsoftware::home_conf_dir())) {
        try {
            std::cout << "Creating home configuration directory " << energonsoftware::home_conf_dir() << std::endl;
            boost::filesystem::create_directory(energonsoftware::home_conf_dir());
        } catch(const boost::filesystem::filesystem_error& e) {
            std::cerr << "Could not create configuration directory: " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

bool initialize_configuration()
{
    std::cout << "Initializing configuration..." << std::endl;
    GameConfiguration& config(GameConfiguration::instance());

    std::cout << "Reading configuration from '" << g_configfilename << "'..." << std::endl;
    bool ret = config.load(g_configfilename);
    config.validate();

    return ret;
}

void save_configuration()
{
    std::cout << "Saving configuration to '" << g_configfilename << "'..." << std::endl;

    GameConfiguration& config(GameConfiguration::instance());
    config.save(g_configfilename);
}

bool initialize_logger()
{
    const GameConfiguration& config(GameConfiguration::instance());
    if(!energonsoftware::Logger::configure(config.logging_type(), config.logging_level(), config.logging_filename())) {
        return false;
    }

    energonsoftware::Logger& logger(energonsoftware::Logger::instance("gled.main"));
    LOG_INFO("Logger initialized!\n");

    return true;
}

void signal_handler(int signum)
{
    energonsoftware::Logger& logger(energonsoftware::Logger::instance("gled.main"));
    if(signum == SIGINT) {
        LOG_INFO("Caught SIGINT, quitting...\n");
        energonsoftware::Engine::instance().quit();
    }
}

void initialize_signal_handlers()
{
    energonsoftware::Logger& logger(energonsoftware::Logger::instance("gled.main"));
    LOG_INFO("Initializing signal handlers...\n");

    signal(SIGINT, signal_handler);
}

bool create_window(int width, int height, int bpp, bool fullscreen, const std::string& caption)
{
    energonsoftware::Logger& logger(energonsoftware::Logger::instance("gled.main"));

    //SDL_putenv("SDL_VIDEO_WINDOW_POS=1000,200");

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_CRITICAL("Could not initialize SDL: " << SDL_GetError() << "\n");
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, bpp);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // 2xFSAA (requires GL_ARB_multisample)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    Uint32 flags = SDL_ASYNCBLIT | SDL_ANYFORMAT | SDL_HWPALETTE | SDL_OPENGL;
    if(fullscreen) flags |= SDL_FULLSCREEN;

    g_window = SDL_SetVideoMode(width, height, bpp, flags);
    if(NULL == g_window) {
        LOG_CRITICAL("Unable to set video mode!\n");
        return false;
    }

    SDL_WM_SetCaption(caption.c_str(), NULL);

    return true;
}

int main(int argc, char* argv[])
{
    if(!parse_arguments(argc, argv)) {
        return 1;
    }

    if(!initialize_config_directory()) {
        return 1;
    }

    try {
        if(!initialize_configuration()) {
            std::cerr << "WARNING: Reading configuration failed!" << std::endl;
            save_configuration();
        }
    } catch(const energonsoftware::ConfigurationError& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return false;
    }
    const GameConfiguration& config(GameConfiguration::instance());

    if(!initialize_logger()) {
        std::cerr << "ERROR: Could not initialize logger!" << std::endl;
        return 1;
    }

    energonsoftware::Logger& logger(energonsoftware::Logger::instance("gled.main"));
    LOG_INFO("Current directory: " << boost::filesystem::initial_path() << "\n");
#if USE_SSE
    LOG_INFO("Using SSE\n");
#endif
    config.dump(logger);

    // initialize the signal handlers
    initialize_signal_handlers();

    // create the window
    if(!create_window(config.video_width(), config.video_height(), config.video_depth(), config.video_fullscreen(), energonsoftware::window_title())) {
        LOG_CRITICAL("Unable to create the window!\n");
        return 1;
    }

    // init the engine
    if(!energonsoftware::Engine::instance().init(config.video_width(), config.video_height(), config.video_depth())) {
        LOG_ERROR("Engine::init() failed!\n");
        energonsoftware::Engine::instance().shutdown();
        return 1;
    }
    energonsoftware::Engine::instance().renderer().resize_viewport(config.video_width(), config.video_height(), config.game_fov());

    // UI controller goes on the system allocator
    energonsoftware::MemoryAllocator& allocator(energonsoftware::Engine::instance().system_allocator());
    energonsoftware::UIController::controller(boost::shared_ptr<energonsoftware::UIController>(new(16, allocator) GameUIController(),
        boost::bind(GameUIController::destroy, _1, &allocator)));

    // load the scene
    if(!energonsoftware::Engine::instance().state().scene().load(energonsoftware::scene_dir() / "default.scene")) {
        LOG_ERROR("Error loading the scene!\n");
        energonsoftware::Engine::instance().shutdown();
        return 1;
    }

    // setup the player
    boost::shared_ptr<Player> player(Player::instance());
    energonsoftware::Engine::instance().state().scene().register_physical(player);
    energonsoftware::Engine::instance().state().scene().camera().attach(player);

    energonsoftware::Position position(energonsoftware::Engine::instance().state().scene().map().player_spawn_position());
    LOG_INFO("Moving player to " << position.str() << "\n");
    player->position(position);

    float angle(energonsoftware::Engine::instance().state().scene().map().player_spawn_angle());
    LOG_INFO("Rotating " << angle << " degrees\n");
    player->rotate(DEG_RAD(angle), energonsoftware::Vector3(0.0f, 1.0f, 0.0f));

    // setup the UI controller
    //energonsoftware::UIController::controller()->grab_input(true);
    //energonsoftware::UIController::controller()->show_mouse_cursor(false);
    energonsoftware::UIController::controller()->warp_mouse(
        energonsoftware::Position(energonsoftware::Engine::instance().renderer().half_viewport_size())
    );

    LOG_INFO("Entering event loop...\n");
    while(!energonsoftware::Engine::instance().should_quit()) {
        energonsoftware::UIController::controller()->warp_mouse(
            energonsoftware::Position(energonsoftware::Engine::instance().renderer().half_viewport_size())
        );
        if(!EventHandler::instance().run()) {
            energonsoftware::Engine::instance().quit();
            continue;
        }

        energonsoftware::Engine::instance().start_frame();
        energonsoftware::Engine::instance().finish_frame();
        SDL_GL_SwapBuffers();
    }

    energonsoftware::Engine::instance().shutdown();
    SDL_Quit();
    save_configuration();

    return 0;
}
