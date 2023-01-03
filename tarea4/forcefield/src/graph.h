#include "generic_object.h"

class Graph
{
   public:
       Graph() = default;
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
       int add_node();
       int add_node(int node_dest);
       void add_edge(int n1, int n2);
       void add_tags(int id, const std::vector<GenericObject> &objects);

   private:
       int id_counter = 0;
       std::map<int, Node> nodes;
       std::map<std::pair<int, int>,  Edge> edges;

};
