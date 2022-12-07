#include "door_detector.h"

std::vector<Door_detector::Door> Door_detector::detector_puertas(vector<Eigen::Vector2f> &line){

    //usamos la derivada de curret line
    float umbral = 1000;
    //std::vector<> puertas; 
    //auto filtrado = std::norm(cppitertools::sliding_window(current_line))
     std::vector <float> derivada(line.size()-1);
    for (const auto &&[i, p]: line|iter::sliding_window(2)|iter::enumerate){
        float resta = p[1].norm() - p[0].norm();
        //
        //derivada.emplace_back(); 1 alternativa
        derivada[i]=resta;
        //
    }

    std::vector <std::TuplePrx<int, bool>> peaks;
    for (auto &&[i, d] : derivada|iter::enumerate)
    {
        if(fabs(d) > umbral)
        {
            if (d > 0)
            {
                peaks.emplace_back(std::make_tuple(i,true));
            }
            else
            {
                peaks.emplace_back(std::make_tuple(i,false));
            }
        }
    }

    vector<Eigen::Vector2f> doors;
    for(auto &&p : peaks | iter::combinations_with_replacement(2))
    {
        auto &[p1, pos1] = p[0];
        auto &[p2, pos2] = p[1];

        auto v1 = line[p1];
        auto v2 = line[p2];

        if((pos1 and not pos2) or (pos2 and not pos1) and ((v1-v2).norm() < 1100) and ((v1-v2).norm() > 600))
        {
            doors.push_back((v1+v2)/2);
        }
    }
return doors;

}