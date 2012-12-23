#! /usr/bin/env python

import os
import sys

EnsureSConsVersion(2, 1)

# checks pkg-config version
def CheckPKGConfig(context, version):
    context.Message( "Checking for pkg-config version >= %s..." % version)
    ret = context.TryAction("pkg-config --atleast-pkgconfig-version=%s" % version)[0]
    context.Result(ret)
    return ret

# checks for a package using pkg-config
def CheckPKG(context, name):
     context.Message("Checking for package %s..." % name)
     ret = context.TryAction("pkg-config --exists \'%s\'" % name)[0]
     context.Result(ret)
     return ret

# checks boost version
def CheckBoost(context, version):
    # boost versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
        version_n += int(v_arr[0])*100000
    if len(v_arr) > 1:
        version_n += int(v_arr[1])*100
    if len(v_arr) > 2:
        version_n += int(v_arr[2])

    context.Message("Checking for Boost version >= %s..." % version)
    ret = context.TryCompile(
"""
#include <boost/version.hpp>

#if BOOST_VERSION < %d
#error Installed boost is too old!
#endif
int main()
{
    return 0;
}

""" % version_n, ".cc")
    context.Result(ret)
    return ret

# check google protocol buffers version
def CheckProtoBuf(context, version):
    # protobuf versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
        version_n += int(v_arr[0])*1000000
    if len(v_arr) > 1:
        version_n += int(v_arr[1])*1000
    if len(v_arr) > 2:
        version_n += int(v_arr[2])

    context.Message("Checking for Google Protocol Buffers version >= %s..." % version)
    ret = context.TryCompile(
"""
#include <google/protobuf/descriptor.h>

#if GOOGLE_PROTOBUF_VERSION < %d
#error Installed protobuf is too old!
#endif
int main()
{
    return 0;
}

""" % version_n, ".cc")
    context.Result(ret)
    return ret

# check wxWidgets version
def CheckWxWidgets(context, major, minor, release):
    context.Message("Checking for wxWidgets version >= %d.%d.%d..." % (major, minor, release))
    ret = context.TryRun(
"""
#include <wx/wx.h>

int main()
{
    if(!wxCHECK_VERSION(%d, %d, %d)) {
        return 1;
    }
    return 0;
}
""" % (major, minor, release), ".cc")
    context.Result(ret[0])
    return ret

# checks SQLite version
def CheckSQLite(context, version):
    # sqlite versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
        version_n += int(v_arr[0])*1000000
    if len(v_arr) > 1:
        version_n += int(v_arr[1])*1000
    if len(v_arr) > 2:
        version_n += int(v_arr[2])

    context.Message("Checking for SQLite version >= %s..." % version)
    ret = context.TryCompile(
"""
#include <sqlite3.h>

#if SQLITE_VERSION_NUMBER < %d
#error Installed SQLite is too old!
#endif
int main()
{
    return 0;
}

""" % version_n, ".cc")
    context.Result(ret)
    return ret

### BASE DIRECTORIES ###
src_dir = "src"
base_build_dir = "build"
build_dir = base_build_dir
lib_dir = "lib"
bin_dir = "bin"
conf_dir = os.path.join("etc", "gled")
data_dir = os.path.join("share", "gled")

### BASE COMPILER OPTIONS ###
ccflags = [
    "-std=c++11",
    "-msse3",
    "-Wall",
    "-Wextra",
    "-Woverloaded-virtual",
    "-Wno-unused-parameter",
    "-Wnon-virtual-dtor",
    "-Wno-missing-field-initializers",
    "-Wno-unknown-pragmas",
    "-pipe",
    "-fno-strict-aliasing",
#    "-Wold-style-cast",
#    "-Weffc++",
#    "-Wshadow",
]
ccpath = [
    os.getcwd(),
    "/opt/local/include",       # osx darwin ports
]
ccdefs = [ "USE_SSE" ]
ldflags = []
ldpath = [
    os.path.join(os.getcwd(), lib_dir),
    "/opt/local/lib",           # osx darwin ports
]
libs = []

### TARGETS ###
core_lib = "gled-core"
engine_lib = "gled-engine"

game_app = "gled"
editor_app = "gled-editor"
tests_app = "test"

### META TARGETS ###
build_meta_targets = []

if int(ARGUMENTS.get("ctags", 0)):
    build_meta_targets.append("ctags")
elif int(ARGUMENTS.get("coreonly", 0)):
    pass
elif int(ARGUMENTS.get("testcheckin", 0)):
    build_meta_targets.append("unittest")
    build_meta_targets.append("runtests")
else:
    if int(ARGUMENTS.get("install", 0)):
        build_meta_targets.append("install")
    if int(ARGUMENTS.get("unittest", 0)):
        build_meta_targets.append("unittest")

### ARGUMENTS ###
install_dir = ARGUMENTS.get("installdir", "")
print("Using install dir: %s" % install_dir)
ccdefs.append("INSTALLDIR=\\\"%s\\\"" % install_dir)
ccdefs.append("BINDIR=\\\"%s\\\"" % bin_dir)
ccdefs.append("CONFDIR=\\\"%s\\\"" % conf_dir)
ccdefs.append("DATADIR=\\\"%s\\\"" % data_dir)

if "unittest" in build_meta_targets:
    #ccflags.extend([ "-fprofile-arcs", "-ftest-coverage" ])
    ccdefs.append("WITH_UNIT_TESTS")
    ldflags.append("-ldl")
    #libs.append("gcov")
    build_dir = os.path.join(build_dir, "test")

    core_lib += "-test"
    engine_lib += "-test"

profile = int(ARGUMENTS.get("profile", 0))
if profile:
    ccflags.extend([ "-pg" ]) #, "-fprofile-arcs" ])
    ccdefs.append("WITH_PROFILE")
    ldflags.append("-pg")
    #libs.append("gcov")
    build_dir = os.path.join(build_dir, "profile")

    core_lib += "-profile"
    engine_lib += "-profile"

    game_app += "-profile"
    editor_app += "-profile"
    tests_app += "-profile"

efence = int(ARGUMENTS.get("efence", 0))

if int(ARGUMENTS.get("release", 0)):
    ccflags.extend([
        "-O3",
        "-finline-functions",
        "-ffast-math",
        "-fno-unsafe-math-optimizations",
        "-fno-common",
        "-funroll-loops",
        "-fstack-protector-all",
        "-fstack-check",
        "-Wstack-protector",
        #"-fomit-frame-pointer",
        #"-Winline",
    ])
    #ccdefs.append("_NDEBUG")
    ccdefs.append("NDEBUG")
    build_dir = os.path.join(build_dir, "release")
else:
    ccflags.extend([
        "-O0",
        "-g",
        "-fno-default-inline",
    ])

    if sys.platform == "darwin":
        ccflags.extend([
            "-gdwarf-2",
        ])
    else:
        ccflags.extend([
            "-gstabs+",
        ])

    #ccdefs.append("_DEBUG")
    ccdefs.append("DEBUG")
    build_dir = os.path.join(build_dir, "debug")

    # append -debug to our targets
    core_lib += "-debug"
    engine_lib += "-debug"

    game_app += "-debug"
    editor_app += "-debug"
    tests_app += "-debug"

print("Using variant directory: %s" % build_dir)

# checks for a C++ library and exists if it doesn't exist
def CheckLibOrExit(conf, lib, add=True):
    if not conf.CheckLib(lib, language="C++", autoadd=add):
        print("lib%s not found!" % lib)
        Exit(1)

# checks for the common libraries
def CheckCommonConfiguration(env, check_libs):
    check_libs = True   # bleh :(

    custom_tests = { "CheckPKGConfig": CheckPKGConfig, "CheckPKG": CheckPKG,
        "CheckBoost" : CheckBoost, "CheckWxWidgets": CheckWxWidgets }

    conf = Configure(env, custom_tests=custom_tests)
    if not conf.env.GetOption("clean"):
        #if not conf.CheckPKGConfig("0.21"):
        #    print("pkg-config version >= 0.21 required!")
        #    Exit(1)

        if not conf.CheckBoost("1.40"):
            print("Boost version >= 1.40 required!")
            Exit(1)

        if conf.CheckCHeader("valgrind/callgrind.h") or conf.CheckCHeader("callgrind.h"):
            conf.env.MergeFlags({ "CPPDEFINES": [ "-DHAS_CALLGRIND_H" ] })

        if check_libs:
            CheckLibOrExit(conf, "pthread")

            CheckLibOrExit(conf, "boost_system")
            CheckLibOrExit(conf, "boost_date_time")
            CheckLibOrExit(conf, "boost_filesystem")
            CheckLibOrExit(conf, "boost_regex")
            CheckLibOrExit(conf, "boost_thread")

            if sys.platform == "darwin":
                CheckLibOrExit(conf, "iconv")

            if "unittest" in build_meta_targets:
                CheckLibOrExit(conf, "cppunit")

            if efence:
                CheckLibOrExit(conf, "efence")

    return conf

# checks for engine libraries
def CheckEngineConfiguration(env, check_libs):
    #check_libs = True   # bleh :(

    conf = CheckCommonConfiguration(env, check_libs)
    if not conf.env.GetOption("clean"):
        if check_libs:
            CheckLibOrExit(conf, "GL")
            CheckLibOrExit(conf, "GLU")
            CheckLibOrExit(conf, "GLEW")
            CheckLibOrExit(conf, "png")
            CheckLibOrExit(conf, "openal")
            CheckLibOrExit(conf, "ogg")
            CheckLibOrExit(conf, "vorbis")
            CheckLibOrExit(conf, "vorbisfile")
            CheckLibOrExit(conf, "alut")

    return conf

# checks for game libraries
def CheckGameConfiguration(env, check_libs):
    #check_libs = True   # bleh :(

    conf = CheckEngineConfiguration(env, check_libs)
    if not conf.env.GetOption("clean"):
        if check_libs:
            pass

    return conf

# checks for editor libraries
def CheckEditorConfiguration(env, check_libs):
    #check_libs = True   # bleh :(

    conf = CheckEngineConfiguration(env, check_libs)
    if not conf.env.GetOption("clean"):
        if check_libs:
            if not conf.CheckWxWidgets(2, 8, 11):
                print("wxWidgets version >= 2.8.11 required!");
                Exit(1)

    return conf

def GenerateCommonEnv(app_name=""):
    env = Environment(CCFLAGS=ccflags, CPPPATH=ccpath, CPPDEFINES=ccdefs,
        LIBPATH=ldpath, LINKFLAGS=ldflags, LIBS=libs)

    if app_name:
        env.MergeFlags({ "CPPDEFINES": [ "-DAPPNAME=\\\"%s\\\"" % app_name ] })

    return env

def GenerateEngineEnv(include_libs, app_name=""):
    env = GenerateCommonEnv(app_name)

    if sys.platform == "linux" or sys.platform == "darwin":
        if sys.platform != "darwin":
            env.MergeFlags({ "CPPPATH": [ "/usr/X11R6/include" ] })

        if include_libs:
            env.MergeFlags({ "LIBPATH": [ "/usr/X11R6/lib" ] })

    if sys.platform == "darwin":
        env.MergeFlags({ "CPPPATH": [ 
            "/System/Library/Frameworks/AGL.framework/Versions/A/Headers",
            "/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers"
        ] })

        if include_libs:
            env.MergeFlags({ "LINKFLAGS": [ "-framework AGL", "-framework OpenGL" ]})

        env.MergeFlags({ "CPPPATH": [
            "/System/Library/Frameworks/OpenAL.framework/Versions/A/Headers"
        ] })

        if include_libs:
            env.MergeFlags({ "LINKFLAGS": [ "-framework OpenAL" ]})

    if not env.GetOption("clean"):
        ### Freetype2 ###
        env.ParseConfig("freetype-config --cflags")
        if include_libs: env.ParseConfig("freetype-config --libs")

    return env

def GenerateGameEnv(include_libs, app_name=""):
    env = GenerateEngineEnv(app_name)

    if not env.GetOption("clean"):
        ### SDL ###
        env.ParseConfig("sdl-config --cflags")
        if include_libs: env.ParseConfig("sdl-config --libs")

    return env

def GenerateEditorEnv(include_libs, app_name=""):
    env = GenerateEngineEnv(app_name)

    if not env.GetOption("clean"):
        ### wxWidgets ###
        env.ParseConfig("wx-config --cxxflags")
        env.ParseConfig("wx-config --libs")
        env.ParseConfig("wx-config --gl-libs")

    return env

def BuildCore():
    print("Building core library...")

    env = GenerateCommonEnv()
    conf = CheckCommonConfiguration(env, False)
    env = conf.Finish()

    obj = SConscript(os.path.join(src_dir, "SConscript"), exports=[ "env" ],
        variant_dir=build_dir, duplicate=0)
    obj.extend(SConscript(os.path.join(src_dir, "core", "SConscript"), exports=[ "env" ],
        variant_dir=os.path.join(build_dir, "core"), duplicate=0))
    lib = env.StaticLibrary(os.path.join(lib_dir, core_lib), obj)

    Clean(lib, base_build_dir)
    Clean(lib, lib_dir)

def BuildEngine():
    print("Building engine library...")

    env = GenerateEngineEnv(False)
    conf = CheckEngineConfiguration(env, False)
    env = conf.Finish()

    obj = SConscript(os.path.join(src_dir, "engine", "SConscript"), exports=[ "env" ],
        variant_dir=os.path.join(build_dir, "engine"), duplicate=0)
    lib = env.StaticLibrary(os.path.join(lib_dir, engine_lib), obj)

    Clean(lib, base_build_dir)
    Clean(lib, lib_dir)

def BuildGame():
    print("Building game...")

    env = GenerateGameEnv(True, game_app)
    conf = CheckGameConfiguration(env, True)
    env = conf.Finish()

    env.MergeFlags({ "LIBS": [ engine_lib, core_lib ] })

    obj = SConscript(os.path.join(src_dir, "game", "SConscript"), exports=[ "env" ],
        variant_dir=os.path.join(build_dir, "game"), duplicate=0)
    app = env.Program(os.path.join(bin_dir, game_app), obj)

    #if sys.platform == "darwin":
    #    basename = game_app.split("-")[0]
    #    Command(os.path.join("osx", "GLEd.app", "Contents", "MacOS", basename),
    #        os.path.join(bin_dir, game_app),
    #        Copy("$TARGET", "$SOURCE"))
    #    Clean(app, os.path.join("osx", "GLEd.app", "Contents", "MacOS", basename))

    Clean(app, base_build_dir)
    Clean(app, bin_dir)

    if "install" in build_meta_targets:
        env.Install(os.path.join(install_dir, os.path.join(bin_dir, game_app)))
        env.Install(os.path.join(install_dir, os.path.join(conf_dir, "gled.conf")))

def BuildEditor():
    print("Building editor...")

    env = GenerateEditorEnv(True, editor_app)
    conf = CheckEditorConfiguration(env, True)
    env = conf.Finish()

    env.MergeFlags({ "LIBS": [ engine_lib, core_lib ] })

    obj = SConscript(os.path.join(src_dir, "editor", "SConscript"), exports=[ "env" ],
        variant_dir=os.path.join(build_dir, "editor"), duplicate=0)
    app = env.Program(os.path.join(bin_dir, editor_app), obj)

    #if sys.platform == "darwin":
    #    basename = editor_app.split("-")[0]
    #    Command(os.path.join("osx", "GLEd.app", "Contents", "MacOS", basename),
    #        os.path.join(bin_dir, editor_app),
    #        Copy("$TARGET", "$SOURCE"))
    #    Clean(app, os.path.join("osx", "GLEd.app", "Contents", "MacOS", basename))

    Clean(app, base_build_dir)
    Clean(app, bin_dir)

    if "install" in build_meta_targets:
        env.Install(os.path.join(install_dir, os.path.join(bin_dir, editor_app)))
        env.Install(os.path.join(install_dir, os.path.join(conf_dir, "gled.conf")))

def BuildTests():
    print("Building tests...")

    env = GenerateEngineEnv(True, tests_app)
    conf = CheckEngineConfiguration(env, True)
    env = conf.Finish()

    # have to add libs to LINKFLAGS so we can tell the linker
    # to add all of the symbols (or we'll have no tests)
    if sys.platform == "darwin":
        # hey guess what, this doesn't work. thanks a lot apple
        env.MergeFlags({ "LINKFLAGS": [
            "-Wl,-all_load",
            "-l%s" % engine_lib,
            "-l%s" % core_lib,
            "-Wl,-noall_load"
        ] })
    elif "linux" in sys.platform:
        env.MergeFlags({ "LINKFLAGS": [
            "-Wl,-whole-archive",
            "-l%s" % engine_lib,
            "-l%s" % core_lib,
            "-Wl,-no-whole-archive"
        ] })
    else:
        env.MergeFlags({ "LIBS": [ engine_lib, core_lib ] })

    obj = SConscript(os.path.join(src_dir, "test", "SConscript"), exports=[ "env" ],
        variant_dir=os.path.join(build_dir, "test"), duplicate=0)
    app = env.Program(os.path.join(bin_dir, tests_app), obj)

    # have to add libraries as dependencies by hand on linux
    if sys.platform == "darwin" or "linux" in sys.platform:
        Depends(app, os.path.join(lib_dir, "lib%s.a" % engine_lib))
        Depends(app, os.path.join(lib_dir, "lib%s.a" % core_lib))

    Clean(app, base_build_dir)
    Clean(app, bin_dir)

    if "runtests" in build_meta_targets:
        #print("Running tests...")
        #from test.test import main as test_main
        #test_alias = Alias(tests_app, [ app ], test_main(["test/test.py", "-r", "-V"]))
        #AlwaysBuild(test_alias)

        #from test.test import main as test_main
        #print("Tests exited with status: %d" % test_main(["test/test.py", "-r", "-V"]))

        print("TODO: make this work")

# create th
if "ctags" in build_meta_targets:
    os.system("ctags -R")
    sys.exit(0)

# everything requires core
BuildCore()

if "unittest" in build_meta_targets:
    # build everything that's not an app
    BuildEngine()
    BuildTests()
else:
    BuildEngine()
    BuildGame()
    BuildEditor()
