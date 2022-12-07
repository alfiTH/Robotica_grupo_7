
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Eigen/Dense>
#include <genericworker.h>
#include "robot.h"


class State_machine
{
    public:
        State_machine(rc::Robot *robot);
        ~State_machine();
        // state machine
        Eigen::Vector3f state_machine(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line);
        enum class State {IDLE, SEARCHING, APPROACHING, WAITING, LOST};
        State state = State::SEARCHING;
        Eigen::Vector3f search_state(const RoboCompYoloObjects::TObjects &objects);
        Eigen::Vector3f approach_state(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line);
        Eigen::Vector3f wait_state();
        Eigen::Vector3f lost_state(const RoboCompYoloObjects::TObjects &objects);
        int errorFrame;
    private:
        rc::Robot *robot;
};
#endif
