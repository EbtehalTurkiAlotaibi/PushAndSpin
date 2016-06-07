#include <iostream>
#include <string>
#include "DataStructures.h"
using namespace std;

void writeStatisitcs (string& fileName,DataRow dataRow)  {
    std::stringstream convert;
    convert <<  fileName;
    char* fileName_  = &(convert.str())[0];
    std::cout<<"DataFile ="<<fileName_<<"\n";

    ofstream file;
    file.open (fileName_, std::ios_base::app);
    if (!file)
        std:cerr<<"ERROR in writeStatisitcs\n";

    // Write graph information
    file << "Solver=\t" <<dataRow.solverName<<"\t"
         << "Graph Name=\t" <<dataRow.graphName<<"\t"
         << "Nodes#\t"<< dataRow.nodesNumber<<"\t"
         <<"Edges#\t"<<dataRow.EdgesNumber<<"\t"
         <<"Agents#\t"<<dataRow.agentsNumber<<"\t"
         <<"Obstacles#\t"<<dataRow.obstaclesNumber<<"\t"
         <<"Free#\t" <<dataRow.freesNumber<<"\t"
         <<"MaxBrigeLength\t"<<dataRow.bridgeLength<<"\t"
         <<"Handles#\t"<<dataRow.handelsNumber<<"\t"
         <<"sizeOfC0=\t"<<dataRow.sizeOfC0<<"\t"
         <<"maxHandleLength=\t"<<dataRow.maxHandleLength<<"\t"
         <<"GridHeigth=\t"<<dataRow.gridHeight<<"\t"
         <<"Gridwidth=\t"<<dataRow.gridWidth<<"\t"
         << "Completeness_rate=\t" << dataRow.completenessRate << "\t"
         << "Execution_time=" << "\t"  <<dataRow.executionTime << "\t"
         << "Path_length=" << "\t" << dataRow.pathLength << "\t"
         << "Condense_length=" << "\t" << dataRow.condenseLength << "\t"
         << "Reverse Rate=" << "\t" << dataRow.reverseRate << "\n" ;

    file.close ();
}

float ComputeCompletenessRate (std::vector<Agent> BA, std::vector<Agent>& agents){
    float perc=0.0;
    for (int a=0; a<agents.size(); a++) {
        for (int b=0; b<BA.size(); b++)
            if (agents[b].getID() == agents[a].getID())
                if  (agents[b].getGoal() == agents[a].getCurrent()) {
                    //std::cout<<","<<agents[a].getGoal()<<"]";
                    perc++;
        }
        //std::cout<<"\n";
    }

    return (perc/agents.size());

}
