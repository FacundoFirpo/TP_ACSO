#include "thread-pool.h"
#include <queue>

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    // Inicializamos workers
    for (size_t i = 0; i < numThreads; ++i){
        wts[i].available = true;
        wts[i].ready = new Semaphore(0);
        wts[i].ts = thread([this, i] { worker(i); });
    }

    // Inicializamos dispatcher
    dispatcherSignal = new Semaphore(0);
    remainingTasks = new Semaphore(0);
    dt = thread([this] { dispatcher(); });
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (!thunk) throw invalid_argument("Cannot schedule nullptr function.");

    if (done) throw runtime_error("Cannot schedule task after destruction.");

    {
        lock_guard<mutex> lg(queueLock);
        taskQueue.push(thunk);
    }
    dispatcherSignal->signal();
    remainingTasks->signal();  // para que wait sepa cuántas tareas hay
}

void ThreadPool::dispatcher() {
    while (true) {
        dispatcherSignal->wait();  // espera que schedule le avise que hay algo

        if (done) break;

                // buscar worker disponible
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
                lock_guard<mutex> lg(queueLock);
                if (taskQueue.empty()) continue;  // doble check por race
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
            lock_guard<mutex> lg(wts[id].lock);
            thunkCopy = wts[id].thunk;
        }

        thunkCopy();  // ejecutar

        remainingTasks->wait();  // avisar que terminó
        {
            lock_guard<mutex> lg(wts[id].lock);
            wts[id].available = true;
        }
    }
}

void ThreadPool::wait() {
    while (true) {
        bool all_done = false;
        {
            lock_guard<mutex> lg(queueLock);
            if (taskQueue.empty()) {
                all_done = true;
                for (auto& w : wts) {
                    lock_guard<mutex> lock(w.lock);
                    if (!w.available) {
                        all_done = false;
                        break;
                    }
                }
            }
        }
        if (all_done) break;
        this_thread::yield();
    }
}

ThreadPool::~ThreadPool() {
    wait();
    done = true;
    dispatcherSignal->signal();  // para salir del loop

    for (auto& w : wts) w.ready->signal(); // desbloquear workers si esperan
    dt.join();
    for (auto& w : wts) w.ts.join();

    delete dispatcherSignal;
    delete remainingTasks;
    for (auto& w : wts) delete w.ready;
}
