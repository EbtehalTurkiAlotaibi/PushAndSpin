#include <iostream>
#include <string>
#include "PushAndSpin.h"
using namespace std;


void CreateGraphRecRecords (Graph &graph,Instance &config,string& Filename) {
    std::stringstream convert;
    convert <<  Filename;
    char* graphRec_  = &(convert.str())[0];
    ofstream myfile1 (graphRec_);
    if (!myfile1)
        std::cerr<<"ERROR in CreateGraphRecRecords\n";

    std::cout<<"GraphRec name ="<<graphRec_<<"\n";
    myfile1<<"id:1\nV=\n";
    // Writing nodes :
    for (int i=0; i<graph.numNodes() ; i++)  {
        myfile1<< "(" << (i+1) << ":0)[" ;
        bool flag = false;
        for (int j=0; j<config.numAgents() ; j++)
            if (config.getAgent(j).getStart () == i ){
                myfile1<< config.getAgent(j).getID () +1 << ":0:0]\n";

                flag = true ;
                break ;}
        if (!flag) { // no agent
            myfile1 << "-1:0:0]\n";
               }

    }
    // Writing edges :
    myfile1 << "E=\n";
    for (int i=0; i<graph.numNodes() ; i++)
        for (int j=i; j<graph.numNodes() ; j++)
            if (graph.edgeExists (i,j)) {
                myfile1 << "{" << (i+1) << "," << (j+1) << "} (0) \n";
            }
    // Writing modes :
    myfile1 << "MOD =\nM:TRANSITIVE\n";
    myfile1.close();
}


// Write each step of the plan, annotated with the agent id.
void writePlan(const PushAndSpin::Plan &simulation,
               string& Filename,
               int& path_len,
               int& condense_len,
               bool PAR,
               PushAndSpin::Condense& simulatnous_indexes)
{
    std::stringstream convert;
    convert <<  Filename;
    char* graphRec_  = &(convert.str())[0];

   ofstream temp (graphRec_,ios::app);
        std::cout<<"GraphRec name ="<<graphRec_<<"\n";

    if (!temp)
                std::cerr<<"ERROR in writePlan\n";
    temp << "Solution\n";

      // Writing Condense
      if (PAR) {
          int moves = 0 ;
          for (int a=0;a<simulatnous_indexes.size(); a++){ // for each step
          for (size_t i = 0; i < simulatnous_indexes[a].second.size (); i++) // moving horizontal
          {
              for (size_t j = 0; j < simulatnous_indexes[a].second[i].second.size()-1; ++j)
                      { // for each MOVE in the path of agent

                              temp <<  simulatnous_indexes[a].second[i].second[j]+ 1 << " ---> " <<  simulatnous_indexes[a].second[i].second[j+1] + 1
                      << "\t[0 ---> 0]" << "\t(" << moves << ")" << "\n";
                              //out<< "simulation[i].second.size() != 1) case !" ;
                              //out<<  simulation[i].second[j] << " ---> " << simulation[i].second[j+1]<< "\n";
                         //if ( simulatnous_indexes[a].second[i].second.size()>2 && j<simulatnous_indexes[a].second[i].second.size()-2)
                             // for sequential MOVE
                         //{ moves++; condense_len++;}
                             // for any MOVE
                         path_len++;
                      }

              }
          moves++;
          condense_len++;
          }
         temp <<  "Length:" << moves;
      }



      // Writing Paths
      else {
          int moves = 0 ;
          for (size_t i = 0; i < simulation.size (); i++)
          {
              if (simulation[i].second.size()== 1){
                  int k =-1;
                  for (k=i-1 ; k>=0 ; k--)
                      if (simulation[k].first==simulation[i].first)
                          break;

                  if (k==-1) { // No previous moves!
                  continue ;
                  }

                  else {

                  temp << simulation[k].second[simulation[k].second.size()-1] + 1
                          << " ---> " << simulation[i].second[0] + 1
          << "\t[0 ---> 0]" << "\t(" << moves++ << ")" << "\n";

                  //out<< "simulation[i].second.size()== 1) case !" ;
                  //out<< simulation[k].second[simulation[k].second.size()-1]<< " ---> " << simulation[i].second[0]<< "\n";
                  }

              }
              else
              for (size_t j = 0; j < simulation[i].second.size()-1; ++j)
                  {

                          temp << simulation[i].second[j] + 1 << " ---> " << simulation[i].second[j+1] + 1
                  << "\t[0 ---> 0]" << "\t(" << moves++ << ")" << "\n";
                          //out<< "simulation[i].second.size() != 1) case !" ;
                          //out<<  simulation[i].second[j] << " ---> " << simulation[i].second[j+1]<< "\n";

                  }
          }
         temp <<  "Length:" << moves;
         path_len = condense_len = moves;
      }


     temp.close();
}
