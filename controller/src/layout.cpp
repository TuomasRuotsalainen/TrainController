#include <vector>
#include <memory>
#include <stdexcept> 


class Vehicle {

};

enum CouplerState {
    CONNECTED_VERIFIED,
    CONNECTED_UNVERIFIED,
    UNCONNECTED_VERIFIED,
    UNCONNECTED_UNVERIFIED
};


class VehicleCoupling {
    public:
        Vehicle* lead_vehicle;
        Vehicle* trail_vehicle;

        CouplerState state;

        VehicleCoupling(Vehicle* lead, Vehicle* trailing, CouplerState state) {
            this->lead_vehicle=lead;
            this->trail_vehicle=trailing;
            this->state=state;
        }
};


class Locomotive : public Vehicle {
    private:
        int actuator_locomotive_id;

    public:
        Locomotive(int actuator_id) : Vehicle(), actuator_locomotive_id(actuator_id) {}
};

class Waggon : public Vehicle {};


class RailSection {
    private:
        RailSection* lead_railsection;
        RailSection* trail_railsection;

        std::vector<Vehicle*> vehicles; // the lead vehicle in front

        int vehicle_capacity;

    public:
        RailSection(const int vehicle_capacity, RailSection* lead_railsection, RailSection* trail_railsection) {
            this->vehicle_capacity=vehicle_capacity;
            this->lead_railsection=lead_railsection;
            this->trail_railsection=trail_railsection;

            if (lead_railsection != nullptr && lead_railsection->get_trail() == nullptr) {
                lead_railsection->update_trail(this);
            }

            if (trail_railsection != nullptr && trail_railsection->get_lead() == nullptr) {
                trail_railsection->update_lead(this);
            }

        }

        int get_vehicle_count() {
            return vehicles.size();
        }

        // remove instance at index 0 from vehicles
        void exit_vehicle_lead() {
            if (!vehicles.empty()) {
                vehicles.erase(vehicles.begin());
            } else {
                throw std::out_of_range("No vehicles to exit from lead");
            }
        }

        // remove instance at last index from vehicles
        void exit_vehicle_trailing() {
            if (!vehicles.empty()) {
                vehicles.pop_back();
            } else {
                throw std::out_of_range("No vehicles to exit from trailing");
            }
        }

        // add vehicle to first index of vehicles
        void enter_vehicle_lead(Vehicle* vehicle) {
            if (vehicles.size() < static_cast<size_t>(vehicle_capacity)) {
                vehicles.insert(vehicles.begin(), vehicle);
            } else {
                throw std::overflow_error("Vehicle capacity exceeded at lead");
            }
        }

    // add vehicle to last index of vehicles
    void enter_vehicle_trailing(Vehicle* vehicle) {
        if (vehicles.size() < static_cast<size_t>(vehicle_capacity)) {
            vehicles.push_back(vehicle);
        } else {
            throw std::overflow_error("Vehicle capacity exceeded at trailing");
        }
    }

        RailSection* get_lead() {
            return this->lead_railsection;
        }

        RailSection* get_trail() {
            return this->trail_railsection;
        }

        void update_lead(RailSection* section) {
            this->lead_railsection = section;
        }

        void update_trail(RailSection* section) {
            this->trail_railsection = section;
        }
};

class StraightRail : public RailSection{
    public:
        StraightRail(const int capacity, RailSection* lead, RailSection* trail) : RailSection(capacity, lead, trail) {}
};

class DecouplingRail : public RailSection{
    private:
        int sensor_ir_port_id;
        int actuator_decoupler_id;

    public:
        DecouplingRail(RailSection* lead, RailSection* trail, int ir_port, int decoupler) : RailSection(1, lead, trail), sensor_ir_port_id(ir_port), actuator_decoupler_id(decoupler) {
        }
};




class Layout {
    private:
        std::unique_ptr<RailSection> lead_section;
        std::unique_ptr<DecouplingRail> decoupling_section;
        std::unique_ptr<RailSection> trail_section;

        std::unique_ptr<Locomotive> locomotive;
        std::unique_ptr<Waggon> waggon_1;
        
        std::unique_ptr<VehicleCoupling> coupling;


    public:
        Layout() {
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
        }

        RailSection* get_lead() {
            return lead_section.get();
        }

        RailSection* get_trail() {
            return trail_section.get();
        }

        DecouplingRail* get_decoupler() {
            return decoupling_section.get();
        }
};

