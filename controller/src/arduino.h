#ifndef ARDUINO_H
#define ARDUINO_H

#include "enums.h"
#include <string>

class ArduinoInterface {
    public:
        ArduinoInterface();
        int get_status();
        int get_connection();
        void send_command(Command command);
    private:
        int connection;
        int get_arduino_connection();
        void write_message(std::string message);
};

int doStuff();

#endif