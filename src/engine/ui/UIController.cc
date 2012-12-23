#include "src/pch.h"
#include "src/engine/Engine.h"
#include "InputState.h"
#include "UIController.h"

namespace energonsoftware {

boost::shared_ptr<UIController> UIController::current_controller(new UIController());
std::stack<boost::shared_ptr<UIController> > UIController::controller_stack;

void UIController::release_controllers()
{
    current_controller.reset(new UIController());
    while(!controller_stack.empty()) {
        controller_stack.pop();
    }
}

UIController::UIController()
{
}

UIController::~UIController() throw()
{
}

}
