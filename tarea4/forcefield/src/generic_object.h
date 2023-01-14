//
// Created by alumno on 14/12/22.
//

#ifndef FORCEFIELD_GENERICOBJECT_H
#define FORCEFIELD_GENERICOBJECT_H
#include <genericworker.h>
#include "door_detector.h"



class GenericObject {

    public:
    GenericObject();
    GenericObject(const RoboCompYoloObjects::TBox &box);
    GenericObject(const Door_detector::Door &door);
    RoboCompYoloObjects::TBox getObject();
    string getTypeObject();
    bool setRoom(int room);
    bool sameType(GenericObject object);
    Eigen::Vector3f get_target_coordinates();

    private:
    const std::vector<string> yoloName {"person", "bicycle", "car", "motorcycle", "airplane",
"bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter",
"bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
"backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", 
"kite","baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
"wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
"broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed",
"dining table", "toilet", "tv", "laptop", "mouse", "remote",  "keyboard", "cell phone", "microwave", 
"oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", 
"hair drier", "toothbrush"};

    //std::map<int,string> dic_object ={{},{},{}}
    RoboCompYoloObjects::TBox object;
    string type_object;
};


#endif //FORCEFIELD_GENERICOBJECT_H
