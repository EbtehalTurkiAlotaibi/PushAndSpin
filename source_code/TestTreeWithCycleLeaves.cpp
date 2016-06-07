#include "iostream"
#include <vector>
#include "Agent.h"
#include <stdlib.h>

using namespace std;


void selectAgents (std::vector<Agent>& agents,int numAgents,int nodes_num) {
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

void createRandomTWCL (int nodesNum, int bridgLength, int freeNodes, std::vector<std::pair <int,int> >& Edges, std::vector <Agent>& agents,DataRow& dataRow)  {
    srand (time(NULL));
       int min =-1, max=-1;
       int counter=0;
    //------
    // C0
    min = 3;
    max = nodesNum-bridgLength;
    int start = counter;
    int C0 = min + (rand() % (int)(max - min + 1));
    for (int i=0; i<C0; i++) {
        Edges.push_back(std::make_pair(counter,counter+1));
        counter++;
    }
        // Backedge
        Edges.push_back(std::make_pair(counter,start));


    // Bridge
    for (int i=0; i<bridgLength; i++) {
        Edges.push_back(std::make_pair(counter,counter+1));
        counter++;
    }

    // C1
    start = counter;
    int C1 = nodesNum-bridgLength-C0;
    for (int i=0; i<C1; i++) {
        Edges.push_back (std::make_pair(counter,counter+1));
        counter++;
    }
        // Backedge
        Edges.push_back (std::make_pair(counter,start));

    // C
    /*int rest = nodesNum-bridgLength-C0 -C1;
    while (rest>3) {
        min = 3;
        max = nodesNum-bridgLength-C0-C1-1;
        int C =  min + (rand() % (int)(max - min + 1));
        start = counter;

        min = 0;
        max = counter-1;
        int linker =   min + (rand() % (int)(max - min + 1));
        Edges.push_back (std::make_pair(linker,counter++));
        Edges.push_back (std::make_pair(counter,counter++));
        for (int i=0; i<C; i++) {
            Edges.push_back (std::make_pair(counter,counter+1));
            counter++;
        }
        rest = rest - C - 1;
    }

    while (counter<nodesNum)
                Edges.push_back (std::make_pair(counter,counter++));
    */

    // agents
        int numAgents = nodesNum - freeNodes;
        selectAgents (agents, numAgents,nodesNum);

        dataRow.nodesNumber = nodesNum;
        dataRow.agentsNumber = agents.size();
        dataRow.freesNumber = nodesNum -agents.size();
        dataRow.graphName = "TWCL";
        dataRow.bridgeLength = bridgLength;
}

