#if !defined __COMMON_H__
#define __COMMON_H__

#include "src/core/math/Vector.h"

namespace energonsoftware {

// TODO: move this out of core, it doens't belong here

// sleep time in microseconds
inline int thread_sleep_time()
{
#if defined _DEBUG
    return 1000;
#else
    return 100;
#endif
}

#if defined WIN32
    /* TODO: move these to the project or something */
    #define INSTALLDIR ""
    #define BINDIR "bin"
    #define CONFDIR "etc\\gled"
    #define DATADIR "share\\gled"
#endif

inline std::string window_title()
{
    return "GLED";
}

inline boost::filesystem::path install_dir()
{
    return INSTALLDIR;
}

inline boost::filesystem::path bin_dir()
{
    return install_dir() / BINDIR;
}

inline boost::filesystem::path conf_dir()
{
    return install_dir() / CONFDIR;
}

boost::filesystem::path home_conf_dir();

inline boost::filesystem::path game_conf()
{
    return home_conf_dir() / "gled.conf";
}

inline boost::filesystem::path editor_conf()
{
    return home_conf_dir() / "gled-editor.conf";
}

inline boost::filesystem::path data_dir()
{
    return install_dir() / DATADIR;
}

inline boost::filesystem::path font_dir()
{
    return data_dir() / "fonts";
}

inline boost::filesystem::path material_dir()
{
    return data_dir() / "materials";
}

inline boost::filesystem::path animation_dir()
{
    return data_dir() / "animations";
}

inline boost::filesystem::path model_dir()
{
    return data_dir() / "models";
}

inline boost::filesystem::path shader_dir()
{
    return data_dir() / "shaders";
}

inline boost::filesystem::path sound_dir()
{
    return data_dir() / "sounds";
}

inline boost::filesystem::path texture_dir_no_data()
{
    return "textures";
}

inline boost::filesystem::path texture_dir()
{
    return data_dir() / texture_dir_no_data();
}

inline boost::filesystem::path light_dir()
{
    return data_dir() / "lights";
}

inline boost::filesystem::path map_dir()
{
    return data_dir() / "maps";
}

inline boost::filesystem::path scene_dir()
{
    return data_dir() / "scenes";
}

Vector3 swizzle(const Vector3& v);

}

#endif
