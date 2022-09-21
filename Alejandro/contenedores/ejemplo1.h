#ifndef ejemplo1_H
#define ejemplo1_H

#include <vector>
#include <random>
#include <QDebug>
#include <ranges>
#include <chrono>
#include <omp.h>
#include <iostream>
#include <unordered_map>


class Contenedores
{


    private:
        struct Node
        {
            uint id;
            std::vector<uint> links;
        };

        std::unordered_map<uint,Node> graph;
        std::vector<int> vec;
        std::list<int> lis;
        std::map<std::pair<int,int> , int> mymap;
        std::tuple<int, std::string, float, float> mytuple;
    public:
        Contenedores();
        void test_vector();
        void test_vector_omp();
        void test_map();
        void test_list();
        void test_tuple();
        void test_graph();


};

#endif 
