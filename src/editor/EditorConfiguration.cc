#include "src/pch.h"
#include "src/core/common.h"
#include "EditorConfiguration.h"

EditorConfiguration& EditorConfiguration::instance()
{
    static boost::shared_ptr<EditorConfiguration> configuration;
    if(!configuration) {
        configuration.reset(new EditorConfiguration());
    }
    return *configuration;
}

EditorConfiguration::EditorConfiguration()
    : energonsoftware::EngineConfiguration()
{
    set_default("editor", "x", "0");
    set_default("editor", "y", "0");
    set_default("editor", "width", "1280");
    set_default("editor", "height", "720");

    set_default("video", "depth", "32");

    set_default("game", "fov", "75.0");

    set_default("input", "sensitivity", "1.0");

    set_default("logging", "filename", (energonsoftware::home_conf_dir() / "gled-editor.log").string());

    load_defaults();

    generate_header("Editor configuration file");
}

EditorConfiguration::~EditorConfiguration() throw()
{
}

bool EditorConfiguration::load_config()
{
    return load(energonsoftware::editor_conf());
}

bool EditorConfiguration::save_config()
{
    return save(energonsoftware::editor_conf());
}

bool EditorConfiguration::config_exists() const
{
    return exists(energonsoftware::editor_conf());
}

void EditorConfiguration::validate() const throw(energonsoftware::ConfigurationError)
{
    energonsoftware::EngineConfiguration::validate();

    if(!energonsoftware::is_int(get("video", "depth"))) {
        throw energonsoftware::ConfigurationError("Video depth must be an integer");
    }

    if(!energonsoftware::is_double(get("game", "fov"))) {
        throw energonsoftware::ConfigurationError("Editor fov must be a float");
    }

    if(!energonsoftware::is_double(get("input", "sensitivity"))) {
        throw energonsoftware::ConfigurationError("Input sensitivity must be a float");
    }
}

void EditorConfiguration::on_save()
{
}
