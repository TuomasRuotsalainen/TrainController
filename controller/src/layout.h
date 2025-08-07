#ifndef LAYOUT_H
#define LAYOUT_H

class RailSection{
    public:
        int get_vehicle_count();
};

class DecouplingRail{};


class Layout{
    public:
        Layout() {}

        RailSection* get_lead();

        RailSection* get_trail();

        DecouplingRail* get_decoupler();
};


#endif