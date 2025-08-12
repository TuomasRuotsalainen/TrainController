#include <vector>
#include <memory>
#include <stdexcept> 
#include "enums.h"
#include "layout.h"
#include <iostream>


VehicleCoupling::VehicleCoupling(Vehicle* lead, Vehicle* trailing, CouplerState state) {
    this->lead_vehicle = lead;
    this->trail_vehicle = trailing;
    this->state = state;
}

Locomotive::Locomotive(int actuator_id) : Vehicle(), actuator_locomotive_id(actuator_id) {}

void Locomotive::set_direction(Direction dir) {
    this->direction = dir;
}

Direction Locomotive::get_direction() {
    return direction;
}

RailSection::RailSection(const int vehicle_capacity, RailSection* lead_railsection, RailSection* trail_railsection) {
    this->vehicle_capacity = vehicle_capacity;
    this->lead_railsection = lead_railsection;
    this->trail_railsection = trail_railsection;

    if (lead_railsection != nullptr && lead_railsection->get_trail() == nullptr) {
        lead_railsection->update_trail(this);
    }

    if (trail_railsection != nullptr && trail_railsection->get_lead() == nullptr) {
        trail_railsection->update_lead(this);
    }
}

int RailSection::get_vehicle_count() {
    return vehicles.size();
}

void RailSection::exit_vehicle_lead() {
    if (!vehicles.empty()) {
        vehicles.erase(vehicles.begin());
    } else {
        throw std::out_of_range("No vehicles to exit from lead");
    }
}

void RailSection::exit_vehicle_trailing() {
    if (!vehicles.empty()) {
        vehicles.pop_back();
    } else {
        throw std::out_of_range("No vehicles to exit from trailing");
    }
}

void RailSection::enter_vehicle_lead(Vehicle* vehicle) {
    if (vehicles.size() < static_cast<size_t>(vehicle_capacity)) {
        vehicles.insert(vehicles.begin(), vehicle);
    } else {
        throw std::overflow_error("Vehicle capacity exceeded at lead");
    }
}

void RailSection::enter_vehicle_trailing(Vehicle* vehicle) {
    if (vehicles.size() < static_cast<size_t>(vehicle_capacity)) {
        vehicles.push_back(vehicle);
    } else {
        throw std::overflow_error("Vehicle capacity exceeded at trailing");
    }
}

RailSection* RailSection::get_lead() {
    return this->lead_railsection;
}

RailSection* RailSection::get_trail() {
    return this->trail_railsection;
}

void RailSection::update_lead(RailSection* section) {
    this->lead_railsection = section;
}

void RailSection::update_trail(RailSection* section) {
    this->trail_railsection = section;
}

StraightRail::StraightRail(int vehicle_capacity, RailSection* lead_railsection, RailSection* trail_railsection)
    : RailSection(vehicle_capacity, lead_railsection, trail_railsection) {}

DecouplingRail::DecouplingRail(RailSection* lead, RailSection* trail, int sensor_ir_port_id, int actuator_decoupler_id)
    : RailSection(1, lead, trail), sensor_ir_port_id(sensor_ir_port_id), actuator_decoupler_id(actuator_decoupler_id) {}


Layout::Layout() {
    // TODO add ir sensor and actuator
    int SENSOR_IR_PORT_ID = 0;
    int ACTUATOR_DECOUPLER_ID = 0;

    lead_section = std::make_unique<StraightRail>(10, nullptr, nullptr);
    trail_section = std::make_unique<StraightRail>(10, nullptr, nullptr);
    decoupling_section = std::make_unique<DecouplingRail>(lead_section.get(), trail_section.get(), SENSOR_IR_PORT_ID, ACTUATOR_DECOUPLER_ID);
    
    int ACTUATOR_LOCOMOTIVE_ID = 0;

    locomotive = std::make_unique<Locomotive>(ACTUATOR_LOCOMOTIVE_ID);
    waggon_1 = std::make_unique<Waggon>();

    coupling = std::make_unique<VehicleCoupling>(locomotive.get(), waggon_1.get(), CONNECTED_VERIFIED);

    lead_section->enter_vehicle_lead(locomotive.get());
}

bool Layout::process_event(std::string event) {
    // process sensor event from hardware, and update the layout stateaccordingly
    // port change state means, that a convoy has progressed in two possible ways:
    // 1. a vehicle has entered a decoupling rail section
    // 2. a vehicle has exited a decoupling rail section
    if (event.rfind("PORT1-OPEN", 0) == 0) {
        // a vehicle has exited the decoupling rail section
        // -> find out the rail section where the vehicle will go
        // -> add vehicle to the next rail section and remove it from the decoupling rail section
        std::cout << "Handling open event for PORT1" << std::endl;
        RailSection* next_section;
        if (locomotive->get_direction() == TO_TRAIL) {
            next_section = decoupling_section->get_trail();
        } else {
            next_section = decoupling_section->get_lead();
        }

        this->decoupling_section->exit_vehicle_lead();
        this->lead_section->enter_vehicle_lead(locomotive.get());
        std::cout << "Open event for PORT1 handled" << std::endl;
        return true;
        
    } else if (event.rfind("PORT1-CLOSED", 0) == 0) {
        // a vehicle has entered the decoupling rail section
        // -> find out from which rail section the vehicle came
        // -> remove vehicle from the previous rail section and add it to the decoupling rail section
        std::cout << "Handling close event for PORT1" << std::endl;
        RailSection* previous_section;
        if (locomotive->get_direction() == TO_TRAIL) {
            previous_section = decoupling_section->get_lead();
        } else {
            previous_section = decoupling_section->get_trail();
        }
        this->lead_section->exit_vehicle_trailing();
        this->decoupling_section->enter_vehicle_trailing(locomotive.get());
        std::cout << "Close event for PORT1 handled" << std::endl;
        return true;
    } else {
        //std::cout << "Unknown event: " << event << std::endl;
    }

    return false;
}

void Layout::process_command(Command command) {
    // process a command from the dispatcher
    switch (command) {
        case LOCOMOTIVE_REVERSE:
            std::cout << "Reversing locomotive" << std::endl;
            locomotive->set_direction(TO_TRAIL);
            break;
        case LOCOMOTIVE_AHEAD:
            std::cout << "Moving locomotive ahead" << std::endl;
            locomotive->set_direction(TO_LEAD);
            break;
    }
}

RailSection* Layout::get_lead() {
    return lead_section.get();
}

RailSection* Layout::get_trail() {
    return trail_section.get();
}
DecouplingRail* Layout::get_decoupler() {
    return decoupling_section.get();
}
