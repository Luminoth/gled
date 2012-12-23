#if !defined __STATE_H__
#define __STATE_H__

#include "renderer/Font.h"
#include "scene/Scene.h"

namespace energonsoftware {

// TODO: replace with a configuration class
class State
{
public:
    static void destroy(State* const state, MemoryAllocator* const allocator);

private:
    static Logger& logger;

public:
    virtual ~State() throw();

public:
bool load_font(const std::string& name, size_t height, const Color& color);

const std::string& display_text() const { return _display_text; }
void display_text(const std::string& text) { _display_text = text; }
void append_display_text(const std::string& text) { _display_text += text; }

public:
    Scene& scene() { return _scene; }
    const Scene& scene() const { return _scene; }

const Font& font() const { return _font; }

private:
    Scene _scene;

Font _font;
std::string _display_text;

private:
    friend class Engine;

private:
    State();
    DISALLOW_COPY_AND_ASSIGN(State);
};

}

#endif
