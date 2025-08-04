#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

extern std::queue<std::string> eventQueue;
extern std::mutex queueMutex;
extern std::condition_variable cv;
extern std::atomic<bool> running;

int runScheduler();

void eventProducer();

#endif