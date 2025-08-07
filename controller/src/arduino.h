#ifndef ARDUINO_H
#define ARDUINO_H

class ArduinoInterface {
    public:
        ArduinoInterface();
        int get_status();
        int get_connection();
    private:
        int connection;
        int get_arduino_connection();
};

int doStuff();

#endif