#if !defined __UPDATETHREAD_H__
#define __UPDATETHREAD_H__

#include "src/core/thread/BaseThread.h"

namespace energonsoftware {

class UpdateThread : public BaseThread
{
public:
    static void destroy(UpdateThread* const state, MemoryAllocator* const allocator);

public:
    virtual ~UpdateThread() throw();

private:
    virtual void on_run();

private:
    double _last_update;

private:
    friend class Engine;
    UpdateThread();
    DISALLOW_COPY_AND_ASSIGN(UpdateThread);
};

}

#endif
