//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"

GenericObject::GenericObject()
{
    std::cout << "GENERIC VOID OBJECT CREATE"<< endl;
}

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
    if(type_object == "Door")
        return type_object;
    else
        return yoloName.at((int)this->type_object.at(0));
}

bool GenericObject::sameType(GenericObject object)
{
    return this->type_object.compare(object.getTypeObject());
}

Eigen::Vector3f GenericObject::get_target_coordinates()
{
    return Eigen::Vector3f{this->object.x, this->object.y, this->object.z};
}
