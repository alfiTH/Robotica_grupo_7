//
// Created by alumno on 14/12/22.
//

#ifndef FORCEFIELD_GENERICOBJECT_H
#define FORCEFIELD_GENERICOBJECT_H
#include <genericworker.h>
#include "door_detector.h"

class GenericObject {

    public:
    GenericObject(const RoboCompYoloObjects::TBox &box);
    GenericObject(const Door_detector::Door &door);
    RoboCompYoloObjects::TBox getObject();
    string getTypeObject();
    bool setRoom(int room);
    bool sameType(GenericObject object);
    Eigen::Vector3f get_target_coordinates();

    private:
    //std::map<int,string> dic_object ={{},{},{}}
    RoboCompYoloObjects::TBox object;
    string type_object;
};


#endif //FORCEFIELD_GENERICOBJECT_H
