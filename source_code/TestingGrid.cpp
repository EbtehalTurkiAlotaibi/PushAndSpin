#include "iostream"
#include <vector>
#include "Agent.h"
#include <stdlib.h>

using namespace std;


void selectAgents_ (std::vector<Agent>& agents,int numAgents,int nodes_num) {
    std::vector <int> starts, goals;
    int start, goal;
    srand (time(NULL));
    int max=nodes_num-1,min=0;
    for (int i=0; i<numAgents; i++) {
        do {

            start = min + (rand() % (int)(max - min + 1));
        } while (std::find(starts.begin(),starts.end(),start)!=starts.end());
        starts.push_back(start);

        do {

            goal = min + (rand() % (int)(max - min + 1));
        } while (std::find(goals.begin(),goals.end(),goal)!=goals.end());
        goals.push_back(goal);

     agents.push_back(Agent(i,start,goal));
    }

}
void CreateGrid (int R,int C,int free, std::vector <std::pair<int,int> >& Edges,std::vector<Agent>& agents,DataRow& dataRow)  {
    int counter=0;

    for (int row=0; row<R-1; row++)
        for (int col=0; col<C; col++) {
            if ((counter+1) % C != 0) {
                Edges.push_back(std::make_pair(counter, counter+1));
            }
            Edges.push_back(std::make_pair(counter,counter+C));
            counter++;
        }
    // Last row
    for (int col=0; col<C-1; col++) {
        Edges.push_back(std::make_pair(counter, counter+1));
        counter++;
    }
    /*
    for (int i=0; i<R-1; i++) {
        for (int j=0; j<C; j++) {
            if ((counter+1)%C!=0) // not the last column
                    Edges.push_back(std::make_pair(counter,counter+1));
            Edges.push_back(std::make_pair(counter,counter+C));
            counter++;
      }
    }

    for (int j=0; j<C; j++) {
        Edges.push_back(std::make_pair(counter,counter+1));
        counter++;
    }
*/
    int numAgents = (R*C) - free;
    selectAgents_ (agents, numAgents,(R*C));

    dataRow.nodesNumber = R*C;
    dataRow.EdgesNumber = Edges.size();
    dataRow.agentsNumber = agents.size();
    dataRow.freesNumber = (R*C) - agents.size();
    dataRow.gridHeight = R ;
    dataRow.gridWidth = C;


}
