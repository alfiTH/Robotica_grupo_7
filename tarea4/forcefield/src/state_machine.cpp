#include "state_machine.h"
#include <cppitertools/range.hpp>
////////////////////ESTADOS////////////////////////////////////////////

 State_machine::State_machine(){
    this->state = State::SCAN;
    std::cout << "STATE MACHINE CREATE"<< endl;
 }

State_machine::~State_machine(){
    show_graph();
}


void State_machine::initialize(rc::Robot *robot){
    this->robot = robot;
}
void State_machine::show_graph(){
    graph.show_graph();
}

//Compara la distancia entre el elemnto a y be con un +-umbral
bool in_range(float a, float b, float umbral)
{

    //std::cout<<"Distanca a "<< a<< "Distancia b"<<b<<std::endl;
    return (a < b + umbral) && (a > b - umbral);
}

//R etorna si son igual en tipo y parecidos en distancia
bool equal_object(GenericObject object1, GenericObject object2, float tolerance, rc::Robot *robot)
{
    return object1.getTypeObject() == object2.getTypeObject() && in_range(
        robot->get_robot_target_coordinates(object2.get_target_coordinates()).norm(),
        robot->get_robot_target_coordinates(
        object1.get_target_coordinates()).norm(), tolerance);
}

//Acciones de la maquina de estados
void State_machine::state_machine_action(std::vector<GenericObject> &objects)
{
    switch (state)
    {
        case State_machine::State::SCAN:
        {
            scan_state(objects);
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

//Condiciones de la maquina de estados
void State_machine::state_machine_condition(std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    switch (state)
    {
        case State_machine::State::SCAN:
        {
            break;
        }
        case State_machine::State::SEARCHING:
        {
            //Si encontramos el objeto target vamos a por el pon el APPROACHING
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
            //ya hemos alcanzado el target
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
            //Se dara un tiempo en movimiento rar la transición entre habitaciones
            static std::chrono::time_point<std::chrono::system_clock> start;
            if (!firstTime)
            {
                start = std::chrono::system_clock::now();
                firstTime = true;
            }
            else{
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<float,std::milli> duration = end - start;
                if (duration.count() > 2200 )
                 {
                     robot->stop();
                     firstTime = false;
                     show_graph();
                     firstScan = false;
                     state =  State_machine::State::SCAN;

                 }
            }
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
    static std::vector<GenericObject> objectList; //Objetos reconocidos
    static int idNode;      //ID del nodo escrito anteriormente
    static bool direction;  //Direcion en caso de no ser el primer escaneo
    static bool  explored;  //Teminado el escaneo
    float velScan = 0.3;    //Velocidad de escaneo
    float umbral_mismo_obj = 750; // umbral para considerar un mismo objeto por distancia

    //En el primer escaneo sera de 360 gracos reconociendo el primer elemento de la lista
    if (firstScan)
    {
        robot->rotate(velScan);
        direction = false;
    }
    //Si no sera un escaneo de 180 grados
    else
    {
        static std::chrono::time_point<std::chrono::system_clock> start;
        if (direction)
            robot->rotate(velScan);
        else
            robot->rotate(-velScan);

        if (!firstTime2)
        {
            start = std::chrono::system_clock::now();
            firstTime2 = true;
            explored = false;
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> duration = end - start;
        if (duration.count() > (M_PI_2-0.2)/velScan && !direction )
        {
            start = std::chrono::system_clock::now();
            direction = true;
        }
        else if (duration.count() > (M_PI-0.2) /velScan && direction )
        {
            firstTime2 = false;
            direction = false;
            explored = true;

        }
    }

    //con rodos los objetos reconocidos insertamos los nuevos
    for(auto &object : objects)
    {
        if (robot->get_robot_target_coordinates(object.get_target_coordinates()).norm() < 2500 or
        (object.getTypeObject() == "Door" and robot->get_robot_target_coordinates(object.get_target_coordinates()).norm() < 3000))
        {
            //El objeto esta repetido?
            auto itObj = std::find_if(objectList.begin(), objectList.end(),
                                      [obj = &object, robot=this->robot, umbral_mismo_obj](GenericObject &s) { 
                                    return equal_object(*obj, s, umbral_mismo_obj, robot); });
            //el tipo no esta repetido lo añadimos
            if (itObj == objectList.end()) {
                objectList.push_back(object);
            }
        }
    }   

    //mostamos objetos guardados
    for (auto &x : objectList){
        std::cout << x.getTypeObject() << ", ";
    }
    std::cout<<std::endl;
    
    if (objectList.size()>3)
    {
        //Ecncontramos el primer objeto identificado
        auto exit = std::find_if(objects.begin(), objects.end(),
                                   [obj = &objectList.at(0), robot=this->robot, umbral_mismo_obj](GenericObject &s) {
                                       return equal_object(*obj, s, umbral_mismo_obj, robot); });
        //Orientamos el robot
        std::cout << "Orientamos" << explored<<std::endl;
        if((objects.end() != exit and fabs((exit->get_target_coordinates().x())<100) and firstScan) or explored)
        {
            qInfo()<< "Procesando de nodo";
            std::set<string> typeList;
            bool firstDoor = false;
            std::vector<int> nodos;
            for (int i = 0; i < graph.num_nodes();i++)
                nodos.push_back(i);

            for(auto &obj : objectList)
            {
                string nameObject = obj.getTypeObject();
                if (obj.getTypeObject() == "Door" ) {
                    // puerta de ingreso
                    if (obj.get_target_coordinates().norm() < 1250) {
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
                        robot->set_current_target(obj);
                        robot->set_has_target(false);
                        //GenericObject *auxObj = new GenericObject(obj);
                        //delete this->oldDoorRoom;
                        //this->oldDoorRoom = auxObj;
                        firstDoor = true;
                        nameObject = nameObject + "FutureRoom";
                    }
                }
                else{
                    while (true)
                    {
                        auto diferente = std::find_if(nodos.begin(), nodos.end(), [n = nameObject, g = &graph](auto p)
                            {auto tags = g->get_tags(p); return  std::find(tags.begin(), tags.end(), n) == tags.end(); });
                        if (diferente == nodos.end())
                            break;
                        else
                            nodos.erase(diferente, nodos.end());
                    }
                }
                typeList.emplace(nameObject);
            }
            if (!firstDoor){
                auto puerta = std::find_if(objects.begin(), objects.end(),
                             [ robot=this->robot, umbral_mismo_obj](GenericObject &s) {
                                 return s.getTypeObject() == "Door"; });
                if (puerta == objects.end())
                    std::cout << "No se encontraron puertas"<<std::endl;
                robot->set_current_target(*puerta);
                robot->set_has_target(false);
            }

            //misma sala
            if (nodos.end() != std::find_if(nodos.begin(), nodos.end(), [n = typeList.size(), g = &graph](auto p)
                {return  g->get_tags(p).size() == n; }))
            {
                state =  State_machine::State::WAITING;
            }
            //sala no identificada
            else
            {
                qInfo()<<"Insertando Nodo";
                idNode= graph.add_node();
                graph.set_tags(idNode, typeList);
                objectList.clear();
                auto aux = graph.get_tags(idNode);
                qInfo()<<"Objetos de la sala";
                for(auto &x: aux)
                    std::cout<<x<<", ";
                std::cout<<std::endl;
                qInfo()<<"Nodo insertado";
                state =  State_machine::State::SEARCHING;

            }

        }
    }
}

void State_machine::search_state( std::vector<GenericObject> &objects)
{
    
    qInfo()<<__FUNCTION__<<QString::fromUtf8( robot->get_current_target().getTypeObject().c_str()) ;
    robot->rotate(-0.5);
    
}

void State_machine::approach_state( std::vector<GenericObject> &objects)
{
    GenericObject target = robot->get_current_target();
    Eigen::Vector3f cord = robot->get_robot_target_coordinates(target.get_target_coordinates());
    qInfo()<<__FUNCTION__<< robot->has_target()<< QString::fromUtf8(target.getTypeObject().c_str()) <<cord.x()<<cord.y()<<cord.z()<< cord.norm();

    auto tagetAux = std::find_if(objects.begin(), objects.end(),
                             [obj = &target, robot=this->robot, umbral_mismo_obj = 1000](GenericObject &s) {
                                 return equal_object(*obj, s, umbral_mismo_obj, robot); });

    if(tagetAux != objects.end()){
        robot->set_current_target(*tagetAux);
    }


}

void State_machine::transition_room_state( std::vector<GenericObject> &objects)
{
    qInfo()<<__FUNCTION__;
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
    show_graph();
    sleep(15);


}