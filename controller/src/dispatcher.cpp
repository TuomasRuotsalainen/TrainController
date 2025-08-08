#include <memory>
#include <vector>
#include "layout.h"
#include "enums.h"
#include "arduino.h"

// this class is responsible for decision making
// initially a simple state machine


class State;

class StateChangeCondition {
    private:

    public:
        bool check_for_change() {
            return false;
        }

};

class ConditionRailSectionEmpty : public StateChangeCondition {
    private:
        RailSection* rail_section;

    public:
        ConditionRailSectionEmpty(RailSection* rail_section) {
            this->rail_section=rail_section;

        }
        bool check_for_change() {
            return rail_section != nullptr && rail_section->get_vehicle_count() == 0;
        }

};

class StateChange {
    private:
        State* next_state;
        std::unique_ptr<StateChangeCondition> change_condition;

    public:
        StateChange(State* next_state, std::unique_ptr<StateChangeCondition> change_condition) {
            this->next_state=next_state;
            this->change_condition=std::move(change_condition);
        }

        // only if change is needed
        State* get_next_state() {
            if (change_condition->check_for_change() == true) {
                return next_state;
            }

            return nullptr;
        }

};

class State {
    private:
        std::vector<Command> commands_on_entry;
        std::vector<std::unique_ptr<StateChange>> state_changes;

    public:
        State(std::vector<Command> commands_on_entry) {
            this->commands_on_entry=commands_on_entry;
        }

        void add_state_change(std::unique_ptr<StateChange> state_change) {
            this->state_changes.push_back(std::move(state_change));
        }

        State* get_next_state() {
            for (auto& state_change : state_changes) {
                State* next_state = state_change->get_next_state();
                if (next_state != nullptr) {
                    return next_state;
                }
            }

            return nullptr;
        }

        std::vector<Command> get_entry_commands() {
            return this->commands_on_entry;
        }

};



class Dispatcher {

    private:
        std::unique_ptr<State> initial_state; // loco and waggon on lead section, connection good
        std::unique_ptr<State> moving_to_decoupling;
        std::unique_ptr<State> arrived_at_decoupling;
        std::unique_ptr<State> decoupling_attempt_done;
        std::unique_ptr<State> verifying_decoupling_state;
        std::unique_ptr<State> moving_to_initial_state;
        // todo add coupling

        State* currrent_state;

        Layout* layout;
        ArduinoInterface* arduino;

    public:
        Dispatcher(Layout* layout, ArduinoInterface* arduino) {

            this->layout=layout;
            this->arduino=arduino;

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

            this->currrent_state = initial_state.get();

            // 2. define state changes
            std::unique_ptr<StateChangeCondition> initial_condition = std::make_unique<ConditionRailSectionEmpty>(layout->get_trail());
            std::unique_ptr<StateChange> initial_state_change = std::make_unique<StateChange>(moving_to_decoupling.get(), std::move(initial_condition));

            initial_state->add_state_change(std::move(initial_state_change));
        }

        void check_for_state_change() {
            State* next_state = this->currrent_state->get_next_state();
            if (next_state != nullptr) {
                this->change_state(next_state);

            }
        }

        void change_state(State* next_state) {
            this->currrent_state = next_state;
            std::vector<Command> commands = this->currrent_state->get_entry_commands();

            for (auto& command : commands) {
                arduino->send_command(command);
            }

        }

};