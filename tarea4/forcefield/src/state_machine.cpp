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
    //std::cout<<"Distanca a "<< a<< "Distancia b"<<b<<std::endl;
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
        case State_machine::State::TRANSITION_ROOM:
        {
             transition_room_state(objects);
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
                    [obj = robot->get_current_target(), robot=this->robot, umbral_mismo_obj = 500](GenericObject &s) {
                    return equal_object(obj, s, umbral_mismo_obj, robot); }))
            {
                robot->set_has_target(true);
                state =  State_machine::State::APPROACHING;
            }
            break;
        }
        case State_machine::State::APPROACHING:
        {
            if (robot->get_distance_to_target()<500)
            {
                //robot->set_has_target(false);
                state =  State_machine::State::TRANSITION_ROOM;
                //return Eigen::Vector3f{0.f, 0.f, 0.f};
                //robot->rotate();
            }
            break;
        }

        case State_machine::State::TRANSITION_ROOM:
        {
            static std::chrono::time_point<std::chrono::system_clock> start;
            if (!firstTime)
            {
                start = std::chrono::system_clock::now();
                firstTime = true;
            }
            else{
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<float,std::milli> duration = end - start;
                qInfo()<< "time"<<duration.count();
                if (duration.count() > 2000 )
                 {
                     robot->stop();
                     firstTime = false;
                     state =  State_machine::State::SCAN;

                 }
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
    static int idNode;
    static int posOldDoorRoom;
    robot->rotate(0.2);
    float umbral_mismo_obj = 750;

    for(auto &object : objects)
    {
        if (robot->get_robot_target_coordinates(object.get_target_coordinates()).norm() < 2100 or object.getTypeObject() == "Door")
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
    
    if (objectList.size()>3)
    {
        //Ecncontramos el primer objeto identificado
        if(objects.end() == std::find_if(objects.begin(), objects.end(),
                                [obj = &objectList.at(0), robot=this->robot, umbral_mismo_obj](GenericObject &s) { 
                                return equal_object(*obj, s, umbral_mismo_obj, robot); }))
        {
            qInfo()<< "Procesando de nodo";
            std::set<string> typeList;
            bool firstDoor = false;

            for(auto &obj : objectList)
            {
                string nameObject;
                nameObject = obj.getTypeObject();
                if (obj.getTypeObject() == "Door" ) {
                    // puerta de ingreso
                    if (obj.get_target_coordinates().norm() < 400 and obj.get_target_coordinates().norm() > 200) {
                        qInfo()<<"(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((";
                        for(auto &x: graph.get_tags(idNode))
                            std::cout<<x<<", ";
                        std::cout<<std::endl;

                        auto tag = graph.get_tags(idNode);
                        tag.erase("DoorFutureRoom");
                        tag.insert("Door" + to_string(idNode+1));
                        graph.set_tags(idNode, tag);
                        for(auto &x: graph.get_tags(idNode))
                            std::cout<<x<<", ";
                        std::cout<<std::endl;
                        nameObject = nameObject + to_string(idNode);
                    }
                    //posible puerta de salida
                    else if (!firstDoor)
                    {
                        posOldDoorRoom = typeList.size() - 1;
                        robot->set_current_target(obj);
                        robot->set_has_target(false);
                        //GenericObject *auxObj = new GenericObject(obj);
                        //delete this->oldDoorRoom;
                        //this->oldDoorRoom = auxObj;
                        firstDoor = true;
                        nameObject = nameObject + "FutureRoom";
                    }
                }
                typeList.emplace(nameObject);
            }
            if (!firstDoor){
                auto puerta = std::find_if(objects.begin(), objects.end(),
                             [ robot=this->robot, umbral_mismo_obj](GenericObject &s) {
                                 return s.getTypeObject() == "Door"; });
                robot->set_current_target(*puerta);
                robot->set_has_target(false);
                qInfo()<<"nope";
            }
            for(auto &x: typeList)
                std::cout<<x<<", ";
            std::cout<<std::endl;
            qInfo()<<"Insertando Nodo";
            idNode= graph.add_node();
            graph.set_tags(idNode, typeList);
            for(auto &x: typeList)
                std::cout<<x<<", ";
            std::cout<<std::endl;
            qInfo()<<"antes clear";
            objectList.clear();
            for(auto &x: typeList)
                std::cout<<x<<", ";
            std::cout<<std::endl;
            for(auto &x: graph.get_tags(idNode))
                std::cout<<x<<", ";
            std::cout<<std::endl;
            qInfo()<<"Nodo insertado";
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
    
    qInfo()<<__FUNCTION__<<QString::fromUtf8( robot->get_current_target().getTypeObject().c_str()) ;
    robot->rotate(0.5);
    
}

void State_machine::approach_state( std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    Eigen::Vector3f cord = robot->get_robot_target_coordinates(target.get_target_coordinates());
    qInfo()<<__FUNCTION__<< robot->has_target()<< QString::fromUtf8(target.getTypeObject().c_str()) <<cord.x()<<cord.y()<<cord.z()<< cord.norm();

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

void State_machine::transition_room_state( std::vector<GenericObject> &objects)
{
    qInfo()<<__FUNCTION__;
    //robot->rotate(0);
    //robot->advance(500);
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
    robot->stop();

}