#include "src/pch.h"
#include "src/engine/Engine.h"
#include "src/engine/renderer/Renderer.h"
#include "StaticTextCtrl.h"

namespace energonsoftware {

StaticTextCtrl::StaticTextCtrl(boost::shared_ptr<Window> parent, const std::string& text)
    : Window(parent), _text(text)
{
}

StaticTextCtrl::~StaticTextCtrl() throw()
{
}

void StaticTextCtrl::on_render() const
{
    Engine::instance().renderer().render_text(font(), position(), _text);
}

}
