#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <string.h>
#include "strstream"
using namespace std;

struct DataRow {
    int nodesNumber;
    int EdgesNumber;
    int agentsNumber;
    int freesNumber;
    int obstaclesNumber;
    std::string graphName;
    string solverName;
    int bridgeLength;
    int handelsNumber;
    int sizeOfC0;
    int maxHandleLength;
    int gridHeight;
    int gridWidth;
    int pathLength;
    int condenseLength;
    double completenessRate;
    double executionTime;
    float reverseRate;
    DataRow () {
        nodesNumber=EdgesNumber=agentsNumber=freesNumber=obstaclesNumber=bridgeLength=handelsNumber
                =sizeOfC0=maxHandleLength=gridHeight=gridWidth=pathLength=condenseLength=completenessRate=executionTime=reverseRate=-1;
        graphName=solverName="";
    }
    std::string IntToString ( int number )
    {
    std::ostringstream oss;
    oss<< number;
    return oss.str();
    }
};
#endif // DATASTRUCTURES_H
