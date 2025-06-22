/**
 * File: tptest.cc
 * ---------------
 * Simple test in place to verify that the ThreadPool class works.
 */

#include <iostream>
#include "thread-pool.h"
#include <mutex>
using namespace std;

void sleep_for(int slp){
    this_thread::sleep_for(chrono::milliseconds(slp));
}

static mutex oslock;

static const size_t kNumThreads = 12;
static const size_t kNumFunctions = 1000;
#include <atomic>
static atomic<int> completed{0};

static void simpleTest() {
  ThreadPool pool(kNumThreads);
  for (size_t id = 0; id < kNumFunctions; id++) {
    pool.schedule([id] {
      {
        lock_guard<mutex> lock(oslock);
        cout << "Thread (ID: " << id << ") has started." << endl;
      }
      size_t sleepTime = (id % 3) * 10;
      sleep_for(sleepTime);
      {
        lock_guard<mutex> lock(oslock);
        cout << "Thread (ID: " << id << ") has finished." << endl;
      }
      completed.fetch_add(1, memory_order_relaxed);
    });
  }

  pool.wait();

  cout << "Tareas completadas: " << completed.load() << " de " << kNumFunctions << endl;
}

int main(int argc, char *argv[]) {
  simpleTest();
  return 0;
}
