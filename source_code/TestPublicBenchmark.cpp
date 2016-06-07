#include "DataStructures.h"
bool doubleEdge (std::vector<std::pair<int,int> > edge, int first_, int second_){
    for (int i=0; i<edge.size(); i++) {
        if (edge[i].first == first_ && edge[i].second == second_)
            return false;
        if (edge[i].second == first_ && edge[i].first == second_)
            return false;
    }
    return true;
}

void PublicBenchmark(int& node_number,std::vector <std::pair <int, int> >& Edges_,string& mapName,DataRow& dataRow)
{

     string sourcePath="/home/ubuntu/Alhamdulellah/map_folder/"; // <--- located here

     vector <string> mapFileArray;
     // load all map files in the array
     mapFileArray.push_back(mapName);

     // mapFileArray contains all map files required ,, it is ready to import and read


       //***************************************** For Each Map **********************************************/
       // *****************************************************************************************************/

         // get the map name note: the map name included some of path directory ex: path/map name
           string fileName=mapFileArray[0];
         //read the occupancy grid map
           OccupancyGridMap* OGM= new OccupancyGridMap();
           OGM->importMapLayout(sourcePath, fileName.c_str());

            // name of map convert int to const char *

            // Create suitable Datastructure
              // 1- Graph
              std::vector <std::pair <int, int> >Edges;
              char **mapLayout = OGM->getMapLayout();
              int counter=0;
              int H=OGM->getHeight();
              int W=OGM->getWidth();
              std::vector <int> obstacles;
              // Construct the graph for the instance
              // 1- write matrix into file of grid graph
              // 2- read graph from that file
              // 3- depth first search the graph and store in new one

              //////////////////////////////////////////////////////////////////////
              // 1- write matrix into file of grid graph
              for (int i=0; i<H-1; i++) {
                  for (int j=0; j<W; j++) {
                      std::cout<<mapLayout[i][j]<<",";
                      if (mapLayout[i][j]!='@')
                           obstacles.push_back(counter);
                      if ((counter+1)%W!=0) {// not the last column
                              Edges.push_back(std::make_pair(counter,counter+1));
                      }
                      Edges.push_back(std::make_pair(counter,counter+W));
                      counter++;
                }
                  std::cout<<"\n";
                  std::cout<<"OK1\n\n\n\n\n";
              }

              std::cout<<"H="<<H<<", W="<<W<<"\n";

              for (int j=0; j<W; j++) {
                  if (mapLayout[H-1][j]!='@')
                    obstacles.push_back(counter);
                  std::cout<<mapLayout[H-1][j]<<",";
                  Edges.push_back(std::make_pair(counter,counter+1));
                  counter++;
                  std::cout<<"OK2\n\n\n\n\n";
              }



              std::cout<<"Out\n\n\n\n\n";

              string graphName = "/home/ubuntu/Alhamdulellah/Demos/PublicProblemTest/Test.gml";

              std::cout<<"OK2\n\n\n\n\n";

              string graphName_  = createGraph(counter,Edges,graphName) ;
              Edges.clear();
              // 2- read graph from that file
              Graph graph;
              graph.readGraphML(graphName_.c_str());
              // 3- breadth first search the graph and store in new one
              std::deque<int> nodes;
              for (int i=0; i<graph.numNodes(); i++) {
                graph.set_type(i,"B");
                graph.set_counter(i,-1);
              }

              counter=0;

              int root = 0;
              while (std::find(obstacles.begin(),obstacles.end(),root)!=obstacles.end())
                  root = root+1;

              //std::cout<<"root="<<root<<"\n";
              graph.set_type(root,"C");
              graph.set_counter(root,counter++);
              nodes.push_back(root);


              while (!nodes.empty()) {
                  int u = nodes.front();
                  nodes.pop_front();
                  //std::cout<<"It is "<<u<<"'s turn\n";
                  //std::cout<<"It counter="<<graph.get_counter(u)<<"\n";
                  std::vector<unsigned int> nbrs;
                  graph.getNeighbors (u, nbrs);
                  int parent = u;
                  for (int v=0; v<nbrs.size(); v++) {
                      //std::cout<<"node#"<<nbrs[v]<<"type is "<<graph.get_type(nbrs[v])<<"\n";
                          if (std::find(obstacles.begin(),obstacles.end(),nbrs[v])==obstacles.end()){
                              if (graph.get_counter(nbrs[v])==-1)
                                graph.set_counter(nbrs[v],counter++);
                              if (doubleEdge(Edges,graph.get_counter(u),graph.get_counter(nbrs[v]))){
                                Edges.push_back(std::make_pair(graph.get_counter(u),graph.get_counter(nbrs[v])));
                                //std::cout<<"Edge is added between "<<u<<","<<nbrs[v]<<" ("<<graph.get_counter(u)<<","<<graph.get_counter(nbrs[v])<<")\n";
                            }
                              if (graph.get_type(nbrs[v])=="B"){
                                graph.set_type(nbrs[v],"C");
                                nodes.push_back(nbrs[v]);

                          }
                      }
                  }
              }
              std::cout<<"Edges\n";
              for (int i=0; i<Edges.size(); i++)
                std::cout<<Edges[i].first<<"-->"<<Edges[i].second<<"\n";

              node_number = counter;
              Edges_=Edges;


              dataRow.obstaclesNumber = obstacles.size();
              dataRow.bridgeLength = -1;
              dataRow.EdgesNumber = Edges.size();

              dataRow.graphName = mapName;
              dataRow.handelsNumber = dataRow.maxHandleLength = dataRow.sizeOfC0 = -1;

    }




void selectRandomAgents (std::vector<Agent>& agents_,int &agentSize,DataRow& dataRow,int nodeNumber) {

    // 2- Agents
    int counter = nodeNumber;
    std::vector <int> starts, goals;
    int start, goal;
    std::vector<Agent> agents;
    srand (time(NULL));
    int max=counter-1,min=1;
    for (int i=0; i<agentSize; i++) {
        do {
            start = min + (rand() % (int)(max - min + 1));
        } while (std::find(starts.begin(),starts.end(),start)!=starts.end());
        starts.push_back(start);

        do {

            goal = min + (rand() % (int)(max - min + 1));
        } while (std::find(goals.begin(),goals.end(),goal)!=goals.end());
        goals.push_back(goal);

        Agent temp (i,start,goal);
        agents.push_back(temp);
   }



    agents_=agents;
    dataRow.agentsNumber = agents.size();
    dataRow.freesNumber = counter - agents.size();

}
