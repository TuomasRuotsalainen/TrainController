#ifndef ARDUINO_H
#define ARDUINO_H

#include "enums.h"
#include <string>
#include <thread>

class ArduinoInterface {
    public:
        ArduinoInterface();
        int get_status();
        int get_connection();
        void send_command(Command command);
    private:
        bool running;
        int connection;
        int get_arduino_connection();
        void write_message(std::string message);
        void subscribe();
        void serial_read_loop();
        std::thread reader_thread;


};

int doStuff();

#endif