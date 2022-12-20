//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"
GenericObject::GenericObject(const RoboCompYoloObjects::TBox &box)
{
    object = box;
}

GenericObject::GenericObject(const Door_detector::Door &door)
{
    RoboCompYoloObjects::TBox box;
    box.x= door.punto_medio.x();
    box.y= door.punto_medio.y();
    object = box;
}

RoboCompYoloObjects::TBox GenericObject::getObject()
{
    return object;
}

