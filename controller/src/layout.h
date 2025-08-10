#ifndef LAYOUT_H
#define LAYOUT_H

#include "enums.h"

class Vehicle {

};

class VehicleCoupling {
    public:
        Vehicle* lead_vehicle;
        Vehicle* trail_vehicle;

        CouplerState state;

        VehicleCoupling(Vehicle* lead, Vehicle* trailing, CouplerState state);
};

class Locomotive : public Vehicle {
    private:
        int actuator_locomotive_id;

    public:
        Locomotive(int actuator_id);
};

class Waggon : public Vehicle {};

class RailSection {
    private:
        RailSection* lead_railsection;
        RailSection* trail_railsection;

        std::vector<Vehicle*> vehicles; // the lead vehicle in front

        int vehicle_capacity;

    public:
        RailSection(const int vehicle_capacity, RailSection* lead_railsection, RailSection* trail_railsection);

        int get_vehicle_count();

        // remove instance at index 0 from vehicles
        void exit_vehicle_lead();

        // remove instance at last index from vehicles
        void exit_vehicle_trailing();

        // add vehicle to first index of vehicles
        void enter_vehicle_lead(Vehicle* vehicle);

        // add vehicle to last index of vehicles
        void enter_vehicle_trailing(Vehicle* vehicle);

        RailSection* get_lead();

        RailSection* get_trail();

        void update_lead(RailSection* section);

        void update_trail(RailSection* section);
};

class StraightRail : public RailSection{
    public:
        StraightRail(const int capacity, RailSection* lead, RailSection* trail);
};

class DecouplingRail : public RailSection{
    private:
        int sensor_ir_port_id;
        int actuator_decoupler_id;

    public:
        DecouplingRail(RailSection* lead, RailSection* trail, int ir_port, int decoupler);
};

class Layout{
    private:
        std::unique_ptr<RailSection> lead_section;
        std::unique_ptr<DecouplingRail> decoupling_section;
        std::unique_ptr<RailSection> trail_section;

        std::unique_ptr<Locomotive> locomotive;
        std::unique_ptr<Waggon> waggon_1;
        
        std::unique_ptr<VehicleCoupling> coupling;

    public:
        Layout();

        RailSection* get_lead();

        RailSection* get_trail();

        DecouplingRail* get_decoupler();
};


#endif