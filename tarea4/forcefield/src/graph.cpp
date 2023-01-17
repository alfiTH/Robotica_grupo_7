#include "graph.h"

Graph::Graph()
{
    id_counter = 0;
    std::cout << "GRAPH CREATE"<< endl;
}

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
        this->nodes.insert (std::pair<int,Node>(node_dest, nodo));
        id_counter = this->nodes.size();
        std::cout<< "Nodo añadido"<<std::endl;
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
        this->edges.insert(std::pair<std::pair<int, int>,  Edge>(edge_key, edge_value));
    }
}

void Graph::set_tags(int id, const std::set<std::string> objects)
{
    //Si no existe nodo lo creamos
    if (!this->nodes.contains(id))
        add_node(id);
    //Obtenemos el nodo
    Node nod = this->nodes.at(id);

    //Incorporamos la lista de objetos
    nod.objects = objects;
}

std::set<std::string> Graph::get_tags(int id)
{
    std::set<std::string> stringTipoObj;

    auto item = nodes.find(id);
    if (item != nodes.end()) {
             return item->second.objects;
    } else {
        cout << "Node does not exist!" << endl;
        return stringTipoObj;
    }

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
