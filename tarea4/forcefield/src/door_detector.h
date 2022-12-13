#ifndef DOOR_DETECTOR_H
#define DOOR_DETECTOR_H

#include <Eigen/Dense>
#include <genericworker.h>
#include <timer/timer.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>

class Door_detector
{
    public:

        struct Door
        {
            Eigen::Vector2f punto_medio;
            Eigen::Vector2f punto1;
            Eigen::Vector2f punto2;
            float dist_pmedio;
            void draw(AbstractGraphicViewer *viewer)
            {
                static std::vector<QGraphicsItem *> items;
                for(const auto &i: items)
                    viewer->scene.removeItem(i);
                items.clear();

                auto item = viewer->scene.addEllipse(-100, -100, 200, 200, QPen(QColor("magenta"), 20));
                item->setPos(punto_medio.x(), punto_medio.y());
            }
        };

        Door_detector() = default;
        std::vector<Door> detector(const std::vector<Eigen::Vector2f> &line);
    private:

};
#endif