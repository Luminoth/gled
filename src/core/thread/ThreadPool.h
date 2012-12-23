#if !defined __THREADPOOL_H__
#define __THREADPOOL_H__

#include "BaseJob.h"

namespace energonsoftware {

class ThreadFactory;

struct base_job_less : public std::binary_function<boost::shared_ptr<BaseJob>, boost::shared_ptr<BaseJob>, bool>
{
    bool operator()(const boost::shared_ptr<BaseJob>& lhs, const boost::shared_ptr<BaseJob>& rhs) const
    { return lhs->priority() < rhs->priority(); }
};

class ThreadPool : public boost::recursive_mutex
{
private:
    static Logger& logger;

public:
    explicit ThreadPool(size_t size);
    virtual ~ThreadPool() throw();

public:
    // starts the threads in the pool
    void start(const ThreadFactory& factory);

    // adds work to the pool
    // NOTE: job must have been declared with new and the pool takes ownership of it
    void push_work(BaseJob* job);

    bool has_work();

    // gets work from the pool
    boost::shared_ptr<BaseJob> pop_work();

    // stops all threads
    void stop();

    bool running() const { return _running; }

private:
    size_t _size;
    boost::thread_group _threads;
    bool _running;
    std::priority_queue<boost::shared_ptr<BaseJob>, std::deque<boost::shared_ptr<BaseJob> >, base_job_less > _work;

private:
    ThreadPool();
    DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};

}

#endif
