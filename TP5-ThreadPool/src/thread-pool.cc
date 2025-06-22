#include "thread-pool.h"

/*
Inicializo ThreadPool con un numero de workers, cada worker tiene su propio thread,
y un semaphore para que el dispatcher pueda avisarle cuando tiene trabajo
*/
ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false), activeTasks(0) {
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

/*
Pushea tareas y le avisa al dispatcher
*/
void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (!thunk) throw invalid_argument("Cannot schedule nullptr function.");
    if (done) throw runtime_error("Cannot schedule task after destruction.");

    {
        lock_guard<mutex> lock(queueLock);
        taskQueue.push(thunk);
        activeTasks++;
    }

    dispatcherSignal->signal();
}

/*
Asigna una tarea a un worker disponible
Si no hay workers disponibles, espera a que uno se libere
*/
void ThreadPool::dispatcher() {
    while (true) {
        dispatcherSignal->wait();

        if (done) break;

        int workerId = -1;
        // Busca un worker disponible
        while (workerId == -1 && !done) {
            for (size_t i = 0; i < wts.size(); ++i) {
                unique_lock<mutex> lock(wts[i].lock);
                if (wts[i].available) {
                    wts[i].available = false;
                    workerId = i;
                    break;
                }
            }
            if (workerId == -1) this_thread::yield(); // no hay workers disponibles entonces espera
        }


        // Si encontramos un worker disponible, le asignamos una tarea
        if (workerId != -1) {
            function<void(void)> thunk;
            {
                lock_guard<mutex> lock(queueLock);
                if (taskQueue.empty()) continue;
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

/*
Ejecuta las tareas
*/
void ThreadPool::worker(int id) {
    while (true) {
        wts[id].ready->wait(); // espera una tarea
        if (done) break;

        function<void(void)> thunkCopy;
        {
            lock_guard<mutex> lock(wts[id].lock);
            thunkCopy = wts[id].thunk;
        }

        thunkCopy(); // ejecuta la tarea

        {
            lock_guard<mutex> lock(wts[id].lock);
            wts[id].available = true; // vuelve a estar disponible
        }

        // actualiza el número de tareas activas
        {
            lock_guard<mutex> lock(queueLock);
            activeTasks--;
            if (activeTasks == 0) {
                cv_wait.notify_all();
            }
        }
    }
}

/*
Bloquea hasta que todas las tareas hayan sido ejecutadas
*/
void ThreadPool::wait() {
    unique_lock<mutex> lock(queueLock);
    cv_wait.wait(lock, [this] {
        return activeTasks == 0;
    });
}

/*
Cuando termina, libera todo
*/
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
