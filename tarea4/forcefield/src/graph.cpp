#include "graph.h"

int Graph::add_node()
{
    int old_id;

    if (this->nodes.contains(0)) {
         // found
        old_id = 0;
        for(auto &&n:this->nodes)
        {
            if (n.id - old_id >1)
                {
                    add_node(old_id+1);
                } 
            else
            {
                old_id = n.id;
            }
        }
        add_node(old_id+1);
    } else {
       // not found
        add_node(0);
    }
    return 0 
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
        return 0
    }
}

void Graph::add_edge(int n1, int n2)
{

    std::pair<int, int> edge_key =  make_pair(n1, n2);
    if (this->edge.contains(edge_key)) {
        // found
        return -1;
    }else {
        // not found
        Graph::Edge edge_value(n1, n2);
        this->edges.insert(edge_key, edge_value);
        return 0;
    }
}

void Graph::add_tags(int id, const std::vector<GenericObject> &objects)
{
    //Si no existe nodo lo creamos
    if (!this->nodes.contains(id))
        add_node(id);
    
    //Optenemos el nodo
    Node nod = this->nodes.at(id);

    //Incorporamos la lista de objetos
    for(auto &&obj:objects)
        if (!nod.objects.contains(obj.getTypeObjet()))
            nod.objects.insert(obj.getTypeObjet());
}


void Graph::draw(AbstractGraphicViewer *viewer)
{


}
