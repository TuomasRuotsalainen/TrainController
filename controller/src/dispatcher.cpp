#include <memory>
#include <vector>
#include "layout.h"
#include "enums.h"
#include "arduino.h"
#include <iostream>
#include "dispatcher.h"

// this class is responsible for decision making
// initially a simple state machine


bool StateChangeCondition::check_for_change() {
    return false;
}

ConditionRailSectionEmpty::ConditionRailSectionEmpty(RailSection* rail_section)
    : rail_section(rail_section) {
}

bool ConditionRailSectionEmpty::check_for_change() {
    return rail_section != nullptr && rail_section->get_vehicle_count() == 0;
}

StateChange::StateChange(State* next_state, std::unique_ptr<StateChangeCondition> change_condition)
    : next_state(next_state), change_condition(std::move(change_condition)) {
}

State* StateChange::get_next_state() {
    if (change_condition->check_for_change() == true) {
        return next_state;
    }

    return nullptr;
}
State::State(std::vector<Command> commands_on_entry)
    : commands_on_entry(commands_on_entry) {
}

void State::add_state_change(std::unique_ptr<StateChange> state_change) {
    state_changes.push_back(std::move(state_change));
}
    

State* State::get_next_state() {
    for (auto& state_change : state_changes) {
        State* next_state = state_change->get_next_state();
        if (next_state != nullptr) {
            return next_state;
        }
    }

    return nullptr;
}

std::vector<Command> State::get_entry_commands() {
    return this->commands_on_entry;
}

Dispatcher::Dispatcher(Layout* layout, ArduinoInterface* arduino)
    : layout(layout), arduino(arduino) {

    // 1. initialize states
    std::vector<Command> initial_state_commands = {LOCOMOTIVE_STOP};
    initial_state = std::make_unique<State>(initial_state_commands);

    std::vector<Command> moving_to_decoupling_commands = {LOCOMOTIVE_REVERSE};
    moving_to_decoupling = std::make_unique<State>(moving_to_decoupling_commands);

    std::vector<Command> arrived_at_decoupling_commands = {LOCOMOTIVE_STOP};
    arrived_at_decoupling = std::make_unique<State>(arrived_at_decoupling_commands);
    
    std::vector<Command> decoupling_attempt_done_commands = {DECOUPLING_ACTION};
    decoupling_attempt_done = std::make_unique<State>(decoupling_attempt_done_commands);

    std::vector<Command> verifying_decoupling_state_commands = {LOCOMOTIVE_AHEAD};
    verifying_decoupling_state = std::make_unique<State>(verifying_decoupling_state_commands);

    std::vector<Command> moving_to_initial_state_commands = {LOCOMOTIVE_AHEAD};
    moving_to_initial_state = std::make_unique<State>(moving_to_initial_state_commands);

    this->current_state = initial_state.get();

    // 2. define state changes
    std::unique_ptr<StateChangeCondition> initial_condition = std::make_unique<ConditionRailSectionEmpty>(layout->get_trail());
    std::unique_ptr<StateChange> initial_state_change = std::make_unique<StateChange>(moving_to_decoupling.get(), std::move(initial_condition));

    initial_state->add_state_change(std::move(initial_state_change));
}
void Dispatcher::start() {
    // This method is intended to start the dispatcher state machine.
    // Planned logic: repeatedly check for state changes and execute entry commands for the current state.
    // Implementation pending.
    //TODO
}
