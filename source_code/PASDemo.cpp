/****************************************************************************
Copyright (c) 2013, Ryan Luna, Athanasios Krontiris, Kostas Bekris
All rights reserved.

The Software is available for download and use subject to the terms and
conditions of this License. Access or use of the Software constitutes
acceptance and agreement to the terms and conditions of this License.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and non-profit purposes, without fee,
and without a written agreement is hereby granted.  Permission to incorporate
this software into commercial products may be obtained by contacting the authors.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the names of the authors nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

// Author: Ryan Luna
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <math.h>
#include <vector>
#include <cstring>
#include <unistd.h>
#include "OccupancyGridMap.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include "PushAndSpin.h"
#include "PushAndSwap.h"
#include "Instance.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QApplication>
#include <QMessageBox>
#include <deque>
#include "ScenarioLoader.h"
#include "GraphRec.cpp"
#include "TestInput.cpp"
#include "testinguibibox.h"
#include "TestPublicBenchmark.cpp"
#include "Statistics.cpp"
#include "TestPushAndSwap.cpp"
#include "TestPushAndSpin.cpp"
#include "mainwindow.h"
#include "testuserinput.h"
#include "testingcriticalproblems.h"
#include "testingpublicbenchmark.h"
#include "testingrandombiconnected.h"
#include "testingrandomtwcl.h"
#include "TestTreeWithCycleLeaves.cpp"
#include "testrandomgrid.h"
#include "bibox.cpp"
#include "TestingGrid.cpp"

using namespace std;

int agent_cout = 0 ;
int N,A, E;
std::string name;
double exexc_time ;
int path_len=0, condense_len=0;
float completed_agents;
bool PAR;


// This function generates two text files formatted in GraphRec format.. the first file is for Unsmoothed results
// and the other one is for smoothed results

// Read in a problem instance and solve the problem using PushAndSwap


int main(int argc, char *argv[]) {
    std::cout<<"HH\n";

    QApplication a(argc, argv);
    MainWindow main_window;


    if(main_window.exec() == QDialog::Accepted) {
    int testing_plan_index = main_window.testing_plan_number;
    int solver_index = main_window.solver;
    DataRow dataRow;
        if (testing_plan_index==1)  {
           if (solver_index<3)  {
               TestUserInput w  ;
            if(w.exec() == QDialog::Accepted) {
                int node_number = w.node_number;
                std::vector <std::pair <int, int> >Edges = w.Edges;
                std::vector<Agent> agents = w.agents;
                int solver = solver_index;
                std::cout<<"node#"<<node_number<<", solver="<<solver<<"\n";
                string graphName = "/home/ubuntu/Alhamdulellah/Demos/UserInputTest/TestingUI_N"+IntToString(node_number)+"_E"+IntToString(Edges.size())+"_A"+IntToString(agents.size())+".gml";
                string instanceName = "/home/ubuntu/Alhamdulellah/Demos/UserInputTest/TestingUI_N"+IntToString(node_number)+"_E"+IntToString(Edges.size())+"_A"+IntToString(agents.size())+".instance";
                string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/UserInputTestGraphs/" + IntToString(solver) + "/TestingUI_N"+IntToString(node_number)+"_E"+IntToString(Edges.size())+"_A"+IntToString(agents.size())+".txt";
                string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="UserInput";

                string graph = createGraph(node_number,Edges,graphName) ;
                string instance = createInstance(agents.size(),agents, graph,instanceName) ;

                if (solver==1) {// push and swap
                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }

                if (solver ==2) { // push and Spin
                   PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }
             }
            }//end if (solver<2)
           else {
               testingUIBibox w;
               if(w.exec() == QDialog::Accepted) {
                   int free=w.free;
                   int maxLength = w.maxHandle;
                   int handles = w.handle_number;
                   int c0 = w.c0;
                   string graphRec="/home/ubuntu/Alhamdulellah/Demos/UserInputTestGraphs/TestingUI,F="+IntToString(free)+",m="+IntToString(maxLength)+",L="+IntToString(handles)+",C0="+IntToString(c0)+".txt" ;
                   string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                   readBibox (c0, handles,maxLength,free,graphRec,dataFile,dataRow);
               }
           } // end else if (solver<3)

      }// end testing_plan_index==1

    if (testing_plan_index==2) {
        TestingCriticalProblems w;
        if (w.exec() == QDialog::Accepted) {
            int problem_index = w.problem_index;
            int solver = solver_index;
            string solver_method;
            switch (problem_index) {
            case 1: { // tree
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/tree.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/tree.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/"+ IntToString(solver) + "/tree.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="tree";

                if (solver==1) {// push and swap
                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;
            }
            case 2: { // tunnel
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/tunnel.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/tunnel.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/tunnel.txt";
                string dataFile= "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="tunnel";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;
            }
            case 3: { // string
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/string.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/string.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/string.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="string";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }
            case 4: { // corners
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/corners.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/corners.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/corners.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="corners";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }
            case 5: { // connector
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/connector.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/connector.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/connector.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="connector";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }
            case 6: { // loop chain
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/loop.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/loop.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/loop.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="loop chain";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }

            case 7: { // biconnected
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/biconnected.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/biconnected.instance";
                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/biconnected.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="biconnected";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;
            }
            case 8: { //TWCL
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/TWCL.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/TWCL.instance";

                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/TWCL.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="TWCL";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;
            }
            case 9: {//grid
                string graph = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/grid.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTest/grid.instance";

                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/grid.txt";
                string dataFile= "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="grid";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }
            case 10: {//polygon
                string graph = "/home/ubuntu/Alhamdulellah/Demos/config/polygon.gml";
                string instance = "/home/ubuntu/Alhamdulellah/Demos/config/polygon.instance";

                string graphRec="/home/ubuntu/Alhamdulellah/Demos/CriticalProblemTestGraphs/" + IntToString(solver) + "/polygon.txt";
                string dataFile=  "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="polygon";

                if (solver==1) {// push and swap

                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Swap";
                }
                if (solver ==2) { // push and Spin
                     std::cout<<"GRAPHREC at main="<<graphRec<<"\n";
                    PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    solver_method = "Push And Spin";
                }
                if (solver ==3) { // push and Spin
                    /*std::vector<Bicycle> cycles =w.cycles ;
                    std::vector <BiEdge> BiEdges = w.BiEdges;
                    Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                    solver_method = "Bibox";*/
                }
                break;

            }
            }
        }// end if (w.exec() == QDialog::Accepted)

      } //end if (testing_plan_index==2)

    if (testing_plan_index==3){
        TestingPublicBenchmark w;
        if (w.exec() == QDialog::Accepted) {
            int solver = solver_index;
            int node_number;
            std::vector <std::pair <int, int> > Edges_;
            std::vector<Agent> agents_;
            string mapName = w.mapName;
            std::pair<int,int> agtSize = w.agentRange;
            int stepSize = w.stepSize;

            // Export new map
            //PublicBenchmark(node_number,Edges_,mapName,dataRow); // it will read, and fill these datastructures;
            string graphName = "/home/ubuntu/Alhamdulellah/Demos/PublicProblemTest/";
            graphName.append (mapName);
            graphName.erase(graphName.find_last_of("."),string::npos);
            string tempGraphName = graphName;
            graphName.append(".gml");
            string graph = graphName;//createGraph(node_number,Edges_,graphName) ;//
            //int x;
            //std::cin>>x;
            //-----------
            for (int agentSize=agtSize.first; agentSize<agtSize.second; agentSize+=stepSize) {
               std::vector<Agent> agents_;
               int nodeNumber = node_number;
                selectRandomAgents (agents_,agentSize,dataRow, nodeNumber);
                string instanceName = tempGraphName;
                instanceName.append("_A_");
                instanceName.append(IntToString(agentSize));
                instanceName.append(".instance");


                string instance = createInstance(agents_.size(),agents_, graph,instanceName) ;


                string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/PublicProblemTestGraphs/" + IntToString(solver) + "/TestingPublicBenchmark" + "_map" /*+ fileName*/ + "_A"+ IntToString(agentSize)+".txt" ;
                string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="PublicBenchmark";


                if (solver==1) {// push and swap
                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }

                if (solver ==2) { // push and Spin
                   PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }
                if (solver ==3) { // push and Spin
                    //std::vector<Bicycle> cycles =w.cycles ;
                    //std::vector <BiEdge> BiEdges = w.BiEdges;
                    //Bibox (cycles, BiEdges,exexc_time,path_len,condense_len,completed_agents,N,E,A);
                }
            }//end for

        }
      } // end if (testing_plan_index==3)
    if (testing_plan_index==4) {
        testingRandomBiconnected w;
         if(w.exec() == QDialog::Accepted) {
             int plan = w.plan;
             int stepSize = w.stepSize;
             //
             int free = w.free;
             std::pair<int,int> c0Range = w.c0Range;
             std::pair<int,int> handlesRange = w.handlesRange;
             std::pair<int,int> maxLenRange = std::make_pair(w.c0Range.first*2,w.c0Range.second*2); /////
             //
             int c0 = w.c0;
             int handles = w.handles;
             int maxLen = w.maxLen;
             std::pair<int,int> freeRange = w.freeRange;
             if (plan ==1) {// fixed free nodes
                 for (int i=0; i<(c0Range.second-c0Range.first); i+=stepSize) {
                     string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                     string graphRec="/home/ubuntu/Alhamdulellah/Demos/RandomBiconnectedGraphs/" + IntToString(solver_index) + "/TestingRandBi_f"+IntToString(free)+
                             "_c0_"+IntToString(c0Range.first+i) +"_H_"+IntToString(handlesRange.first+i) +
                             +  "_M_" +IntToString(maxLenRange.first+i) + ".txt" ;
                     if (solver_index<3) {
                         int nodes; std::vector<Agent> agents; std::vector<std::pair<int,int> >Edges;
                         generate_bipartite(c0Range.first+i,handlesRange.first+i,2*(c0Range.first+i),free,nodes,Edges,agents,dataRow);
                         string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomBiconnected/TestingRandBi_f"+IntToString(free)+
                                 "_c0_"+IntToString(c0Range.first+i) +"_H_"+IntToString(handlesRange.first+i) +
                                 +  "_M_" +IntToString(maxLenRange.first+i) +".gml";

                         string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomBiconnected/TestingRandBi_f"+IntToString(free)+
                                 "_c0_"+IntToString(c0Range.first+i) +"_H_"+IntToString(handlesRange.first+i) +
                                 +  "_M_" +IntToString(maxLenRange.first+i) +".instance";
                         string graph = createGraph(nodes,Edges,graphName) ;
                         string instance = createInstance(agents.size(),agents, graph,instanceName) ;
                         string InstanceType = "RandomBiconnectedGraph";

                         if (solver_index==1) {// push and swap
                             PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                         }

                         if (solver_index ==2) { // push and Spin
                            PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                         }

                     }
                     else {
                         readBibox (c0Range.first+i, handlesRange.first+i,2*(c0Range.first+i),free,graphRec,dataFile,dataRow);
                    }
                }
             }// end if (plan ==1)
             if (plan==2) {
                 for (int i=freeRange.first; i<freeRange.second; i+=stepSize) {
                     string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                     string graphRec="/home/ubuntu/Alhamdulellah/Demos/RandomBiconnectedGraphs/" + IntToString(solver_index) + "/TestingRandBi_f"+IntToString(i)+
                             "_c0_"+IntToString(c0) +"_H_"+IntToString(handles) +
                             +  "_M_" +IntToString(maxLen) + ".txt" ;
                     if (solver_index<3) {
                         int nodes; std::vector<Agent> agents; std::vector<std::pair<int,int> >Edges;
                         generate_bipartite(c0,handles,maxLen,i,nodes,Edges,agents,dataRow);
                         string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomBiconnected/TestingRandBi_f"+IntToString(i)+
                                 "_c0_"+IntToString(c0) +"_H_"+IntToString(handles) +
                                 +  "_M_" +IntToString(maxLen) + ".gml";
                         string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomBiconnected/TestingRandBi_f"+IntToString(i)+
                                 "_c0_"+IntToString(c0) +"_H_"+IntToString(handles) +
                                 +  "_M_" +IntToString(maxLen) + ".instance";
                         string graph = createGraph(nodes,Edges,graphName) ;
                         string instance = createInstance(agents.size(),agents, graph,instanceName) ;
                         string InstanceType = "RandomBiconnectedGraph";

                         if (solver_index==1) {// push and swap
                             PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                         }

                         if (solver_index ==2) { // push and Spin
                            PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                         }

                     }
                     else {
                         readBibox (c0,handles,maxLen,i,graphRec,dataFile,dataRow);
                    }
                }
             } // end if (plan ==2)
         }// end if(w.exec() == QDialog::Accepted)

      }// end if (testing_plan_index==4)

    if (testing_plan_index==5) {
        testingRandomTWCL w;
        if(w.exec() == QDialog::Accepted) {
            if (w.plan==1) { // fixed free variable BL
            for (int i=w.blRange.first; i<w.blRange.second; i+=w.stepSize) {
                std::vector<std::pair <int,int> >Edges;
                std::vector <Agent> agents;
                //void creatTWCL (int nodesNum, int bridgLength, int freeNodes, std::pair <int,int>& Edges, std::vector <Agent>& agents)
                createRandomTWCL (w.node_number,i ,w.free_nodes, Edges, agents,dataRow);
                string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTest/TestingTWCL_N"+IntToString(w.node_number)
                        + "_BL_"+ IntToString(i) + "_F_" + IntToString(w.free_nodes) +".gml";
                string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTest/TestingTWCL_N"+IntToString(w.node_number)
                        + "_BL_"+ IntToString(i) + "_F_" + IntToString(w.free_nodes) +".instance";
                string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTestGraphs/" + IntToString(solver_index) + "/TestingTWCL_N"+IntToString(w.node_number)
                        + "_BL_"+ IntToString(i) + "_F_" + IntToString(w.free_nodes) +".txt" ;
                string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                string InstanceType="RandomTWCL";
                string graph = createGraph(w.node_number,Edges,graphName) ;
                string instance = createInstance(agents.size(),agents, graph,instanceName) ;

                if (solver_index==1) {// push and swap
                    PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }
                if (solver_index ==2) { // push and Spin
                   PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                }

                }
            }// end if (w.plan==1)
            if (w.plan==2) {
                for (int i=w.freeRange.first; i<w.freeRange.second; i+=w.stepSize) {
                    std::vector<std::pair <int,int> >Edges;
                    std::vector <Agent> agents;
                    createRandomTWCL (w.node_number,w.bridge_length,i, Edges, agents,dataRow);
                    string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTest/TestingTWCL_N"+IntToString(w.node_number)
                            + "_BL_"+ IntToString(w.bridge_length) + "_F_" + IntToString(i) +".gml";
                    string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTest/TestingTWCL_N"+IntToString(w.node_number)
                            + "_BL_"+ IntToString(w.bridge_length) + "_F_" + IntToString(i) +".instance";
                    string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/RandomTWCLTestGraphs/" + IntToString(solver_index) + "/TestingTWCL_N"+IntToString(w.node_number)
                            + "_BL_"+ IntToString(w.bridge_length) + "_F_" + IntToString(i) +".txt" ;
                    string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                    string InstanceType="RandomTWCL";
                    string graph = createGraph(w.node_number,Edges,graphName) ;
                    string instance = createInstance(agents.size(),agents, graph,instanceName) ;

                    if (solver_index==1) {// push and swap
                        PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    }
                    if (solver_index ==2) { // push and Spin
                       PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                    }

                    }

            }
        } // end if(w.exec() == QDialog::Accepted)
      } // end if (testing_plan_index==5)

    if (testing_plan_index==6) {
        testRandomGrid w;
                if(w.exec() == QDialog::Accepted) {
                    if (w.plan==1) { // fixed free variable HW
                        std::cout<<"Here\n";
                    for (int i=w.HWRange.first; i<w.HWRange.second; i+=w.stepSize) {
                        std::vector<std::pair <int,int> >Edges;
                        std::vector <Agent> agents;
                        //CreateGrid (w.HW,w.HW,int free, std::vector <std::pair<int,int> >& Edges,std::vector<Agent> agents)
                        CreateGrid (i,i,w.free_nodes ,Edges,agents,dataRow);
                        string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomGridTest/TestingGrid_H"+IntToString(i)
                                +"_W_" +IntToString(i)+ "_F_" +IntToString(w.free_nodes) + ".gml";
                        string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomGridTest/TestingGrid_H"+IntToString(i)
                                +"_W_" +IntToString(i)+ "_F_" +IntToString(w.free_nodes) + ".instance";
                        string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/RandomGridTestGraphs/" + IntToString(solver_index) + "/TestingGrid_H"+IntToString(i)
                                +"_W_" +IntToString(i)+ "_F_" +IntToString(w.free_nodes) + ".txt" ;
                        string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                        string InstanceType="RandomGrid";
                        string graph = createGraph((i*i),Edges,graphName) ;
                        string instance = createInstance(agents.size(),agents, graph,instanceName) ;

                        if (solver_index==1) {// push and swap
                            PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                        }
                        if (solver_index ==2) { // push and Spin
                           PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                        }
                        if (solver_index==3) { // bibox
                            readBibox (4, ((i-1)*(i-1)-1),4,w.free_nodes,graphRec,dataFile,dataRow);
                        }

                        } // end for
                    }// end if (w.plan==1)
                    if (w.plan==2) {
                        std::cout<<"Here\n";
                        for (int i=w.freeRange.first; i<w.freeRange.second; i+=w.stepSize) {
                            std::vector<std::pair <int,int> >Edges;
                            std::vector <Agent> agents;
                            //CreateGrid (w.HW,w.HW,int free, std::vector <std::pair<int,int> >& Edges,std::vector<Agent> agents)
                            CreateGrid (w.HW,w.HW,i ,Edges,agents,dataRow);
                            string graphName = "/home/ubuntu/Alhamdulellah/Demos/RandomGridTest/TestingGrid_H"+IntToString(w.HW)
                                    +"_W_" +IntToString(w.HW)+ "_F_" +IntToString(i) + ".gml";
                            string instanceName = "/home/ubuntu/Alhamdulellah/Demos/RandomGridTest/TestingGrid_H"+IntToString(w.HW)
                                    +"_W_" +IntToString(w.HW)+ "_F_" +IntToString(i) + ".instance";
                            string graphRec =  "/home/ubuntu/Alhamdulellah/Demos/RandomGridTestGraphs/" + IntToString(solver_index) + "/TestingGrid_H"+IntToString(w.HW)
                                    +"_W_" +IntToString(w.HW)+ "_F_" +IntToString(i) + ".txt" ;
                            string dataFile = "/home/ubuntu/Alhamdulellah/Demos/Data/DataRow.txt";
                            string InstanceType="RandomGrid";
                            string graph = createGraph((w.HW*w.HW),Edges,graphName) ;
                            string instance = createInstance(agents.size(),agents, graph,instanceName) ;

                            if (solver_index==1) {// push and swap
                                PushAndSwapTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                            }
                            if (solver_index ==2) { // push and Spin
                               PushAndSpinTest (graph, instance,graphRec,dataFile,InstanceType,dataRow);
                            }
                            if (solver_index==3) { // bibox
                                readBibox (4, ((w.HW-1)*(w.HW-1)-1),4,i,graphRec,dataFile,dataRow);
                            }
                            } // end for

                  }// end  if (w.plan==2)
        }// endif(w.exec() == QDialog::Accepted)

    } // end if (testing_plan_index==6)

    }// end if(main_window.exec() == QDialog::Accepted)

    // GraphRec
    system("gnome-terminal -x sh -c 'cd /home/ubuntu/Alhamdulellah ; ./GraphRec' &");
    return a.exec();

}

