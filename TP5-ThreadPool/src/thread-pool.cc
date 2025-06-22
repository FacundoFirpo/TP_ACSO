#include "thread-pool.h"

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false), tasksInFlight(0) {
    // workers
    for (size_t i = 0; i < numThreads; ++i) {
        wts[i].available = true;
        wts[i].ready = new Semaphore(0);
        wts[i].ts = thread([this, i] { worker(i); });
    }

    // dispatcher
    dispatcherSignal = new Semaphore(0);
    dt = thread([this] { dispatcher(); });
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (!thunk) throw invalid_argument("Cannot schedule nullptr function.");
    if (done) throw runtime_error("Cannot schedule task after destruction.");

    {
        lock_guard<mutex> lock(queueLock);
        taskQueue.push(thunk);
        tasksInFlight++;
    }

    dispatcherSignal->signal();
}

void ThreadPool::dispatcher() {
    while (true) {
        dispatcherSignal->wait();

        if (done) break;

        int workerId = -1;
        while (workerId == -1 && !done) {
            for (size_t i = 0; i < wts.size(); ++i) {
                unique_lock<mutex> lock(wts[i].lock);
                if (wts[i].available) {
                    wts[i].available = false;
                    workerId = i;
                    break;
                }
            }
            if (workerId == -1) this_thread::yield();
        }

        if (workerId != -1) {
            function<void(void)> thunk;
            {
                lock_guard<mutex> lock(queueLock);
                if (taskQueue.empty()) continue; // doble check para race
                thunk = taskQueue.front();
                taskQueue.pop();
            }
            {
                lock_guard<mutex> lock(wts[workerId].lock);
                wts[workerId].thunk = thunk;
            }
            wts[workerId].ready->signal();
        }
    }
}

void ThreadPool::worker(int id) {
    while (true) {
        wts[id].ready->wait();
        if (done) break;

        function<void(void)> thunkCopy;
        {
            lock_guard<mutex> lock(wts[id].lock);
            thunkCopy = wts[id].thunk;
        }

        thunkCopy();

        {
            lock_guard<mutex> lock(wts[id].lock);
            wts[id].available = true;
        }

        {
            lock_guard<mutex> lock(queueLock);
            tasksInFlight--;
            if (tasksInFlight == 0) {
                cv_wait.notify_all();
            }
        }
    }
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(queueLock);
    cv_wait.wait(lock, [this] {
        return tasksInFlight == 0;
    });
}

ThreadPool::~ThreadPool() {
    wait();
    done = true;
    dispatcherSignal->signal();

    for (auto& w : wts) w.ready->signal();
    dt.join();
    for (auto& w : wts) w.ts.join();

    delete dispatcherSignal;
    for (auto& w : wts) delete w.ready;
}
