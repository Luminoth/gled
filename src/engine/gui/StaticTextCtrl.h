#if !defined __STATICTEXTCTRL_H__
#define __STATICTEXTCTRL_H__

#include "Window.h"

namespace energonsoftware {

class StaticTextCtrl : public Window
{
public:
    StaticTextCtrl(boost::shared_ptr<Window> parent, const std::string& text="");
    virtual ~StaticTextCtrl() throw();

private:
    void on_render() const;

private:
    std::string _text;

private:
    StaticTextCtrl();
    DISALLOW_COPY_AND_ASSIGN(StaticTextCtrl);
};

}

#endif

