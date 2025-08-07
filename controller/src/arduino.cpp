#include "arduino.h"
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstring>

ArduinoInterface::ArduinoInterface() {
    connection = get_arduino_connection();
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

int doStuff() {
    return 42;
}