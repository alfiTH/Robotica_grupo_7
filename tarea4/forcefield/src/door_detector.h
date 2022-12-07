#ifndef DOOR_DETECTOR_H
#define DOOR_DETECTOR_H

#include <Eigen/Dense>

class Door_detector
{
    public:

        struct Door
        {
            Eigen::Vector2f punto_medio;
            Eigen::Vector2f punto1;
            Eigen::Vector2f punto2;   
            float dist_pmedio;         
        };

        Door_detector();
        ~Door_detector();
        std::vector<Door> detector(const std::vector<Eigen::Vector2f> &line);

    
        
    private:
            
};
#endif