//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"
GenericObject::GenericObject(const RoboCompYoloObjects::TBox &box)
{
    this->object = box;
    this->type_object = box.id;
}

GenericObject::GenericObject(const Door_detector::Door &door)
{
    RoboCompYoloObjects::TBox box;
    box.x= door.punto_medio.x();
    box.y= door.punto_medio.y();
    this->object = box;
    this->type_object = "Door";
}

bool GenericObject::setRoom(int room)
{
    if(type_object == "Door")
    {
        this->type_object = type_object + std::to_string(room);
        return true;
    }
    return false;
}


RoboCompYoloObjects::TBox GenericObject::getObject()
{
    return this->object;
}


string GenericObject::getTypeObject()
{
    return this->type_object;
}

bool GenericObject::sameType(GenericObject object)
{
    return this->type_object.compare(object.getTypeObject());
}