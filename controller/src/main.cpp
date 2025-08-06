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

int arduino(int param);

int main() {
    
    const char* portname = "/dev/ttyACM0";

    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Error opening " << portname << ": " << strerror(errno) << "\n";
        return 1;
    }

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << "\n";
        return 1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 0;    // Non-blocking read
    tty.c_cc[VTIME] = 5;    // 0.5 sec timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << "\n";
        return 1;
    }

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
            write(fd, "ON\n", 3);

            int n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                std::cout << "Arduino says: " << buf;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            write(fd, "OFF\n", 4);

            n = read(fd, buf, sizeof(buf) - 1);
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