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

    private:
    RoboCompYoloObjects::TBox object;
};


#endif //FORCEFIELD_GENERICOBJECT_H
