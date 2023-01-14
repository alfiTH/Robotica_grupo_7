#include "generic_object.h"

class Graph
{
   public:
        Graph();
        int add_node();
        int add_node(int node_dest);
        void add_edge(int n1, int n2);
        void set_tags(int id, const  std::set<std::string> objects);
        std::set<std::string> get_tags(int id);

   private:
   
        struct Node
        {
            Node(int id_) : id(id_){};
            int id;
            std::set<std::string> objects;
        };

        struct Edge
        {
            Edge(int n1_, int n2_) : n1(n1_), n2(n2_){};
            int n1, n2;
        };
        int id_counter;
        std::map<int, Node> nodes;
        std::map<std::pair<int, int>,  Edge> edges;

};
