#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "layout.h"
#include "arduino.h"
#include "enums.h"
#include <vector>
#include <memory>


class StateChangeCondition {
    public:
        virtual bool check_for_change();
        virtual ~StateChangeCondition() = default;

};

class ConditionRailSectionEmpty : public StateChangeCondition {
    private:
        RailSection* rail_section;

    public:
        ConditionRailSectionEmpty(RailSection* rail_section);
        bool check_for_change();

};


class State;

class StateChange {
    private:
        State* next_state;
        std::unique_ptr<StateChangeCondition> change_condition;

    public:
        StateChange(State* next_state, std::unique_ptr<StateChangeCondition> change_condition);
        State* get_next_state();

};


class State {
    private:
        std::vector<Command> commands_on_entry;
        std::vector<std::unique_ptr<StateChange>> state_changes;

    public:
        State(std::vector<Command> commands_on_entry);
        void add_state_change(std::unique_ptr<StateChange> state_change);
        State* get_next_state();
        std::vector<Command> get_entry_commands();

};


class Dispatcher {
    private:
        Layout* layout;
        ArduinoInterface* arduino;
        State* current_state;

        std::unique_ptr<State> initial_state;
        std::unique_ptr<State> moving_to_decoupling;
        std::unique_ptr<State> arrived_at_decoupling;
        std::unique_ptr<State> decoupling_attempt_done;
        std::unique_ptr<State> verifying_decoupling_state;
        std::unique_ptr<State> moving_to_initial_state;

        void change_state(State* next_state);


    public:
        Dispatcher(Layout* layout, ArduinoInterface* arduino);
        void start();
        void check_for_state_change();
};





#endif