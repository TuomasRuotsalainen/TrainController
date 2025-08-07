#include <iostream>
#include <thread>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstring>
#include "arduino.h"
#include "scheduler.h"
#include "layout.h"
#include "dispatcher.h"

int arduino(int param);

int main() {

    int arduino_connection = get_arduino_connection();

    if (arduino_connection == -1) {
        return 0;
    }

    std::unique_ptr<Layout> layout = std::make_unique<Layout>();
    std::unique_ptr<Dispatcher> dispatcher = std::make_unique<Dispatcher>();

    
    char buf[100];

    std::thread producer(eventProducer);

    int processedCount = 0;
    const int maxEvents = 10; // stop after 5 events for demo

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] { return !eventQueue.empty() || !running; });

        while (!eventQueue.empty()) {
            std::string event = eventQueue.front();
            eventQueue.pop();
            lock.unlock();

            std::cout << "Processing led: " << event << std::endl;
            processedCount++;
            write(arduino_connection, "ON\n", 3);

            int n = read(arduino_connection, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                std::cout << "Arduino says: " << buf;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            write(arduino_connection, "OFF\n", 4);

            n = read(arduino_connection, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                std::cout << "Arduino says: " << buf;
            }

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