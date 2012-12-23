#if !defined __EDITORCONFIGURATION_H__
#define __EDITORCONFIGURATION_H__

#include "src/engine/EngineConfiguration.h"

class EditorConfiguration : public energonsoftware::EngineConfiguration
{
public:
    static EditorConfiguration& instance();

public:
    virtual ~EditorConfiguration() throw();

public:
    bool load_config();
    bool save_config();
    bool config_exists() const;

public:
    void editor_x(int x) { set("editor", "x", energonsoftware::to_string(x)); }
    int editor_x() const { return std::atoi(get("editor", "x").c_str()); }
    void editor_y(int y) { set("editor", "y", energonsoftware::to_string(y)); }
    int editor_y() const { return std::atoi(get("editor", "y").c_str()); }
    void editor_width(int width) { set("editor", "width", energonsoftware::to_string(width)); }
    int editor_width() const { return std::atoi(get("editor", "width").c_str()); }
    void editor_height(int height) { set("editor", "height", energonsoftware::to_string(height)); }
    int editor_height() const { return std::atoi(get("editor", "height").c_str()); }

    int video_depth() const { return std::atoi(get("video", "depth").c_str()); }

    float game_fov() const { return std::atof(get("game", "fov").c_str()); }

    float input_sensitivity() const { return std::atof(get("input", "sensitivity").c_str()); }

public:
    virtual void validate() const throw(energonsoftware::ConfigurationError);

private:
    virtual void on_save();

private:
    EditorConfiguration();
    DISALLOW_COPY_AND_ASSIGN(EditorConfiguration);
};

#endif
