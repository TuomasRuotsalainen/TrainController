#include "arduino.h"
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstring>
#include "enums.h"
#include "scheduler.h"
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

//std::mutex queueMutex;
//std::queue<std::string> eventQueue;
//std::condition_variable cv;

ArduinoInterface::ArduinoInterface() {
    connection = get_arduino_connection();
    this->subscribe();

}

int ArduinoInterface::get_arduino_connection() {

    const char* portname = "/dev/ttyACM0";

    std::cout << "Opening serial connection with arduino on port : " << portname << std::endl;


    int arduino_file_descriptor = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (arduino_file_descriptor < 0) {
        std::cerr << "Arduino connection failed: Error opening " << portname << ": " << strerror(errno) << "\n";
        return -1;
    }

    termios tty{};
    if (tcgetattr(arduino_file_descriptor, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << "\n";
        return -1;
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

    if (tcsetattr(arduino_file_descriptor, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << "\n";
        return 1;
    }

    usleep(100 * 1000); // 100 ms

    char buf[100];
    std::string reply;

    std::cout << "Connection test with arduino..." << std::endl;

    int max_attempts = 10;
    while (true) {
        int n = read(arduino_file_descriptor, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                if (std::string(buf).find("PING_OK") != std::string::npos) {
                //if (strcmp(buf, "PING_OK") == 0) {
                    break;
                } else {
                    std::cout << buf;
                }
            }
        write(arduino_file_descriptor, "PING\n", 5);
        usleep(100 * 1000);
        max_attempts--;

        if (max_attempts == 0) {
            std::cout << "Connection test failed! Check Arduino connecion" << std::endl;
            return -1;
        }

    }

    std::cout << "Arduino connection verified!" << std::endl;
    return arduino_file_descriptor;
}

static inline void trim_newline(std::string &s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) {
        s.pop_back();
    }
}

void ArduinoInterface::serial_read_loop() {
    try {

        this->running = true;

        char buf[100];
        std::string partialMessage; // holds data until we have a full line

        while (running) {
            int n = read(connection, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                partialMessage += buf; // append new chunk

                // Process all complete lines
                size_t pos;
                while ((pos = partialMessage.find('\n')) != std::string::npos) {
                    std::string fullMessage = partialMessage.substr(0, pos);
                    
                    // Remove carriage return if Arduino sends CRLF
                    trim_newline(fullMessage);

                    std::cout << "Raw message (with ASCII codes): ";
                    for (unsigned char c : fullMessage) {
                        std::cout << "[" << (int)c << "]";
                    }
                    std::cout << std::endl;

                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        //std::cout << "Message from Arduino: " << fullMessage << std::endl;
                        eventQueue.push(fullMessage);
                    }
                    cv.notify_one();

                    // Erase processed message from buffer
                    partialMessage.erase(0, pos + 1);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception in serial_read_loop: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in serial_read_loop" << std::endl;
    }
    
}


void ArduinoInterface::subscribe() {
    reader_thread = std::thread(&ArduinoInterface::serial_read_loop, this);

}



int ArduinoInterface::get_status() {
    if (connection == -1) {
        std::cout << "Arduino connection is not OK!" << std::endl;
        return -1;
    }

    return 0;
}

int ArduinoInterface::get_connection() {
    return connection;
}

void ArduinoInterface::write_message(std::string message) {
    std::string message_newline = message + "\n";
    write(this->connection, message_newline.c_str(), message_newline.size());
}

void ArduinoInterface::send_command(Command command) {
    switch (command) {
        case LOCOMOTIVE_AHEAD:
            this->write_message("AHEAD");
            break;
        case LOCOMOTIVE_REVERSE:
            this->write_message("REVERSE");
            break;
        case LOCOMOTIVE_STOP:
            this->write_message("STOP");
            break;
        case DECOUPLING_ACTION:
            this->write_message("DECOUPLE");
            break;
        default:
            this->write_message("ON");
            break;

    }
}


int doStuff() {
    return 42;
}