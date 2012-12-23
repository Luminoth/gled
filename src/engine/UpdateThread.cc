#include "src/pch.h"
#include "src/core/util/util.h"
#include "ui/UIController.h"
#include "Engine.h"
#include "State.h"
#include "UpdateThread.h"

namespace energonsoftware {

void UpdateThread::destroy(UpdateThread* const thread, MemoryAllocator* const allocator)
{
    thread->~UpdateThread();
    operator delete(thread, *allocator);
}

UpdateThread::UpdateThread()
    : BaseThread("engine-update"), _last_update(0.0)
{
}

UpdateThread::~UpdateThread() throw()
{
}

void UpdateThread::on_run()
{
    _last_update = get_time();

    while(!should_quit()) {
        UIController::controller()->update(get_time() - _last_update);
        Engine::instance().state().scene().update();
        _last_update = get_time();
    }
}

}
