//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"
GenericObject::GenericObject(const RoboCompYoloObjects::TBox &box)
{
    object = box;
    type_objet = box.id
}

GenericObject::GenericObject(const Door_detector::Door &door)
{
    RoboCompYoloObjects::TBox box;
    box.x= door.punto_medio.x();
    box.y= door.punto_medio.y();
    object = box;
    type_objet = "Door"
}

RoboCompYoloObjects::TBox GenericObject::getObject()
{
    return object;
}


string GenericObject::getTypeObjet()
{
    return type_objet;
}