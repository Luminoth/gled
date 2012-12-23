#if !defined __WINDOW_H__
#define __WINDOW_H__

#include "src/core/math/Vector.h"
#include "src/engine/renderer/Font.h"
#include "src/engine/renderer/Pickable.h"

namespace energonsoftware {

class Window : public boost::enable_shared_from_this<Window>, public Pickable
{
private:
    static Font default_font;

private:
    // NOTE: must be called *after* a GL context is available
    static bool init();
    static void shutdown();

public:
    Window();
    explicit Window(boost::shared_ptr<Window> parent);
    virtual ~Window() throw();

public:
    const Font& font() const { return default_font; }

    void position(const Position& position) { _position = position; }
    const Position& position() const { return _position; }

    void size(const Size& size) { _size = size; }
    const Size& size() const { return _size; }

    void add_child(boost::shared_ptr<Window> child);

    void render() const;

protected:
    virtual void on_render() const = 0;

private:
    Position _position;
    Size _size;

    boost::shared_ptr<Window> _parent;
    std::vector<boost::shared_ptr<Window> > _children;

private:
    friend class Engine;

private:
    DISALLOW_COPY_AND_ASSIGN(Window);
};

}

#endif

