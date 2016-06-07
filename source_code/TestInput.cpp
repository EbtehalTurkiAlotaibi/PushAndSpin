
string& createGraph(int node_number, std::vector <std::pair <int, int> >& Edges,string& graphName) {
 ofstream file;
 std::stringstream convert;
 convert <<  graphName;
 char* fileName_  = &(convert.str())[0];
 std::cout<<"Testing UI ="<<fileName_<<"\n";

 file.open (fileName_);
 // Set file header
 file <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      <<  "\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"> "
      <<  "\n<key id=\"key0\" for=\"node\" attr.name=\"Coordinate\" attr.type=\"string\" /> "
      <<  "\n<key id=\"key1\" for=\"edge\" attr.name=\"Weight\" attr.type=\"double\" /> "
      <<  "\n<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\"> \n";

 // Set nodes
 for (int i=0; i<node_number; i++)
     file << "<node id=\"n" << i << "\"> "
          << "\n<data key=\"key0\">" << i << ".0 0.0 0.0</data> "
          << "\n</node> \n";

 // Set edges
 int counter=0;
 int edge=0, nodes_ptr=0, start = nodes_ptr;
 for (int e=0; e<Edges.size(); e++)
             file << "<edge id=\"e"<< e <<"\" source=\"n"<< Edges[e].first <<"\" target=\"n"<< Edges[e].second <<"\"> "
                  << "\n<data key=\"key1\">1</data>"
                  << "\n</edge> \n";

file <<"  </graph> \n </graphml>";
file.close();
return graphName;
}

string& createInstance (int agent_number, std::vector<Agent> agents,string& graph_name,string&  instanceName)  {
    ofstream file;
    std::stringstream convert;
    convert <<  instanceName;
    char* fileName_  = &(convert.str())[0];
    std::cout<<"Testing UI ="<<fileName_<<"\n";

    file.open (fileName_);
    file << "graph =" <<graph_name <<"\n" ;
    for (int i=0; i<agents.size(); i++) {
        file << "agent: start = "<<agents[i].getStart()<<", goal = "<<agents[i].getGoal()<<"\n";
    }
    file.close();
    return instanceName;
}
