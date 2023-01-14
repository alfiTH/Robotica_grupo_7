#include "state_machine.h"
#include <cppitertools/range.hpp>

////////////////////ESTADOS////////////////////////////////////////////

 State_machine::State_machine(){
    this->state = State::SCAN;
    this->errorFrame = 0;
    std::cout << "STATE MACHINE CREATE"<< endl;
 }

void State_machine::initialize(rc::Robot *robot){
    this->robot = robot;


}

bool in_range(float a, float b, float umbral)
{
    std::cout<<"Distanca a "<< a<< "Distancia b"<<b<<std::endl;
    return (a < b + umbral) && (a > b - umbral);
}


void State_machine::state_machine_action(std::vector<GenericObject> &objects)
{
    switch (state)
    {
        case State_machine::State::SCAN:
                    {
                        scan_state(objects);
                        break;
                    }
        case State_machine::State::FIND:
                    {
                        find_state(objects);
                        break;
                    }
        case State_machine::State::ID_ROOM:
                    {
                        id_room_state(objects);
                        break;
                    }
        case State_machine::State::SEARCHING:
                    {
                        search_state(objects);
                        break;
                    }
        case State_machine::State::APPROACHING:
                    {
                        approach_state(objects);
                        break;
                    }
        case State_machine::State::WAITING:
                    {
                        wait_state();
                        break;
                    }
        default: break;
    }
}

void State_machine::state_machine_condition(std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    switch (state)
    {
        case State_machine::State::SCAN:
                    {
                    
                        break;
                    }
        case State_machine::State::FIND:
                    {
                        
                        break;
                    }
        case State_machine::State::ID_ROOM:
                    {
                        
                        break;
                    }
        case State_machine::State::SEARCHING:
                    {
                    for(auto &object : objects)
                    {
                        if(!target.sameType(object))
                        {
                            robot->set_current_target(object);
                            state =  State_machine::State::APPROACHING;
                        }   
                    }
                    break;
                    }
        case State_machine::State::APPROACHING:
                    {
                    
                        break;
                    }
        case State_machine::State::WAITING:
                    {
                        
                        break;
                    }
        default: break;
    }
}

//////////////////////////ACCIONES////////////////////////

void State_machine::scan_state(std::vector<GenericObject> &objects)
{
    qInfo()<<__FUNCTION__;
    static std::vector<GenericObject> objectList;
    robot->rotate(0.5);
    float umbral_mismo_obj = 500;
    
    for(auto &object : objects)
    {
        if (robot->get_robot_target_coordinates(object.get_target_coordinates()).norm() < 1000)
        {
            string type = object.getTypeObject();

            //El objeto esta repetido?
            auto itObj = std::find_if(objectList.begin(), objectList.end(),
                                      [type](GenericObject &s) { return s.getTypeObject() == type; });
            //el tipo no esta repetido lo añadimos
            if (itObj == objectList.end()) {
                objectList.push_back(object);
                std::cout << "añadido directo: " << type;
            //en caso de mismo tipo observamos si es mismo mediante distancias
            } else if (not in_range(robot->get_robot_target_coordinates(object.get_target_coordinates()).norm(),
                                robot->get_robot_target_coordinates(itObj->get_target_coordinates()).norm(), umbral_mismo_obj)  ) {
                objectList.push_back(object);
                std::cout << "añadido comparado: " << type;
            }
        }
    }   
    for (auto &x : objectList){
        std::cout << x.getTypeObject() << " ";
    }
    std::cout << std::endl;
    if (objectList.size()>4)
    {
        if((objectList.end() - 1 == objectList.begin() + 1) && (objectList.end() - 2 == objectList.begin()))
        {
            static std::set<string> typeList;
            for(auto &object : objectList)
            {
                if (object.getTypeObject() == "Door"){
                    this->posOldDoorRoom = typeList.size(); 
                    this->oldDoorRoom = object;
                }
                typeList.insert(object.getTypeObject());
            }
            int idNode = graph.add_node();
            graph.set_tags(idNode, typeList);
            objectList.clear();
        }
    }
}

//Buscará en el grafo el camino más corto para llegar a la sala que contiene X objeto
void State_machine::find_state( std::vector<GenericObject> &objects)
{
    
    qInfo()<<__FUNCTION__;
    robot->rotate(0);
}

void State_machine::id_room_state( std::vector<GenericObject> &objects)
{
    
    qInfo()<<__FUNCTION__;
    robot->rotate(0);
}

void State_machine::search_state( std::vector<GenericObject> &objects)
{
    
    qInfo()<<__FUNCTION__;   
    robot->rotate(-0.5);
}

void State_machine::approach_state( std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    //qInfo()<<__FUNCTION__<< robot->has_target()<< target.type <<target.x<<target.y<<target.z;
    
    if (robot->get_distance_to_target()< 300.0)
    {
        robot->set_has_target(false);
        state =  State_machine::State::WAITING;
        //return Eigen::Vector3f{0.f, 0.f, 0.f};
        //robot->rotate();
    }

    /// eye tracking: tracks  current selected object or  IOR if none
    //eye_track(robot);

    for(auto &object : objects) {
        if(target.sameType(object))
        {
            robot->set_current_target(object);
            errorFrame = 0;
            break;
        }
    }
    
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

void State_machine::wait_state()
{
    qInfo()<<__FUNCTION__;
    sleep(2);
    state =  State_machine::State::SEARCHING;

}






