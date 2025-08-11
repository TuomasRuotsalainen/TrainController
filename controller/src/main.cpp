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
#include "enums.h"

int main() {

    //int arduino_connection = get_arduino_connection();

    //if (arduino_connection == -1) {
    //    return 0;
    //}

    std::unique_ptr<ArduinoInterface> arduino = std::make_unique<ArduinoInterface>();

    std::cout << "test 1" << std::endl;

    while (arduino->get_status() != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "test 2" << std::endl;

    std::unique_ptr<Layout> layout = std::make_unique<Layout>();
    std::unique_ptr<Dispatcher> dispatcher = std::make_unique<Dispatcher>(layout.get(), arduino.get());

    int arduino_connection = arduino->get_connection();

    char buf[100];

    std::thread producer(eventProducer);

    //int processedCount = 0;

    bool stop = false;

            
    // arduino->send_command(LED);

    std::cout << "test 3" << std::endl;

    dispatcher->start();

    std::cout << "test 4" << std::endl;

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] { return !eventQueue.empty() || !running; });

        while (!eventQueue.empty()) {
            std::string event = eventQueue.front();
            eventQueue.pop();
            lock.unlock();

            std::cout << "Processing event: " << event << std::endl;
            layout->process_event(event);

            //std::cout << "Processing led: " << event << std::endl;

            lock.lock();
        }

        if (stop == true) {
            running = false;
            cv.notify_all();
            break;
        }
    }

    if (producer.joinable()) {
        producer.join();
    }
    return 0;


}