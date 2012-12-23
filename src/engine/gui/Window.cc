#include "src/pch.h"
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/renderer/Shader.h"
#include "Window.h"

namespace energonsoftware {

Font Window::default_font;

bool Window::init()
{
    if(!default_font.load("courier", 24)) {
        return false;
    }
    default_font.color(Color(1.0f, 1.0f, 1.0f, 1.0f));

    return true;
}

void Window::shutdown()
{
}

Window::Window()
    : boost::enable_shared_from_this<Window>(), Pickable()
{
}

Window::Window(boost::shared_ptr<Window> parent)
    : boost::enable_shared_from_this<Window>(), Pickable(), _parent(parent)
{
    _parent->add_child(shared_from_this());
}

Window::~Window() throw()
{
}

void Window::add_child(boost::shared_ptr<Window> child)
{
    _children.push_back(child);
}

void Window::render() const
{
    boost::shared_ptr<Shader> shader(Engine::instance().resource_manager().shader("text"));

    shader->begin();
    Engine::instance().renderer().init_shader_matrices(*shader);

    on_render();

    shader->end();

    BOOST_FOREACH(boost::shared_ptr<Window> child, _children) {
        child->render();
    }
}

}
