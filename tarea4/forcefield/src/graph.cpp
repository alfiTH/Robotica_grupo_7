#include "graph.h"

int Graph::add_node()
{
    int old_id;

    if (this->nodes.contains(0)) {
         // found
        old_id = 0;
        for(auto &&n:this->nodes)
        {
            if (n.first - old_id >1)
                {
                    add_node(old_id+1);
                } 
            else
            {
                old_id = n.first;
            }
        }
        add_node(old_id+1);
        return old_id+1;
    } else {
       // not found
        add_node(0);
        return 0;
    }
    
}
//int id_counter = 0;
//std::map<int, Node> nodes;
//std::map<std::pair<int, int>,  Edge> edges;


int Graph::add_node(int node_dest)
{
    if (this->nodes.contains(node_dest)) {
        // found
        return -1;
    } else {
        // not found
        Graph::Node nodo(node_dest);
        this->nodes.insert(node_dest, nodo);
        id_counter = this->nodes.count();
        return node_dest;
    }
}

void Graph::add_edge(int n1, int n2)
{
    std::pair<int, int> edge_key =  make_pair(n1, n2);
    if (!this->edges.contains(edge_key))
    {
        // not found
        Graph::Edge edge_value(n1, n2);
        this->edges.insert(edge_key, edge_value);
    }
}

void Graph::set_tag(int id, const std::vector<GenericObject> &objects)
{
    //Si no existe nodo lo creamos
    if (!this->nodes.contains(id))
        add_node(id);
    //Obtenemos el nodo
    Node nod = this->nodes.at(id);

    //Incorporamos la lista de objetos
    for(auto &&obj:objects)
        if (!nod.objects.contains(obj.getTypeObject()))
            nod.objects.insert(obj.getTypeObject());
}

std::set<std::string> Graph::get_tag(int id)
{
    std::set<std::string> stringTipoObj;
    for (const auto &i : nodes) {
        if(i.first == id)
        {
            stringTipoObj.insert(i.second);
        }
    }
    return stringTipoObj;
}

//void Graph::add_tags(int id, const std::vector<GenericObject> &objects)
//{
//    //Si no existe nodo lo creamos
//    if (!this->nodes.contains(id))
//        add_node(id);
//
//    //Optenemos el nodo
//    Node nod = this->nodes.at(id);
//
//    //Incorporamos la lista de objetos
//    for(auto &&obj:objects)
//        if (!nod.objects.contains(obj.getTypeObject()))
//            nod.objects.insert(obj.getTypeObject());
//}
