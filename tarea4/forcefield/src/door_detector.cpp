#include "door_detector.h"
#include <cppitertools/enumerate.hpp>
#include <cppitertools/sliding_window.hpp>
#include <cppitertools/combinations.hpp>

std::vector<Door_detector::Door> Door_detector::detector(const std::vector<Eigen::Vector2f> &line)
{
    //std::vector<> puertas;
    //auto filtrado = std::norm(cppitertools::sliding_window(current_line))
    std::vector <float> derivada(line.size()-1);
    for (const auto &&[i, p]: line|iter::sliding_window(2)|iter::enumerate)
        derivada[i]=p[1].norm() - p[0].norm();

    //usamos la derivada de curret line
    const float umbral = 500;
    std::vector <std::tuple<int, bool>> peaks;
    for (auto &&[i, d] : derivada | iter::enumerate)
        if(d > umbral)
            peaks.emplace_back(std::make_tuple(i,true));
        else if(d < -umbral)
            peaks.emplace_back(std::make_tuple(i+1,false));

    std::vector<Door> doors;
    for(auto &&p : peaks | iter::combinations(2))
    {
        auto &[p1, pos1] = p[0];
        auto &[p2, pos2] = p[1];
        auto v1 = line[p1];
        auto v2 = line[p2];

        if(((pos1 and not pos2) or (pos2 and not pos1)) and
            (v1-v2).norm() < 1100 and (v1-v2).norm() > 600
            and v1.y()>200 and v2.y()>200)
        {
            Door door_aux;
            door_aux.punto_medio= ((v1+v2)/2);
            door_aux.punto1=v1;
            door_aux.punto2=v2;
            //qInfo() << __FUNCTION__ << v1.y() << v1.x() << v2.x() << v2.y();
            doors.emplace_back(std::move(door_aux));
        }
        //qInfo() << __FUNCTION__ << "-----------";
    }
return doors;
}

void Door_detector::draw(AbstractGraphicViewer *viewer, vector<Door_detector::Door> doors)
{
    static std::vector<QGraphicsItem *> items;
    for(const auto &i: items)
    {
        viewer->scene.removeItem(i);
        delete i;
    }
    items.clear();

    for(Door_detector::Door &d : doors)
    {
        //qInfo() << __FUNCTION__ << "asdfas";
        auto item = viewer->scene.addEllipse(-100, -100, 200, 200, QPen(QColor("magenta"), 20));
        item->setPos(d.punto_medio.x(), d.punto_medio.y());
        items.push_back(item);
        item = viewer->scene.addEllipse(-100, -100, 200, 200, QPen(QColor("green"), 20));
        item->setPos(d.punto1.x(), d.punto1.y());
        items.push_back(item);
        item = viewer->scene.addEllipse(-100, -100, 200, 200, QPen(QColor("green"), 20));
        item->setPos(d.punto2.x(), d.punto2.y());
        items.push_back(item);
    }
    qInfo() << "-----------";
}