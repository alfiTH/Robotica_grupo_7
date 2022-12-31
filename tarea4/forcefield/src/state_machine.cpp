#include "state_machine.h"
#include <cppitertools/range.hpp>

////////////////////ESTADOS////////////////////////////////////////////



void State_machine::initialize(rc::Robot *robot){
    this->robot = robot;
    this->errorFrame = 0;
    this->state = State_machine::State::SEARCHING;
}


Eigen::Vector3f State_machine::state_machine(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line)
{
    Eigen::Vector3f target;
    switch (state)
    {
        case State_machine::State::SEARCHING:
                    {
                        target = search_state(objects);
                        break;
                    }
        case State_machine::State::APPROACHING:
                    {
                        target = approach_state(objects, line);
                        break;
                    }
        case State_machine::State::WAITING:
                    {
                        target = wait_state();
                        break;
                    }
        case State_machine::State::LOST:
                    {
                        target = lost_state(objects);
                        break;
                    }
        case State_machine::State::IDLE:
                    {

                        break;
                    }
        default: break;
    }
    return target;
}

Eigen::Vector3f State_machine::search_state(const RoboCompYoloObjects::TObjects &objects)
{
    RoboCompYoloObjects::TBox target = robot->get_current_target();
    qInfo()<<__FUNCTION__<< robot->has_target()<< target.type<<target.x<<target.y<<target.z;
    
    if(robot->has_target() == false)
        for(auto &object : objects)
            if(target.type != object.type)
            {
                robot->set_current_target(object);
                state =  State_machine::State::APPROACHING;
                return  robot->get_robot_target_coordinates();
            }    return  Eigen::Vector3f{25.f, 0.5, 0.f};
}

Eigen::Vector3f State_machine::approach_state(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line)
{
    RoboCompYoloObjects::TBox target = robot->get_current_target();
    qInfo()<<__FUNCTION__<< robot->has_target()<< target.type <<target.x<<target.y<<target.z;
    
    if (robot->get_distance_to_target()< 300.0)
    {
        robot->set_has_target(false);
        state =  State_machine::State::WAITING;
        return Eigen::Vector3f{0.f, 0.f, 0.f};
    }

    /// eye tracking: tracks  current selected object or  IOR if none
    //eye_track(robot);

    for(auto &object : objects) {
        if(target.type == object.type)
        {
            robot->set_current_target(object);
            errorFrame = 0;
            break;
        }
        
        else if (object == objects.back())
             errorFrame++;
    }

    if (errorFrame > 4)
    {
        state =  State_machine::State::LOST;
        return  Eigen::Vector3f{0.f, 0.f, 0.f};
    }
    return robot->get_robot_target_coordinates();
}
/*
Eigen::Vector3f State_machine::wait_state()
{
    static rc::Timer clock;    
    static const uint  TIME_INTERVAL = 2000; //ms    static bool first_time = true;    if (first_time)
    {
        clock.tick();
        first_time = false;
        qInfo() << FUNCTION << "Waiting";
    }
    else    
    {
        clock.tock();        
        if(clock.duration() >= TIME_INTERVAL)
        {
            state = State::SEARCHING;            
            first_time = true;        
        }
    }
    return Eigen::Vector3f{0.f, 0.f, 0.f};
}
*/

Eigen::Vector3f State_machine::wait_state()
{
    qInfo()<<__FUNCTION__;
    sleep(2);
    state =  State_machine::State::SEARCHING;
    return Eigen::Vector3f{0.f, 0.f, 0.f};

}

Eigen::Vector3f State_machine::lost_state(const RoboCompYoloObjects::TObjects &objects)
{
    RoboCompYoloObjects::TBox target = robot->get_current_target();
    qInfo()<<__FUNCTION__<< robot->has_target()<< target.type <<target.x<<target.y<<target.z;
    for(auto &object : objects) {
        if(target.type == object.type)
        {   
            robot->set_current_target(object);
            state =  State_machine::State::APPROACHING;
            return  robot->get_robot_target_coordinates();
        }
    }
    auto tag =robot->get_robot_target_coordinates();
    return Eigen::Vector3f{-tag.x(), -tag.y(), -tag.z()};

}


