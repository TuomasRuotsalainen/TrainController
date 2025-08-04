#include <iostream>
#include <thread>
#include "arduino.h"
#include "scheduler.h"

int arduino(int param);

int main() {

    std::thread producer(eventProducer);

    int processedCount = 0;
    const int maxEvents = 5; // stop after 5 events for demo

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] { return !eventQueue.empty() || !running; });

        while (!eventQueue.empty()) {
            std::string event = eventQueue.front();
            eventQueue.pop();
            lock.unlock();

            std::cout << "Processing: " << event << std::endl;
            processedCount++;

            lock.lock();
        }

        if (processedCount >= maxEvents) {
            running = false;
            cv.notify_all();
            break;
        }
    }

    producer.join();
    return 0;


    /*
    for (int i = 1; i <= 10; ++i) {
        std::cout << "Current number: " << i << std::endl;
    }

    int jee { 20 };

    std::cout << "Testing arduino: " << arduino(5) << std::endl;

    std::cout << "Testing doStuff: " << doStuff() << std::endl;

    std::cout << "Testing runScheduler: " << runScheduler() << std::endl;

    std::cout << "Hello, C++!" << std::endl;
    return 0;
    */
}