
#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <string>
#include "scheduler.h"

int runScheduler() {
    std::cout << "Scheduler is running..." << std::endl;
    return 0;
}

std::queue<std::string> eventQueue;
std::mutex queueMutex;
std::condition_variable cv;
std::atomic<bool> running(true);

void eventProducer() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> delayDist(1, 4);
    int counter = 0;

    // this can be used to produce events at random intervals
    // disabled by default, not needed right now

    /*
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(delayDist(gen)));
        std::string event = "Event #" + std::to_string(++counter);

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            eventQueue.push(event);
        }
        cv.notify_one();
    }
    */
}