#ifndef ejemplo1_H
#define ejemplo1_H

#include <vector>
#include <random>
#include <QDebug>
#include <ranges>
#include <chrono>
#include <omp.h>

class Contenedores
{
    private:
        std::vector<int> vec;
        std::vector<int> vec2;
    public:
        Contenedores();
        void sort_vector();

};

#endif 
