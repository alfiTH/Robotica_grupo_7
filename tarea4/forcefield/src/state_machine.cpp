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

bool equal_object(GenericObject object1, GenericObject object2, float tolerance, rc::Robot *robot)
{
    return object1.getTypeObject() == object2.getTypeObject() && in_range(
        robot->get_robot_target_coordinates(object2.get_target_coordinates()).norm(),
        robot->get_robot_target_coordinates(
        object1.get_target_coordinates()).norm(), tolerance);
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
                    if(objects.end() != std::find_if(objects.begin(), objects.end(),
                            [obj = this->oldDoorRoom, robot=this->robot, umbral_mismo_obj = 500](GenericObject &s) { 
                            return equal_object(*obj, s, umbral_mismo_obj, robot); }))
                    {
                        robot->set_current_target(*this->oldDoorRoom);
                        state =  State_machine::State::APPROACHING;
                    }
                    break;
                    }
        case State_machine::State::APPROACHING:
                    {
                    if (robot->get_distance_to_target()<300)
                    {
                        robot->set_has_target(false);
                        state =  State_machine::State::WAITING;
                        //return Eigen::Vector3f{0.f, 0.f, 0.f};
                        //robot->rotate();
                    }
                        break;
                    }
        case State_machine::State::WAITING:
                    {
                        sleep(2);
                        robot->advance(0);
                        state =  State_machine::State::SCAN;
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
    robot->rotate(0.2);
    float umbral_mismo_obj = 750;
    
    for(auto &object : objects)
    {
        if (robot->get_robot_target_coordinates(object.get_target_coordinates()).norm() < 2000)
        {
            //El objeto esta repetido?
            auto itObj = std::find_if(objectList.begin(), objectList.end(),
                                      [obj = &object, robot=this->robot, umbral_mismo_obj](GenericObject &s) { 
                                    return equal_object(*obj, s, umbral_mismo_obj, robot); });
            //el tipo no esta repetido lo añadimos
            if (itObj == objectList.end()) {
                objectList.push_back(object);
                std::cout << "añadido : " << object.getTypeObject()<<std::endl;
            }
        }
    }   
    
    for (auto &x : objectList){
        std::cout << x.getTypeObject() << ", ";
    }
    std::cout<<std::endl;
    
    if (objectList.size()>4)
    {
        if(objects.end() == std::find_if(objects.begin(), objects.end(),
                                [obj = &objectList.at(0), robot=this->robot, umbral_mismo_obj](GenericObject &s) { 
                                return equal_object(*obj, s, umbral_mismo_obj, robot); }))
        {
            qInfo()<< "Insercion de nodo";
            std::set<string> typeList;
            bool firstDoor = false;
            for(auto &obj : objectList)
            {
                typeList.emplace(obj.getTypeObject());
                if (!firstDoor && obj.getTypeObject() == "Door"){
                    this->posOldDoorRoom = typeList.size()-1; 
                    GenericObject *auxObj = new GenericObject(obj);
                        delete this->oldDoorRoom;
                    this->oldDoorRoom = auxObj;
                    firstDoor = true;
                    }
            }
            int idNode = graph.add_node();
            graph.set_tags(idNode, typeList);
            objectList.clear();
            for(auto &x: graph.get_tags(idNode))
                std::cout<<x<<", ";
            std::cout<<std::endl;
            state =  State_machine::State::SEARCHING;
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
    
    qInfo()<<__FUNCTION__<<QString::fromUtf8( this->oldDoorRoom->getTypeObject().c_str()) ;     
    robot->rotate(0.2);
    
}

void State_machine::approach_state( std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    Eigen::Vector3f cord = robot->get_robot_target_coordinates(target.get_target_coordinates());
    qInfo()<<__FUNCTION__<< robot->has_target()<< QString::fromUtf8(target.getTypeObject().c_str()) <<cord.x()<<cord.y()<<cord.z();

    for(auto &object : objects) {
        if(target.sameType(object))
        {
            std::cout<<target.getTypeObject()<< object.getTypeObject()<<std::endl;
                        
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
    robot->advance(500);

}