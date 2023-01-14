
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Eigen/Dense>
#include <genericworker.h>
#include <timer/timer.h>
#include <vector>
#include "robot.h"
#include "generic_object.h"
#include <graph.h>

class State_machine
{
    public:
        State_machine();
        // state machine
        void initialize(rc::Robot *robot);
        void state_machine_action(std::vector<GenericObject> &objects);
        void state_machine_condition(std::vector<GenericObject> &objects);     
        enum class State {SEARCHING, APPROACHING, WAITING, ID_ROOM, SCAN, FIND};
        State state;
        void search_state(std::vector<GenericObject> &objects);
        void scan_state(std::vector<GenericObject> &objects);
        void find_state(std::vector<GenericObject> &objects);
        void id_room_state(std::vector<GenericObject> &objects);
        void approach_state(std::vector<GenericObject> &objects);
        void wait_state();
        int errorFrame;
        //int numRoom = 0;
    private:
        rc::Robot *robot;
        Graph graph;
        GenericObject oldDoorRoom;
        int posOldDoorRoom;

};
#endif
