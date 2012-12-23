#if !defined __BASETHREAD_H__
#define __BASETHREAD_H__

namespace energonsoftware {

class ThreadPool;

class BaseThread
{
private:
    static Logger& logger;

public:
    explicit BaseThread(ThreadPool* pool);
    explicit BaseThread(const std::string& name);

    // NOTE: this calls stop() on the thread
    virtual ~BaseThread() throw();

public:
    std::string name() const;

    void start();

    void quit() { _quit = true; }

    // NOTE: this joins the thread
    void stop();

    bool should_quit() const { return _quit; }

    // releases ownership of the thread (used by the thread pool)
    boost::thread* release();

    std::string str() const;

protected:
    // NOTE: be *extremely* careful when locking this
    // as a deadlock situation exists when the pool is stopped.
    // Use a unique_lock with try_lock() rather than a lock_guard to be safe
    ThreadPool* pool() { return _pool; }
    const ThreadPool* pool() const { return _pool; }

    // for non-pooled threads
    virtual void on_run() {}

private:
    void run();

private:
    ThreadPool* _pool;
    std::string _name;
    bool _quit;

    boost::thread* _thread;
    bool _own_thread;

private:
    DISALLOW_COPY_AND_ASSIGN(BaseThread);
};

// override this and pass to ThreadPool.start() to create
// new threads of a specific type
class ThreadFactory
{
public:
    ThreadFactory();
    virtual ~ThreadFactory() throw();

public:
    virtual BaseThread* new_thread(ThreadPool* pool=NULL) const throw() = 0;
};

}

#endif
