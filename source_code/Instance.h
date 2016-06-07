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

#ifndef PUSH_AND_SWAP_INSTANCE_HPP
#define PUSH_AND_SWAP_INSTANCE_HPP

#include "Agent.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

// Definition of an instance of a multi-agent path planning problem
// Reads an instance file defining the problem.  The class
// contains a graph filename, and a list of agents, their starts,
// and their goals on the graph



class Instance
{
public:
    std::string graphFilename;
    std::vector<Agent> agents;

       char* FileName;
       std::string graph_name;
       int N ;
       int BL ;
       int CL ;
       int C1 ;
       int C2 ;
       int A;
       int R;
       int C;
       int E;
       ofstream file;
       std::string IntToString ( int number )
    {
      std::ostringstream oss;

      // Works just like cout
      oss<< number;

      // Return the underlying string
      return oss.str();
    }


    public:
        Instance(){N =0; BL =0 ; CL =0; C1 =0; C2 =0; A=0; R=0; C=0;}

        void Parsing (const char* filename) {parse(filename);}

        void parse(const char* filename)
        {
            std::ifstream fin;
            fin.open(filename);
            if (!fin)
            {
                std::cout<<"File to open="<<filename<<"\n";
                std::cerr << "ERROR: Could not open " << filename << std::endl;
                exit(1);
            }

            std::string line;
            while (!fin.eof())
            {
                std::getline(fin, line);
                parseLine (line);
            }
            fin.close();
        }

        void parseLine(std::string& line)
        {
            boost::trim_all(line);
            if (line.size() == 0)
                return;

            std::vector<std::string> strs;
            boost::split(strs, line, boost::is_any_of(":"));  // split by colon

            if (strs.size() == 1)
            {
                boost::split(strs, line, boost::is_any_of("="));  // split by equals
                boost::trim_all(strs[0]);
                if (strs[0] == "graph" && strs.size() == 2)
                {
                    boost::trim_all(strs[1]);
                    graphFilename = strs[1];
                }
                else
                {
                    std::cerr << "WARNING: Unknown line: " << strs[0] << std::endl;
                }
            }
            else
            {
                assert(strs.size() == 2);
                if (strs[0] == "agent")
                {
                    std::vector<std::string> conf;
                    boost::trim_all(strs[1]);
                    boost::split(conf, strs[1], boost::is_any_of(","));  // split by comma
                    assert(conf.size() == 2);

                    // Start
                    std::string start = conf[0];
                    std::vector<std::string> stconf;
                    boost::split(stconf, start, boost::is_any_of("="));
                    assert(stconf.size() == 2);
                    boost::trim_all(stconf[0]);
                    assert(stconf[0] == "start");
                    boost::trim_all(stconf[1]);
                    unsigned int st = boost::lexical_cast<unsigned int> (stconf[1]);

                    // Goal
                    std::string goal = conf[1];
                    std::vector<std::string> glconf;
                    boost::split(glconf, goal, boost::is_any_of("="));
                    assert(glconf.size() == 2);
                    boost::trim_all(glconf[0]);
                    assert(glconf[0] == "goal");
                    boost::trim_all(glconf[1]);
                    unsigned int gl = boost::lexical_cast<unsigned int> (glconf[1]);

                    agents.push_back(Agent(agents.size(), st, gl));
                }
            }
        }


        //========================================================================//
        //                              Create graph                              //
        //========================================================================//
           void set_graph_attr() {
            srand (time(NULL));
               int min =-1, max=-1;
            //------
            min = 11;
            max = 1000;
            std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
            N = min + (rand() % (int)(max - min + 1));
            std::cout<<"N ="<<N<<std::endl;
            //------
            min = 6;
            max = N-6;
            BL = min + (rand() % (int)(max - min + 1));
            std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
            std::cout<<"BL ="<<BL<<std::endl;
            //------
            CL = N - BL + 2 ;
            min = 3;
            max = CL-4;
            C1 = min + (rand() % (int)(max - min + 1));
            std::cout<<"CL = "<<CL<<std::endl;
            std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
            std::cout<<"C1 ="<<C1<<std::endl;
            C2 = CL - C1 ;
            std::cout<<"C2 ="<<C2<<std::endl;

        }


           void createGraph () {
            set_graph_attr() ;
            A = set_instance_attr () ;


            graph_name = "N";
            graph_name.append (IntToString (N));
            graph_name.append(",BL");
            graph_name.append (IntToString (BL));
            graph_name.append(",CL");
            graph_name.append (IntToString (CL));
            graph_name.append(",C1");
            graph_name.append (IntToString (C1));
            graph_name.append(",C2");
            graph_name.append (IntToString (C2));
            graph_name.append(",A");
            graph_name.append (IntToString (A));
            graph_name.append(".gml");

            std::stringstream convert;
            convert << "config/N" << N << ",BL" << BL << ",CL" << CL << ",C1" << C1 << ",C2" << C2 <<",A" << A <<".gml";
            FileName = &(convert.str())[0];
            std::cout<<"FileName: "<<FileName<<std::endl;
            file.open (FileName);
            // Set file header
            file <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                 <<  "\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"> "
                 <<  "\n<key id=\"key0\" for=\"node\" attr.name=\"Coordinate\" attr.type=\"string\" /> "
                 <<  "\n<key id=\"key1\" for=\"edge\" attr.name=\"Weight\" attr.type=\"double\" /> "
                 <<  "\n<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\"> \n";

            // Set nodes
            for (int i=0; i<N; i++)
                file << "<node id=\"n" << i << "\"> "
                     << "\n<data key=\"key0\">" << i << ".0 0.0 0.0</data> "
                     << "\n</node> \n";

            // Set edges
            // 1- C1
            int edge=0, nodes_ptr=0, start = nodes_ptr;
            for (int i=0; i<C1-1; i++)
                file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< nodes_ptr++ <<"\" target=\"n"<<nodes_ptr <<"\"> "
                     << "\n<data key=\"key1\">1</data>"
                     << "\n</edge> \n";
            // Backedge
            file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< nodes_ptr <<"\" target=\"n"<<start <<"\"> "
                 << "\n<data key=\"key1\">1</data>"
                 << "\n</edge> \n";

            // Set edges
            // 2- Bridge

            for (int i=0; i<BL-1; i++)
                file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< nodes_ptr++ <<"\" target=\"n"<<nodes_ptr <<"\"> "
                     << "\n<data key=\"key1\">1</data>"
                     << "\n</edge> \n";

            // Set edges
            // 2- C2
            start = nodes_ptr;
            for (int i=0; i<C2-1; i++)
                file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< nodes_ptr++ <<"\" target=\"n"<<nodes_ptr <<"\"> "
                     << "\n<data key=\"key1\">1</data>"
                     << "\n</edge> \n";

            // Backedge
            file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< nodes_ptr <<"\" target=\"n"<<start <<"\"> "
                 << "\n<data key=\"key1\">1</data>"
                 << "\n</edge> \n";

        file <<"  </graph> \n </graphml>";
        file.close();
        }

        //========================================================================//
        //                            Create instance                             //
        //========================================================================//

        int set_instance_attr () {
            srand (time(NULL));
            int A=-1;
            int min =-1, max=-1;
            //------
            int free = BL + 2;
            min = 1;
            max = N-free;
            std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
            A = min + (rand() % (int)(max - min + 1));
            std::cout<<"A ="<<A<<std::endl;
            return A;
        }

        char* create_instance (int &AA,std::string &fileName) {
            AA=A;
            char* Ins_FileName;
            // Setting file name
            std::stringstream convert; // stringstream used for the conversion
            convert <<"config/N"<< N <<",R" << R << ",C" << C
                   << ",A" << A << ".instance";

            Ins_FileName = &(convert.str())[0];
            std::cout<<"Ins_FileName: "<<Ins_FileName<<std::endl;
            file.open (Ins_FileName);


            // Set agents
            std::vector <int> starts, goals;
            int start, goal;
            std::cout<<"Graph name ="<<graph_name<<"\n";
            file << "graph =" <<graph_name <<"\n" ;
            srand (time(NULL));
            int max=N-1,min=0;
            for (int i=0; i<A; i++) {

                do {

                    start = min + (rand() % (int)(max - min + 1));
                } while (std::find(starts.begin(),starts.end(),start)!=starts.end());
                starts.push_back(start);

                do {

                    goal = min + (rand() % (int)(max - min + 1));
                } while (std::find(goals.begin(),goals.end(),goal)!=goals.end());
                goals.push_back(goal);

                file << "agent: start = "<<start<<", goal = "<<goal<<"\n";
            }


            file.close();


            std::string tmp = "/home/ubuntu/build/Demos/config/N" + IntToString(N)
                    + ",BL"+IntToString(BL)
                    + ",CL"+IntToString(CL) + ",C1"+IntToString(C1)
                    + ",C2"+ IntToString(C2) + ",A" +IntToString(A) + ".instance";

           fileName= tmp;

           std::cout<<"Tmp="<<tmp<<"\n";
           std::cout<<fileName<<"\n";

        }
        //========================================================================//
        //                              Create grid                               //
        //========================================================================//
        void set_grid_attr() {
         srand (time(NULL));
            int min =-1, max=-1;
         //------
         min = 4;
         max = 50;
         std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
         R = min + (rand() % (int)(max - min + 1));
         C = min + (rand() % (int)(max - min + 1));
         N = R * C ;
         std::cout<<"R="<<R<<" ,C="<<C<<", N ="<<N<<std::endl;
     }


        void createGrid() {
         set_grid_attr() ;
         A = set_instance_attr_grid () ;

         graph_name = "N";
         graph_name.append (IntToString (N));
         graph_name.append(",R");
         graph_name.append (IntToString (R));
         graph_name.append(",C");
         graph_name.append (IntToString (C));
         graph_name.append(",A");
         graph_name.append (IntToString (A));
         graph_name.append(".gml");

         std::stringstream convert;
         convert << "config/N" << N << ",R" << R << ",C" << C <<",A" << A <<".gml";

         FileName = &(convert.str())[0];
         std::cout<<"FileName: "<<FileName<<std::endl;
         file.open (FileName);
         // Set file header
         file <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
              <<  "\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"> "
              <<  "\n<key id=\"key0\" for=\"node\" attr.name=\"Coordinate\" attr.type=\"string\" /> "
              <<  "\n<key id=\"key1\" for=\"edge\" attr.name=\"Weight\" attr.type=\"double\" /> "
              <<  "\n<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\"> \n";

         // Set nodes
         for (int i=0; i<N; i++)
             file << "<node id=\"n" << i << "\"> "
                  << "\n<data key=\"key0\">" << i << ".0 0.0 0.0</data> "
                  << "\n</node> \n";

         // Set edges
         int counter=0;
         int edge=0, nodes_ptr=0, start = nodes_ptr;
         for (int row=0; row<R-1; row++)
             for (int col=0; col<C; col++) {
                 if ((counter+1) % C != 0) {
                     file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< counter <<"\" target=\"n"<< counter+1 <<"\"> "
                          << "\n<data key=\"key1\">1</data>"
                          << "\n</edge> \n";
                 }
                 file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< counter <<"\" target=\"n"<< counter+C <<"\"> "
                      << "\n<data key=\"key1\">1</data>"
                      << "\n</edge> \n";
                 counter++;
             }
         // Last row
         for (int col=0; col<C-1; col++) {
                 file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< counter <<"\" target=\"n"<< counter+1 <<"\"> "
                      << "\n<data key=\"key1\">1</data>"
                      << "\n</edge> \n";
             counter++;
         }

     file <<"  </graph> \n </graphml>";
     file.close();
     }

     //========================================================================//
     //                            Create instance                             //
     //========================================================================//

     int set_instance_attr_grid () {
         srand (time(NULL));
         int A=-1;
         int min =1, max= N-2;
         A = min + (rand() % (int)(max - min + 1));
         std::cout<<"A ="<<A<<std::endl;
         return A;
     }

     char* create_instance_grid (int &AA,std::string &fileName) {
         AA=A;
         char* Ins_FileName;
         // Setting file name
         std::stringstream convert; // stringstream used for the conversion
         convert <<"config/N"<< N <<",R" << R << ",C" << C << ",A" << A << ".instance";

         Ins_FileName = &(convert.str())[0];
         std::cout<<"Ins_FileName: "<<Ins_FileName<<std::endl;
         file.open (Ins_FileName);


         // Set agents
         std::vector <int> starts, goals;
         int start, goal;
         std::cout<<"Graph name ="<<graph_name<<"\n";
         file << "graph =" <<graph_name <<"\n" ;
         srand (time(NULL));
         int max=N-1,min=0;
         for (int i=0; i<A; i++) {

             do {

                 start = min + (rand() % (int)(max - min + 1));
             } while (std::find(starts.begin(),starts.end(),start)!=starts.end());
             starts.push_back(start);

             do {

                 goal = min + (rand() % (int)(max - min + 1));
             } while (std::find(goals.begin(),goals.end(),goal)!=goals.end());
             goals.push_back(goal);

             file << "agent: start = "<<start<<", goal = "<<goal<<"\n";
         }


         file.close();


         std::string tmp = "/home/ubuntu/build/Demos/config/N" + IntToString(N)
                 + ",R"+IntToString(R)
                 + ",C"+IntToString(C) +  ",A" +IntToString(A) + ".instance";

        fileName= tmp;

        std::cout<<"Tmp="<<tmp<<"\n";
        std::cout<<fileName<<"\n";

     }

     //========================================================================//
     //                          Create fullgraph                              //
     //========================================================================//
     void set_fullgraph_attr() {
      srand (time(NULL));
         int min =-1, max=-1;
      //------
      min = 3;
      max = 50;
      std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
      N = min + (rand() % (int)(max - min + 1));
      min = N+1;
      max = (N *(N-1)/2);
      std::cout<<"min = "<<min<<", max = "<<max<<std::endl;
      E = min + (rand() % (int)(max - min + 1));

      std::cout<<"N="<<N<<" ,E="<<E<<std::endl;
     }


     void createfullgraph() {
      set_fullgraph_attr() ;
      A = set_instance_attr_fullgraph() ;

      graph_name = "N";
      graph_name.append (IntToString (N));
      graph_name.append(",E");
      graph_name.append (IntToString (E));
      graph_name.append(",A");
      graph_name.append (IntToString (A));
      graph_name.append(".gml");

      std::stringstream convert;
      convert << "config/N" << N << ",E" << E <<",A" << A <<".gml";

      FileName = &(convert.str())[0];
      std::cout<<"FileName: "<<FileName<<std::endl;
      file.open (FileName);
      // Set file header
      file <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
           <<  "\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"> "
           <<  "\n<key id=\"key0\" for=\"node\" attr.name=\"Coordinate\" attr.type=\"string\" /> "
           <<  "\n<key id=\"key1\" for=\"edge\" attr.name=\"Weight\" attr.type=\"double\" /> "
           <<  "\n<graph id=\"G\" edgedefault=\"undirected\" parse.nodeids=\"canonical\" parse.edgeids=\"canonical\" parse.order=\"nodesfirst\"> \n";


      // Set nodes
      for (int i=0; i<N; i++)
          file << "<node id=\"n" << i << "\"> "
               << "\n<data key=\"key0\">" << i << ".0 0.0 0.0</data> "
               << "\n</node> \n";

      // Set edges

      int edge=0;
      for (int j=0; j<N-1; j++)
              file << "<edge id=\"e"<< edge++ <<"\" source=\"n"<< j <<"\" target=\"n"<< j+1 <<"\"> "
                   << "\n<data key=\"key1\">1</data>"
                   << "\n</edge> \n";

    std::cout<<"N="<<N<<", Edge now="<<edge<<", E="<<E<<"\n";

      for (int i = edge; i<E ; i++ ) {
          int n1,n2;
          int min = 0;
          int max = N-1;
          n1 = min + (rand() % (int)(max - min + 1));
          min = 0;
          max = N-1;
          n2 = min + (rand() % (int)(max - min + 1));
          if ((n1 == n2) || (n1 == n2+1) || (n2 == n1+1))
              continue;
            file << "<edge id=\"e"<< edge <<"\" source=\"n"<< n1 <<"\" target=\"n"<< n2 <<"\"> "
               << "\n<data key=\"key1\">1</data>"
               << "\n</edge> \n";
      }

  file <<"  </graph> \n </graphml>";
  file.close();
  }

  //========================================================================//
  //                            Create instance                             //
  //========================================================================//

  int set_instance_attr_fullgraph () {
      srand (time(NULL));
      int A=-1;
      int min =1, max= N-2;
      A = min + (rand() % (int)(max - min + 1));
      std::cout<<"A ="<<A<<std::endl;
      return A;
  }

  char* create_instance_fullgraph (int &AA,std::string &fileName) {
      AA=A;
      char* Ins_FileName;
      // Setting file name
      std::stringstream convert; // stringstream used for the conversion
      convert <<"config/N"<< N << ",E" << E << ",A" << A << ".instance";

      Ins_FileName = &(convert.str())[0];
      std::cout<<"Ins_FileName: "<<Ins_FileName<<std::endl;
      file.open (Ins_FileName);


      // Set agents
      std::vector <int> starts, goals;
      int start, goal;
      std::cout<<"Graph name ="<<graph_name<<"\n";
      file << "graph =" <<graph_name <<"\n" ;
      srand (time(NULL));
      int max=N-1,min=0;
      for (int i=0; i<A; i++) {

          do {

              start = min + (rand() % (int)(max - min + 1));
          } while (std::find(starts.begin(),starts.end(),start)!=starts.end());
          starts.push_back(start);

          do {

              goal = min + (rand() % (int)(max - min + 1));
          } while (std::find(goals.begin(),goals.end(),goal)!=goals.end());
          goals.push_back(goal);

          file << "agent: start = "<<start<<", goal = "<<goal<<"\n";
      }


      file.close();


      std::string tmp = "/home/ubuntu/build/Demos/config/N" + IntToString(N)
              + ",E"+IntToString(E) +  ",A" +IntToString(A) + ".instance";

     fileName= tmp;

     std::cout<<"Tmp="<<tmp<<"\n";
     std::cout<<fileName<<"\n";

  }

        //========================================================================//
        //                            Data Collector                              //
        //========================================================================//

        void writeFullInformation () {
            file.open ("/home/ubuntu/Alhamdulellah/Demos/Data.txt", std::ios_base::app);

            // Write graph information
            file << "\nN=" << "\t" << N <<  "\t" << "E=" << "\t" << E << "\t"
                 "A=" << "\t" << A << "\t";

           file.close ();

        }

        void writeCompletenessRate (float perc,float exec_time,int path_len,int condense_len) {

            file.open ("/home/ubuntu/build/Demos/config/Data/completeness_rate_fullgraph.txt", std::ios_base::app);

            // Write graph information
            file << "Completeness_rate=" << "\t" << perc << "\t"
                 << "Execution_time=" << "\t"  <<exec_time << "\t\t"
                 << "Path_length=" << "\t" << path_len << "\t"
                 << "Condense_length=" << "\t" << condense_len << "\t" ;

            file.close ();
        }

        unsigned int numAgents(void) const { return agents.size(); }
        Agent getAgent(unsigned int idx) { return agents[idx]; }
        std::vector<Agent> getAgents(void) { return agents; }

        std::string getGraphFilename (void) const { return graphFilename; }




};

#endif
