

/****************************************************************************
Copyright (c) 2016, Ebtehal Turki Saho Alotaibi, All rights reserved.

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

// Author: Ebtehal Turki Alotaibi

#include <algorithm>
#include <limits>
#include <math.h>

#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <algorithm>
#include "PushAndSpin.h"
#include "Logger.h"
#include "ElementaryCyclesSearch.h"



// Constant which defines the absence of an agent
static const unsigned int NO_AGENT = std::numeric_limits<unsigned int>::max();
// Constant which defines the absence of a vertex
static const unsigned int NO_VERTEX = std::numeric_limits<unsigned int>::max();
static int disc_time = 0 ;
std::vector <int> to_resolve ;

#define NIL -1

PushAndSpin::PushAndSpin() : graph_(NULL) {ID=0;}

void Union (std::vector<unsigned int> S1, std::vector<unsigned int> S2, bool &s1, bool &s2) {

    s1=false,s2=false;
    std::vector<unsigned int> S3;

    for (int i=0; i<S1.size(); i++)
        S3.push_back(S1[i]);

    for (int i=0; i<S2.size(); i++)
        if (std::find(S3.begin(),S3.end(),S2[i])==S3.end())
            S3.push_back(S2[i]);


    std::sort(S1.begin(),S1.end());
    std::sort(S2.begin(),S2.end());
    std::sort(S3.begin(),S3.end());

    /*
    std::cout<<"S1 =";
    for (int i=0; i<S1.size(); i++)
        std::cout<<S1[i]<<",";
    std::cout<<"]\n";
    std::cout<<"S2 =";
    for (int i=0; i<S2.size(); i++)
        std::cout<<S2[i]<<",";
    std::cout<<"]\n";
    std::cout<<"S3 =";
    for (int i=0; i<S3.size(); i++)
        std::cout<<S3[i]<<",";
    std::cout<<"]\n";*/

    if (S1.size() == S3.size()) {
        s1=true;
        for (int i=0; i<S3.size(); i++)
            if (S1[i]!=S3[i])
                s1=false;
    }
    else // S1.size() < S3.size for sure
        s1=false;


    if (S2.size() == S3.size()) {
        s2=true;
        for (int i=0; i<S3.size(); i++)
            if (S2[i]!=S3[i])
                s2=false;
    }
    else // S2.size() < S3.size for sure
        s2=false;

}

bool PushAndSpin::IsBlocked (std::vector<unsigned int> cycle) {
        // 1- check if cycle is subset any registered cycle
        /*std::cout<<"Checking cycle C = {";
        for (int c=0; c<cycle.size(); c++)
            std::cout<<cycle[c]<<",";
        std::cout<<"} cycles_index size ="<<cycles_index.size()<<"\n";*/

        for (int i=0; i<cycles_index.size(); i++) {
            bool s1=false, s2=false;
            Union (cycle, cycles_index[i], s1, s2);
            if (s1 && s2)  // repeat
                return true;
            if (s1 && !s2) { // cycle == cycle U cycles_index[i]
                //std::cout<<"C is the superset\n";
                return true;
            }
            if (s2 && !s1 ) { // cycles_index[i] == cycle U cycles_index[i]
                //std::cout<<"C is the subset\n";
                cycles_index.erase(cycles_index.begin()+i);
                cycles_index.push_back(cycle);
                return true;
            }
            if (!s1 && !s2){
                //std::cout<<"Continue\n";
                continue;
            }
        }

        //std::cout<<"Is ok\n";
        return false;
}

bool NotExist (std::vector<std::vector<unsigned int> > cycles_index,std::vector<unsigned int> shorest_path) {

    for (int c=0; c<shorest_path.size(); c++)

    for (int i=0; i<cycles_index.size(); i++) {
            std::sort(cycles_index[i].begin(),cycles_index[i].end());
            std::sort(shorest_path.begin(),shorest_path.end());
            bool exist=true;
            if (cycles_index[i].size() == shorest_path.size()) {
                for (int j=0; j<shorest_path.size(); j++)
                    if (cycles_index[i][j] != shorest_path[j])
                        exist = false;
            }
            else
                exist = false;

            if (exist)
                return false;
    } //end without return !
    return true;
}

bool PushAndSpin::isCyclic()
{
    // Mark all the vertices as not visited and not part of recursion
    // stack
    int V = graph_->numNodes(), R=-1, C=-1,t;
    std::vector < std::vector<unsigned int> > cycles;
    bool found = false;
    std::vector<unsigned int> neighbors;

    /*
    // Set R
    for (int v = 0; v < V-1; v++) {
        graph_->getNeighbors(v, neighbors);
        bool end=true;
        for (int n=0; n<neighbors.size() ; n++) // if one of neighbour is consequence, not end !
            if (neighbors[n] == v+1) {
                end = false;
                break;
            }
        if (end) {
            R = v+1;
            break;
        }
    }
    C = V/R;
    std::cout<<"R ="<<R<<", C="<<C<<"\n";
    bool r_even=false,c_even = false;
    if (R%2 == 0)
        r_even = true;
    if (C%2 ==0)
        c_even = true;
    std::vector<unsigned int>  cycle;
    if (R==C) {   // outer cycle

        std::vector<unsigned int> v1,v2;

        for (int i=0; i<R; i++){
            if (i==0)
                for (int j=0; j<C; j++)
                    {
                        v1.push_back(j); // 1,2,3
                    }
            else
                v1.push_back((C*(i+1))-1);
        }

        int last=0;
        for (int i=1; i<R; i++){
            if (i==(R-1)) {
                int start = C*(R-1);
                for (int j=0; j<C; j++)
                    {
                        v2.push_back(j+start); // 1,2,3
                    }
            }
            else {
                last = last+C;
                v2.push_back(last);
            }

        }
        for (int i=v2.size()-1; i>=0; i--)
            v1.push_back (v2[i]);

        std::cout<<"V1= {";
            for (int i=0; i<v1.size(); i++)
        std::cout<<v1[i]<<", ";
    std::cout<<"}\n";
        std::cin>>t;

        cycles_index.push_back(v1);

    }




    else {
    */

    // DFS trees
    for (int v = 0; v < V; v++) {

        found = false;
        std::vector<unsigned int> obstacles;
        std::vector<unsigned int> neighbors;
        std::vector<unsigned int> shorest_path;

        obstacles.push_back (v);
        graph_->getNeighbors(v, neighbors);

        for (int n=0; n<neighbors.size()-1; n++)
            for (int nn=n+1; nn<neighbors.size(); nn++){
                shorest_path.clear();
                graph_->AStar(neighbors[n], neighbors[nn], shorest_path, obstacles);
                shorest_path.push_back (v);
                /*std::cout<<"The path between "<<neighbors[n]<<" and "<<neighbors[nn]<<" is {";
                for (int c=0; c<shorest_path.size(); c++){
                    std::cout<<shorest_path[c]<<",";
                }
                std::cout<<"}\n";*/
                if (shorest_path.size()>2) { // For V it is enough to be a node of a cycle
                //std::sort(shorest_path.begin(),shorest_path.end());
                std::cout<<"Processing node#"<<neighbors[n]<<"\n";
                if (NotExist (cycles_index, shorest_path))
                    cycles_index.push_back (shorest_path);
            }

           }

    }

    /*for (int i=0; i<cycles_index.size(); i++) {
        std::cout<<"Cycle = {";
        for (int j=0; j<cycles_index[i].size(); j++)
            std::cout<<cycles_index[i][j]<<",";
        std::cout<<"}\n";
    }*/
    // Sort
    /*for (size_t i = 0; i < cycles_index.size(); ++i)
    {
        // Output will contain indices into the input vector.
        cycles.push_back(cycles_index[i]);

        // Insertion sort:
        for (int j = 0; j < (int)i; ++j)
        {
            // Vertices with larger distance from the root come first
            if (cycles_index[j].size() > cycles_index[i].size())
            {
                // Shifting all members down one:
                for (int k = cycles.size()-2; k >= j; --k)
                    cycles[k+1] = cycles[k];
                cycles[j] = cycles[i];
                break;
            }
        }
    }*/


    cycles = cycles_index;
    cycles_index.clear();

    for (int i=0; i<cycles.size(); i++){
        bool f=false;

        if (IsBlocked (cycles[i]))
                f=true;
        if (!f) {
            set_cycle_type (cycles[i]);
            cycles_index.push_back (cycles[i]);
        }
    }


    if (cycles_index.size()>0)
        return true;
    return false;
}

PushAndSpin::PushAndSpin(const std::vector<Agent>& agents, Graph* graph, bool &PAR_)
{
    reversedmoves = 0;
    rot_time=0;
    extra_computation_time=0;
    PAR = PAR_;
    agents_ = agents;
    graph_ = graph;
    computeSwapVertices();
    ID=0;
    //======================================================================================//
    //                               HERE I CAN MADE CHANGES :                              //
    //======================================================================================//
     if (PAR) { // PAR
            if (isCyclic ()) {
           //std::cout<<"There is a cycle" <<std::endl;
            PAR=true;
            for (int i=0; i<graph_->numNodes() ; i++)
            {
                graph_-> set_counter (i,0);
                graph_-> set_type (i,"B"); // C : Cycle , B : Bridge
             }
        }
        else {
            //std::cout<<"There is no cycle! we will switch to PAS" <<std::endl;
            PAR=false; //act as PAS
            }
    }

    reorderAgents();
    //set_initial_locs() ;
}
void PushAndSpin::set_initial_locs() {
    for (int i=0; i<agents_.size(); i++) {
    std::vector<unsigned int> v ;
    v.push_back (agents_[i].getStart());
    std::pair<unsigned int, std::vector<unsigned int> > p = std::make_pair(agents_[i].getID(),v);
    actions_.push_back (p) ;
    }
}

PushAndSpin::~PushAndSpin()
{
}

void PushAndSpin::print_cycle () {
    for (int i=0; i<cycles_index.size() ; i++) {
        std::cout<<"Cycle ("<< i <<"): " ;
            for (int j=0 ; j< cycles_index[i].size(); j++)
                    std::cout << cycles_index[i][j] << "    " ;
            std::cout<<std::endl;
    }
}

bool IsExist (int v, std::vector<std::vector <unsigned int> > vec) {
    for (int i=0; i<vec.size(); i++)
        for (int j=0; j<vec[i].size(); j++)
            if (v==vec[i][j])
                return true;
    return false;
}

void PushAndSpin::set_cycle_type(std::vector <unsigned int> cycle) {
    for (int i=0; i<cycle.size(); i++) {
        bool B=false;
        std::vector<unsigned int> nbrs;
        graph_->getNeighbors (cycle[i], nbrs);
        for (int n=0; n<nbrs.size(); n++)
        if ( std::find(cycle.begin(), cycle.end(), nbrs[n]) == cycle.end()) {
            graph_->set_type (cycle[i],"B");
            B = true;}
        if (!B)
            graph_->set_type (cycle[i],"C");
    }
}

void PushAndSpin::set_entering_nodes (int s, int sa) {
    // Setting the entering nodes for each cycle respected to (s), the nearest node to s in C

    entering_node_index.clear();
    for (int i=0 ; i<cycles_index.size(); i++)
    {
        int min = graph_->numNodes() ;
        int v = -1;
        for (int j=0 ; j<cycles_index[i].size(); j++){
            if (s == cycles_index[i][j]) { //CB: v = s and stop
                v = s;
                break ;
            }
            else if (sa == cycles_index[i][j]) { //CB: v = sa and stop
                v = sa;
                break;
            }
            else {                      //BB
            std::vector<unsigned int> path;
            graph_->AStar(s, cycles_index[i][j], path);
                if (path.size() < min) {
                    min = path.size();
                    v = cycles_index[i][j];
                }
            }
        }
        entering_node_index.push_back(v);
    }

}

int  PushAndSpin::DFS (std::vector<node>& vertex, int root,int s, int sa,int& occupied,std::vector <unsigned int> temp_path) {
    int available =0;
    std::vector<unsigned int> nbrs;
    graph_->getNeighbors(root,nbrs);

    for (int i=0; i<nbrs.size(); i++)
        if (std::find(temp_path.begin(),temp_path.end(),vertex[nbrs[i]].id)!=temp_path.end())
            continue;
        else if (! vertex[nbrs[i]].visited)
            DFSUtil (vertex[nbrs[i]], vertex, available,s,sa, occupied,temp_path);


    return available;
}
void PushAndSpin::DFSUtil (node parent, std::vector<node>& vertex, int& available,int s,int sa,int& occupied,std::vector <unsigned int> temp_path) {

    vertex[parent.id].visited = true ;
    std::vector<unsigned int> nbrs;
    graph_->getNeighbors(parent.id,nbrs);
    for (int i=0; i<nbrs.size(); i++)
        if (std::find(temp_path.begin(),temp_path.end(),vertex[nbrs[i]].id)!=temp_path.end())
            continue;
        else if (! vertex[nbrs[i]].visited)
            DFSUtil (vertex[nbrs[i]],vertex,available,s,sa,occupied,temp_path);

    if (! vertex[parent.id].done)
            {
                available++;
                vertex[parent.id].done = true;
                if (std::find(occupied_.begin(), occupied_.end(), vertex[parent.id].id) != occupied_.end())
                    occupied++;

            }
}


// To compute the which cycle is best fit(excluding previous_attempts) for the swapping agents location (s)
// The path to the cycle is saved to swap_path.
// The selected cycle <entering node> is saved in swap_cycle
void PushAndSpin::computeSwapCycle(unsigned int s,unsigned int sa,  std::vector < std::vector <unsigned int> >&swapping_cycles,
                                   const std::vector <std::vector  <unsigned int> > &previous_attempts,std::vector<unsigned int> &entering_nodes)
{

    // Setting the entering nodes for each cycle respect to (s)
    set_entering_nodes (s,sa);
    int min = graph_->numNodes(), nearest = -1;
    //Create a formula for comparison !
    //1- count the agent in the path between (s) and entering_nodes[i]
    //2- count the empties in the cycle cycles_index[i]
    //3- if the empties >= agents == it fits


    for (int i=0 ; i<entering_node_index.size(); i++){

        // 1- Set the entering nodes for each cycle respect to (s)
        int available=0, required=0, occupied=0;

        // 2- Calculate required
        std::vector <unsigned int> temp_path, temp_path2;
        graph_->AStar(s, entering_node_index[i], temp_path);
        graph_->AStar(sa, entering_node_index[i], temp_path2);

        if (temp_path.size() < temp_path2.size())
            temp_path = temp_path2; // temp_path2 is the longest

        if (temp_path.back() == entering_node_index[i] ) //remove entering node
            temp_path.erase (temp_path.begin()+temp_path.size()-1);

        for (int t=0; t<temp_path.size();t++)
            if (std::find(occupied_.begin(), occupied_.end(), temp_path[t])!=occupied_.end())
                required++;


        // 2- Calculate availbale
        std::vector <PushAndSpin::node> vertex;
        for (int v=0; v<graph_->numNodes(); v++) {
            node temp;
            temp.id = v;
            temp.visited = temp.done = false;
            vertex.push_back(temp);
        }

        vertex[entering_node_index[i]].visited = true;
        available++;

        if (std::find(occupied_.begin(), occupied_.end(),vertex[entering_node_index[i]].id) != occupied_.end())
            occupied++;


        available += DFS (vertex, vertex[entering_node_index[i]].id,s,sa,occupied,temp_path) ;

                std::cout<<"s="<<s<<", sa ="<<sa<<"\n";

        int xx=0, yy=0;
        //std::cout<<"visited = {";
        for (int t=0; t<vertex.size(); t++) {
            //std::cout<<"id="<<vertex[t].id<<", visited ="<<vertex[t].visited<<"\n";
            if (vertex[t].visited) {
                xx++;
                if (std::find(occupied_.begin(), occupied_.end(),vertex[t].id) != occupied_.end())
                    yy++;
            }
        }
        std::cout<<"}\navailable="<<available<<", occupied ="<<occupied<<"\n";
        std::cout<<"xx="<<xx<<", yy ="<<yy<<"\n";


        if (required <= (available-occupied)) {

            std::vector <unsigned int> temp_cycle ;
            //=====================================//
            // setting the node in the cycle in a way to be start by the entering node

                /*std::cout<<"Element in the original cycle #";
                for (int z=0 ; z<cycles_index[i].size() ; z++) {
                    std::cout<<cycles_index[i][z];
                    std::cout<<",";
                }std::cout<<"\n";*/

                std::vector<unsigned int> nbrs;
                graph_->getNeighbors ( entering_node_index[i], nbrs);

                /*std::cout<<"Before : ";
                for (int n=0; n<nbrs.size(); n++)
                    std::cout<<"n["<<n<<"] ="<<nbrs[n]<<",";
                std::cout<<"\n";*/

                for (int n=0; n<nbrs.size(); n++) {
                    if (std::find (cycles_index[i].begin(), cycles_index[i].end(), nbrs[n])==cycles_index[i].end()){
                        nbrs.erase (nbrs.begin()+n);
                        n--;
                    }
                }

                /*std::cout<<"After : ";
                for (int n=0; n<nbrs.size(); n++)
                    std::cout<<"n["<<n<<"] ="<<nbrs[n]<<",";
                std::cout<<"\n";*/

                int ptr, start, end;
                for (int z=0 ; z<cycles_index[i].size() ; z++) {
                    if (cycles_index[i][z] == entering_node_index[i] ) {
                        if (z!=0) {
                         start = cycles_index[i][(z+1)%cycles_index[i].size()];
                         end = cycles_index[i][(z-1)%cycles_index[i].size()];
                        }
                        else {
                            start = cycles_index[i][(z+1)%cycles_index[i].size()];
                            end = cycles_index[i][cycles_index[i].size()-1];
                        }
                    }
                }

                //std::cout<<"We will start at "<<start<<" ,and end at "<<end<<"\n";

                ptr = start; // starting from the first nighbour of enter_node
                int x=-1;
                for (int c=0; c<cycles_index[i].size(); c++)
                    if (cycles_index[i][c] == start)
                        x=c;

                do{
                    temp_cycle.push_back (cycles_index[i][x%cycles_index[i].size()]);
                    ptr = cycles_index[i][x%cycles_index[i].size()];
                    x++;

                }while (ptr != end) ;

                temp_cycle.insert(temp_cycle.begin(),entering_node_index[i]);
            //=================================================//
                /*for (int t=0; t<temp_cycle.size(); t++)
                    std::cout<<temp_cycle[t]<<",";
                std::cout<<"\n";*/
                // heuristic purpose
                if (temp_path.size() <= min){
                    min = temp_path.size();
                    nearest = swapping_cycles.size(); // index of this cycle
                }

            swapping_cycles.push_back(temp_cycle);
        }
    }


    // heuristic purpose

    if (nearest!=-1) {
        std::vector <unsigned int> temp_cycle = swapping_cycles[nearest];
        swapping_cycles.erase (swapping_cycles.begin()+nearest);
        std::vector < std::vector <unsigned int> > temp_swapping_cycle;
        temp_swapping_cycle.push_back(temp_cycle);
        for (int t=0; t<swapping_cycles.size(); t++)
            temp_swapping_cycle.push_back (swapping_cycles[t]);
        swapping_cycles.clear();
        swapping_cycles = temp_swapping_cycle;
    }

    // Re-compute the entering node for (Suitable cycles only)
    std::vector<unsigned int>  temp_entering_nodes;

    for (int i=0 ; i<swapping_cycles.size(); i++)
    {

        int min = graph_->numNodes() ;
        int v = -1;
        for (int j=0 ; j<swapping_cycles[i].size(); j++){
            std::vector<unsigned int> path;
            if (graph_->AStar(s, swapping_cycles[i][j], path))
                if (path.size() < min) {
                    min = path.size();
                    v = swapping_cycles[i][j];
                }
        }
        temp_entering_nodes.push_back(v);
    }


    entering_nodes = temp_entering_nodes;

}

void PushAndSpin::reset_occupied () {
    occupied_.clear();
    for (int i=0; i<agents_.size(); i++)
        occupied_.insert(agents_[i].getCurrent());
}

int PushAndSpin::stacking_cycle (int agt, int swap_agt, int direction, int entering_node, std::vector <unsigned int> swapping_cycles, int back_agent){
        int degree = swapping_cycles.size();
        int index=-1,front=-1,bk=-1;
        std::vector <unsigned int> p1;
        int temp_agent = -1;

        if (agents_[agt].getCurrent() == swapping_cycles[0]) {
            front=agt;
            bk=swap_agt;
        }
        if (agents_[swap_agt].getCurrent() == swapping_cycles[0]) {
            front=swap_agt;
            bk=agt;
        }

        std::cout<<"C = {";
        for (int i=0; i<swapping_cycles.size(); i++)
            std::cout<<swapping_cycles[i]<<",";
        std::cout<<"}\n";


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Save this index to seperate the stacking steps from multipushing swapping agents inside cycle.
        index=temp_actions_.size();

        // Push agent inside the cycle
        std::vector<unsigned int> unpushable;

        p1.clear();
        p1.push_back(swapping_cycles[0]);
        p1.push_back(swapping_cycles[1]);
        std::cout<<"stacking enter agent at loc :"; std::cout<<agents_[front].getCurrent();
        if (push (front, p1, unpushable))
            std::cout<<" to loc :"; std::cout<<agents_[front].getCurrent()<<std::endl;


        // Push swap_agent inside the cycle
        int dist = 0, source; // the distinaction is the entering node
        source=agents_[bk].getCurrent();

        p1.clear();
        p1.push_back(agents_[bk].getCurrent());
        p1.push_back(swapping_cycles[0]);
        std::cout<<"stacking before-enter at loc :"; std::cout<<agents_[bk].getCurrent();
        if (push (bk, p1, unpushable));
            std::cout<<" to loc :"; std::cout<<agents_[bk].getCurrent()<<std::endl;
            return index;

     }

bool PushAndSpin::stepForward (int agt, int node_in_graph) {
    std::vector <unsigned int> tiny_path;
    tiny_path.push_back (agents_[agt].getCurrent()); // current loc
    tiny_path.push_back (node_in_graph);            // next loc
    addTemporaryAction(agents_[agt].getID(), tiny_path);
    //std::cout<<"addTemporaryAction(agents_[agt].getID(), tiny_path);\n";
    //std::cout<<tiny_path[0]<<",";
    //std::cout<<tiny_path[1]<<"\n";
    agents_[agt].setCurrent(node_in_graph);
    occupied_.insert(agents_[agt].getCurrent());
    return true;
}


bool PushAndSpin::executeRotate (unsigned int agt,unsigned int swap_agent,unsigned int entering_nodes,std::vector <unsigned int >swapping_cycles, int rot_num){
        int degree = swapping_cycles.size();
        int start_simultanous=-1 ,end_simultanous=-1;

        //std::cout<<"Will start at ";
        //std::cout<<bebble<<std::endl;


        for (int cntr=0; cntr<rot_num ; cntr++)
        { // One rotate :
            std::vector <int> indexes;
            // Release all occupied nodes in the cycle "simulatnous implementation"
            for (int i=0 ; i<swapping_cycles.size(); i++)
                if (std::find(occupied_.begin(), occupied_.end(), swapping_cycles[i]) != occupied_.end())
                    for (int a=0; a<agents_.size(); a++)
                        if (agents_[a].getCurrent() == swapping_cycles[i]){
                            // Save occupying agent in a list
                            indexes.push_back (a);
                            occupied_.erase(agents_[a].getCurrent());
                            break;
                            }

            start_simultanous = temp_actions_.size();
             // for all occupying agents in the list
             for (size_t z =0; z<indexes.size (); z++) {
                        int current_loc = agents_[indexes[z]].getCurrent();
                        for (int c=0; c<swapping_cycles.size(); c++)
                            // switch to cycle numbering
                            if (current_loc == swapping_cycles[c]) {
                                current_loc = c;
                                break;
                            }
                            if (!stepForward (indexes[z],swapping_cycles[(degree+(current_loc+1))%degree]))
                                return false;
                    }

            end_simultanous = temp_actions_.size()-1;
            //std::cout<<"Finish a rotate <"<<start_simultanous;
            //std::cout<<","<<end_simultanous<<">"<<std::endl;

            simulatnous_moves_indexes.push_back(std::make_pair(start_simultanous,end_simultanous));
        }

        return true;
    }


bool CB=false;


bool PushAndSpin::bb_case (unsigned int front, unsigned int bk,const std::vector <unsigned int> &shortest_path,std::vector <unsigned int >swapping_cycles,unsigned int entering_nodes, int direction ) {
    std::cout<<"==========================\nCase. BB\n==========================\n";
    std::cout<<"We will rotate :";
    std::cout<<agents_[front].getID();
    std::cout<<" and ";
    std::cout<<agents_[bk].getID();
    std::cout<<std::endl;
    std::cout<<"with loc:";
    std::cout<<agents_[front].getCurrent();
    std::cout<<" and ";
    std::cout<<agents_[bk].getCurrent();
    std::cout<<std::endl;

    //////////////////////////////////////////////////////
    int sequence_index_start_push_swap = temp_actions_.size()-1;
    //////////////////////////////////////////////////////

    //================//
    bool successful_clear= false;
    //================//
    std::cout<<"Swapping cycle = [";
    for (int i=0; i<swapping_cycles.size(); i++)
        std::cout<<swapping_cycles[i]<<",";
    std::cout<<"] ,enter="<<entering_nodes<<"\n";

    // setting node before-entering node
    std::vector<unsigned int> nbrs;
    int before_entering_node =-1;
    graph_->getNeighbors(entering_nodes,nbrs);
    for (int n=0; n<nbrs.size(); n++)
        if (std::find(swapping_cycles.begin(),swapping_cycles.end(),nbrs[n])==swapping_cycles.end()) {
            before_entering_node = nbrs[n];
            std::cout<<"entering_node"<<entering_nodes<<"\n";
            std::cout<<"before_entering_node"<<before_entering_node<<"\n";

        }

    if (multipush(front, bk,entering_nodes,before_entering_node)){
                    std::cout<<"Done until multipush agt#"<<front<<" at "<<agents_[front].getCurrent()<<", swap_agt="<<bk<<" at "<<agents_[bk].getCurrent()<<"\n";
                    successful_clear = true;
    }
    //////////////////////////////////////////////////////
    int end_rev_agent = temp_actions_.size()-1;
    //////////////////////////////////////////////////////

    std::cout<<"Rotate by#"<<swapping_cycles.size()-1<<"\n";
    if (successful_clear)
    stacking_cycle (front, bk, direction,entering_nodes,swapping_cycles,bk);

    //////////////////////////////////////////////////////
    int end_rev_non_agent = temp_actions_.size()-1;
    //////////////////////////////////////////////////////

    //std::cout<<"Stacking done, agt="<<agents_[front].getCurrent()<<", swap_agt="<<agents_[bk].getCurrent()<<"\n";
     if (successful_clear
             && executeRotate (front,bk, entering_nodes,swapping_cycles,swapping_cycles.size()-1))
     {
             std::vector <unsigned int> path;
             path.push_back(agents_[front].getCurrent());
             path.push_back(before_entering_node);
             std::vector<unsigned int> unpushable;
             if (push (front, path, unpushable)
              && executeRotate (front,bk, entering_nodes,swapping_cycles,1))
             {
                 path.clear();
                 path.push_back(agents_[bk].getCurrent());
                 path.push_back(entering_nodes);
             if  (push (bk, path, unpushable))
                {

                    reversePaths_cc_bb (front, bk, sequence_index_start_push_swap , end_rev_agent,  end_rev_non_agent, true);
                    return true;
                }
            }
     }

     /*
     std::cout<<"Done! rotate :";
     std::cout<<agents_[front].getID();
     std::cout<<" and ";
     std::cout<<agents_[bk].getID();
     std::cout<<std::endl;
     std::cout<<"with loc:";
     std::cout<<agents_[front].getCurrent();
     std::cout<<" and ";
     std::cout<<agents_[bk].getCurrent();
     std::cout<<std::endl;
     */
     return false;

}

bool PushAndSpin::cb_case (unsigned int front, unsigned int bk,const std::vector <unsigned int> &shortest_path,std::vector <unsigned int >swapping_cycles,unsigned int entering_nodes, int direction  ) {
    std::cout<<"==========================\nCase. CB\n==========================\n";
    std::cout<<"We will rotate :";
    std::cout<<agents_[front].getID();
    std::cout<<" and ";
    std::cout<<agents_[bk].getID();
    std::cout<<std::endl;
    std::cout<<"with loc:";
    std::cout<<agents_[front].getCurrent();
    std::cout<<" and ";
    std::cout<<agents_[bk].getCurrent();
    std::cout<<std::endl;
    //======================================//
    std::cout<<"Swapping cycle [";
    for (int i=0; i<swapping_cycles.size();i++)
        std::cout<<swapping_cycles[i]<<",";
    std::cout<<"], enter="<<entering_nodes<<"\n";
    CB = true;
    //////////////////////////////////////////////////////
    int sequence_index_start_push_swap = temp_actions_.size()-1;
    int end_rev_agent = sequence_index_start_push_swap;
    //////////////////////////////////////////////////////
    int before_entering_node = agents_[bk].getCurrent();
    //======================================//

    stacking_cycle (front, bk, direction,entering_nodes,swapping_cycles,bk);

    //////////////////////////////////////////////////////
    int end_rev_non_agent = temp_actions_.size()-1;
    //////////////////////////////////////////////////////

    std::cout<<"Stacking done, agt="<<agents_[front].getCurrent()<<", swap_agt="<<agents_[bk].getCurrent()<<"\n";
     if (executeRotate (front,bk, entering_nodes,swapping_cycles,swapping_cycles.size()-1))
     {
             std::vector <unsigned int> path;
             path.push_back(agents_[front].getCurrent());
             path.push_back(before_entering_node);
             std::vector<unsigned int> unpushable;
             if (push (front, path, unpushable)
              && executeRotate (front,bk, entering_nodes,swapping_cycles,1))
             {
                 path.clear();
                 path.push_back(agents_[bk].getCurrent());
                 path.push_back(entering_nodes);
             if  (push (bk, path, unpushable))
                {
                    std::cout<<"Now: front"<<agents_[front].getCurrent()<<", bk="<<agents_[bk].getCurrent()<<"\n";
                    reversePaths_cc_bb (front, bk, sequence_index_start_push_swap , end_rev_agent,  end_rev_non_agent, false);
                    return true;
                }
            }
     }
    /*
    std::cout<<"Done! rotate :";
    std::cout<<agents_[front].getID();
    std::cout<<" and ";
    std::cout<<agents_[bk].getID();
    std::cout<<std::endl;
    std::cout<<"with loc:";
    std::cout<<agents_[front].getCurrent();
    std::cout<<" and ";
    std::cout<<agents_[bk].getCurrent();
    std::cout<<std::endl;*/
        return false;
}

bool PushAndSpin::cc_case (unsigned int agt, unsigned int swap_agent,const std::vector <unsigned int> &shortest_path, int c_index) {
    CB=false;
    std::cout<<"==========================\nCase. CC\n==========================\n";
    std::cout<<"We will rotate :";
    std::cout<<agents_[agt].getID();
    std::cout<<" and ";
    std::cout<<agents_[swap_agent].getID();
    std::cout<<std::endl;
    std::cout<<"with loc:";
    std::cout<<agents_[agt].getCurrent();
    std::cout<<" and ";
    std::cout<<agents_[swap_agent].getCurrent();
    std::cout<<std::endl;
    std::vector<unsigned int> nbrs;


    // 1. find the cycle they implies to (cycle)
    int cycle =c_index;

    // 2. find the entering node <find node with outer neighbourhood> (enter)
     std::vector<int> enter;
     int front_index_in_cycle = -1;

     for (int i=0; i<cycles_index[cycle].size(); i++) {
         nbrs.clear();
         graph_->getNeighbors ( cycles_index[cycle][i], nbrs);
         for (int n=0; n<nbrs.size(); n++) // all nbrs for enter
         if ( std::find(cycles_index[cycle].begin(), cycles_index[cycle].end(), nbrs[n]) == cycles_index[cycle].end()) {
             // if one of them is outer
             enter.push_back(cycles_index[cycle][i]);
             break; // one outer nbr is enoguh to consider it enter node
         }
     }

     //Sort enters
     int index=0; int best =enter[index];
     for (int i=0; i<enter.size(); i++) {
         nbrs.clear();
         graph_->getNeighbors (enter[i], nbrs);
         for (int n=0; n<nbrs.size(); n++) // all nbrs for enter
         if ( std::find(occupied_.begin(), occupied_.end(), nbrs[n]) == occupied_.end()) {
             // if one of them is free
             index = i;
             best = enter[index];
             std::cout<<enter[index]<<" is the better entering nodes since it contains free neighbor \n";
             break; // one outer nbr is enoguh to consider it enter node
         }
     }

     // heuristic purpose
     if (index!=0) { //updeated
         int temp_enter= enter[index];
         enter.erase(enter.begin()+index);
         std::vector <int> temp_enter_cycle;
         temp_enter_cycle.push_back(temp_enter);
         for (int t=0; t<enter.size(); t++)
             temp_enter_cycle.push_back (enter[t]);
         enter.clear();
         enter = temp_enter_cycle;
     }
     /////////////////////////////////////////////////////////////////////////////////////////
     // TO BE MOVED TO ROTATE


    bool successful_swap = false;
    bool done=false;
    for (int t=0; t<enter.size() && !successful_swap && !done ; t++) {


     int sequence_index_start_push_swap = temp_actions_.size()-1;

    //std::cout<<"Enter node ="<<enter[t]<<"\n";

     // 3. set temp_cycle
    std::vector <unsigned int> temp_cycle ;
    int ptr, start, end;
    /* setting start (node next to entering node) and end (node before entering node)*/
    for (int z=0 ; z<cycles_index[cycle].size() ; z++) {
        //std::cout<<cycles_index[cycle][z]<<",";
        if (cycles_index[cycle][z] == enter[t] ) {
            if (z!=0) {
             start = cycles_index[cycle][(z+1)%cycles_index[cycle].size()];
             end = cycles_index[cycle][(z-1)%cycles_index[cycle].size()];
            }
            else {
                start = cycles_index[cycle][(z+1)%cycles_index[cycle].size()];
                end = cycles_index[cycle][cycles_index[cycle].size()-1];
            }
        }
    }
    //std::cout<<"We will start at "<<start<<" ,and end at "<<end<<"\n";
    ptr = start; // starting from the first nighbour of enter_node
    int x=-1;
    for (int c=0; c<cycles_index[cycle].size(); c++)
        if (cycles_index[cycle][c] == start)
            x=c;

    do{
        temp_cycle.push_back (cycles_index[cycle][x%cycles_index[cycle].size()]);
        ptr = cycles_index[cycle][x%cycles_index[cycle].size()];
        x++;
    }while (ptr != end) ;
    temp_cycle.insert(temp_cycle.begin(),enter[t]);

    int enter_in_cycle =-1;
    for (int i=0 ; i<temp_cycle.size(); i++)
        if (temp_cycle[i]== enter[t])
            enter_in_cycle= i ;

    std::cout<<"Swapping cycle [";
    for (int x=0; x<temp_cycle.size(); x++)
        std::cout<<temp_cycle[x]<<",";
    std::cout<<"], enter="<<enter[t]<<"\n";

    // 4. prepare before enter node
    int sequence_index_start=-1, sequence_index_end=-1;
    bool method1=true,method2=true;
    int before_enter_node=-1;
        nbrs.clear();
        graph_->getNeighbors ( enter[t], nbrs);
        for (int n=0; n<nbrs.size(); n++)
            if (std::find (temp_cycle.begin(), temp_cycle.end(), nbrs[n]) == temp_cycle.end() )
                before_enter_node=nbrs[n];

        for (int n=0; n<nbrs.size(); n++) // to prioritize the free before_enter_node
            if (std::find (temp_cycle.begin(), temp_cycle.end(), nbrs[n]) == temp_cycle.end())
                if(std::find (occupied_.begin(), occupied_.end(), nbrs[n]) == occupied_.end() )
                    before_enter_node=nbrs[n];// before entering node



    if (std::find(occupied_.begin(), occupied_.end(), before_enter_node) != occupied_.end()) { // occupied !
        //std::cout<<"before enter is ="<<before_enter_node<<"\n";
        done=false;method1=false;method2=false;
        int agent_in_before =-1;
        for (int a=0; a<agents_.size(); a++) if (agents_[a].getCurrent()==before_enter_node) agent_in_before = a;
        // (1) Try to push far away
        nbrs.clear();
        graph_->getNeighbors ( before_enter_node, nbrs);
        for (int n=0; n<nbrs.size() ; n++)
            if (nbrs[n]==enter[t])
                nbrs.erase(nbrs.begin()+n); // exclude enter node
        int n=0;
        sequence_index_start = temp_actions_.size()-1;
        while (!done && n<nbrs.size()) {
            std::vector<unsigned int> unpushable;
            std::vector<unsigned int> step;
            step.push_back (agents_[agent_in_before].getCurrent());
            step.push_back (nbrs[n]);
            for (int c=0 ; c<temp_cycle.size(); c++)
                unpushable.push_back (temp_cycle[c]);
            if (push (agent_in_before,step,unpushable)) {
                //std::cout<<"Agent#"<<agent_in_before<<" has been moved to Node#"<<nbrs[n]<<" successfully(1)\n";
                done = true;
                method1=true;
            }
            n++;
        }
        // (2) Try to push inside the cycle
        if (!done){
            method1=false;

            int steps =0;
            // 1- check if any of swapping agents on the entering node
            if ((agents_[agt].getCurrent() == temp_cycle[0] && agents_[swap_agent].getCurrent() == temp_cycle[temp_cycle.size()-1])
                || agents_[swap_agent].getCurrent() == temp_cycle[0] && agents_[agt].getCurrent() == temp_cycle[temp_cycle.size()-1])
                steps=2;

            if ((agents_[agt].getCurrent() == temp_cycle[0] && agents_[swap_agent].getCurrent() == temp_cycle[1])
                || agents_[swap_agent].getCurrent() == temp_cycle[0] && agents_[agt].getCurrent() == temp_cycle[1])
                steps=1;


            // Push before_enter_agent to enrer node
            std::vector <unsigned int> to_cycle;std::vector<unsigned int> unpushable;
            to_cycle.push_back(before_enter_node);
            to_cycle.push_back(enter[t]);
            if (executeRotate (-1,-1, enter[t], temp_cycle, steps)&& push(agent_in_before, to_cycle,unpushable))
            {
                done=true;
                method2=true;
            }

        /*
            int l=0, free_l=-1;
            for (int ptr=enter_in_cycle; ptr<temp_cycle.size() ; ptr++, l++) {
                if (std::find(occupied_.begin(), occupied_.end(), temp_cycle[ptr]) == occupied_.end()){
                    free_l = temp_cycle [ptr];
                    //std::cout<<"There is free node at#"<<free_l<<" at distance ="<<l;
                    break;
                }
            }

            if (free_l!=-1)
                if (executeRotate (1,1, enter[t], temp_cycle, l)){
                    done=true;
                    method2=true;
                }
         // Push before_enter_agent to enrer node
         std::vector <unsigned int> to_cycle;std::vector<unsigned int> unpushable;
         to_cycle.push_back(enter[t]);
         push(agent_in_before, to_cycle,unpushable);
         //std::cout<<"Agent#"<<agent_in_before<<" has been moved to Node#"<<enter[t]<<" successfully(2)\n";
        */
        }

        if (done) {
            //std::cout<<"Cleared \n";
            sequence_index_end  = temp_actions_.size()-1;
        }
    }
    else done = true;

    // 5. set front and back agents
    int front = -1, bk =-1, agt_loc=-1, swap_loc =-1;

    for (int x=0; x<temp_cycle.size(); x++)
    {
        if (agents_[agt].getCurrent() ==temp_cycle[x])
            agt_loc = x;
        if (agents_[swap_agent].getCurrent()==temp_cycle[x])
            swap_loc = x;
    }

    //std::cout<<"agt_loc ="<<agt_loc<<", swap_loc ="<<swap_loc<<"\n";
      // std::cout<<"agents_[swap_agent].getCurrent() ="<<agents_[swap_agent].getCurrent()<<"\n";

    if (agt_loc > swap_loc)  { front = agt ;bk = swap_agent ; }
    else { front = swap_agent;bk = agt ; }
    if (agents_[swap_agent].getCurrent()==enter[t] && agents_[agt].getCurrent()==temp_cycle[temp_cycle.size()-1])
        { front = swap_agent;bk = agt ; }
    if (agents_[agt].getCurrent()==enter[t] && agents_[swap_agent].getCurrent()==temp_cycle[temp_cycle.size()-1])
        { front = agt;bk = swap_agent ; }

    //std::cout<<"front ="<<front<<", back ="<<bk<<"\n";

    for (int i=0 ; i<temp_cycle.size(); i++)
        if (temp_cycle[i]== agents_[front].getCurrent())
            front_index_in_cycle = i ;


    // 6. rotate cycle [ start ---> enter]
     std::vector <unsigned int> p1;
     int cntr=0;
     for (int i=front_index_in_cycle+1; i<= temp_cycle.size()-1; i++, cntr++) {
         if ( i != enter_in_cycle ) {
             p1.push_back(temp_cycle[i]);
         }
     }
     p1.push_back(enter[t]);
    if (agents_[front].getCurrent() == enter[t])
        p1.clear();

    /*std::cout<<"Path= [";
    for (int i=0 ; i<p1.size(); i++)
        std::cout<<p1[i]<<",";
    std::cout<<"]\n";*/

    int rot = p1.size();
    //std::cout<<"Rotate by="<<rot<<"\n";

    if (done )
        if (!executeRotate (front,bk, enter[t], temp_cycle, rot))
            successful_swap = false;

    // 7. multipush out


    if (done )
        if (!multipush (front,bk,before_enter_node,enter[t]))
            successful_swap =false;

    // One rotate
    rot = 1;
    //std::cout<<"Rotate by="<<rot<<"\n";
    if (done )
        if (!executeRotate (bk, front, enter[t],temp_cycle, rot))
                    successful_swap = false;

    // Push inside
    std::vector<unsigned int> step;
    step.push_back (enter[t]);
    if (done ) push(front, step, unpushable_);

    // 8. rotate cycle [ enter ---> end ]
    rot = temp_cycle.size()-p1.size()-1;
    //std::cout<<"Rotate by="<<rot<<"\n";
    if (done ) {
        if (!executeRotate (bk, front, enter[t],temp_cycle, rot))
                    successful_swap = false;
        else
            successful_swap = true;

    }

    int num=0;

    // TO BE MOVED TO ROTATE
    if (done &&successful_swap) {
        if (method1)
            reversePaths_cc_bb(agt, swap_agent, sequence_index_start , num,  sequence_index_end, false);
        if (method2)
            reversePaths_cc(agt, swap_agent, sequence_index_start , num,  sequence_index_end, false);
    }

   // TO BE MOVED TO ROTATE
    if (!successful_swap) {
        // Start over (reverse everybody out and delete from temporary sequence)
        deleteTempActions(sequence_index_start_push_swap+1);
        }



    } // end for (t=0; t<enter; t++)


    return successful_swap;

}

bool PushAndSpin::rotate(unsigned int agt, const std::vector <unsigned int> &shortest_path){
    // (r) - (s) ------------------------------------> G(r)
    // agt is the forend agent (r)
    // we want to find the swap agent (s)
    // shortest_path is the shortest path of agent (r)
    assert(shortest_path.size () > 1);
    unsigned int swap_agent = NO_AGENT;
    for (size_t i = 0; i < agents_.size () && swap_agent == NO_AGENT; i++)
    {
        if (agents_[i].getCurrent() == shortest_path[1] )
            swap_agent = i;
    }
    // Be sure that the agnt (r) is at shortest-path[0],
    // Be sure that the swap_agent (s) is at shortest_path[1]
    // Be sure that there is an edge between (r) and (s)
    assert(agents_[agt].getCurrent() == shortest_path[0]);
    assert(swap_agent != NO_AGENT);
    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agent].getCurrent()));
    std::vector <std::vector  <unsigned int> >attempted_swap_vertices;
    bool successful_swap = false;
    //***************************************************************************************//
    //                                                                                       //
    //                                  Here I can made a change                             //
    //                                                                                       //
    //***************************************************************************************//
    // CASE.2: If the swapping agents are inside the cycle
    int cc=false; int c_index=-1;
    for (int c=0; c<cycles_index.size(); c++) {
        if ((std::find (cycles_index[c].begin(),cycles_index[c].end(),agents_[agt].getCurrent())!=cycles_index[c].end())
            && (std::find (cycles_index[c].begin(),cycles_index[c].end(),agents_[swap_agent].getCurrent())!=cycles_index[c].end())) {
                 cc=true;
                 c_index=c;
        }
    }
    if (cc) // CC case
    {
        std::cout<<"CC\n";
        int sequence_index_start_push_swap = temp_actions_.size()-1;
        successful_swap = cc_case (agt,swap_agent,shortest_path,c_index);

       if (successful_swap && agents_[swap_agent].getCurrent()!= agents_[swap_agent].getGoal() && (std::find(unpushable_.begin(),unpushable_.end(),agents_[swap_agent].getGoal())!=unpushable_.end())) {
           /*std::cout<<"Agent #";
           std::cout<<agents_[swap_agent].getID();
           std::cout<<" is added to to_resolve() \n";
           if (std::find (to_resolve.begin(),to_resolve.end(),swap_agent)==to_resolve.end())
                   to_resolve.push_back(swap_agent);*/
           resolve(agt, swap_agent, shortest_path, unpushable_);
       }

       if (!successful_swap) {
           deleteTempActions(sequence_index_start_push_swap+1);
           std::cout<"\n\nRotate IS NOT Done !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n";
       }

    }

    // CASE.1&3 : If the swapping agents are on the bridge , If the swapping agents are on the entering node
    else { //CB and BB cases
    std::cout<<"CB or BB\n";
    int x = agt, y=swap_agent;
    std::vector < std::vector <unsigned int> >swapping_cycles;
    std::vector<unsigned int> entering_nodes;  // the entering node for the instance <agnt,swap_agnt> should be set each time related to the instance position
    computeSwapCycle(agents_[agt].getCurrent(),agents_[swap_agent].getCurrent(),
                     swapping_cycles,attempted_swap_vertices,entering_nodes);

    for (int i =0 ; i <swapping_cycles.size() && !successful_swap &&
         attempted_swap_vertices.size() <swapping_cycles.size() ; i++ ) {
                                // Pushing to swap vertex and clearing two neighbors
                                // multipush = push + clear_vertex ! I need to define clear(agnt,swap_agnt,entering_node)
                                // that moves all fa toawrd the swapping_cycle

    CB=false;
    int front = -1, bk =-1 ;

        if (entering_nodes[i] == agents_[agt].getCurrent() ) { front = agt; bk = swap_agent ; CB=true; }
        if (entering_nodes[i] == agents_[swap_agent].getCurrent() ) { front = swap_agent; bk = agt ; CB=true; }

        std::vector<unsigned int> temp1,temp2;
        graph_->AStar(agents_[agt].getCurrent(), entering_nodes[i], temp1);
        graph_->AStar(agents_[swap_agent].getCurrent(), entering_nodes[i], temp2);
        if (temp1.size() < temp2.size())
             { front = agt ;bk = swap_agent ; }
        else { front = swap_agent;bk = agt ; }
        int direction=-1;

    int sequence_index_start_push_swap = temp_actions_.size()-1;
    // NOW we will differentiate between CB and BB
    if (CB)
        successful_swap = cb_case(front, bk,shortest_path,swapping_cycles[i],entering_nodes[i], direction ) ;
    if (!CB)
        successful_swap = bb_case (front, bk,shortest_path,swapping_cycles[i],entering_nodes[i], direction) ;



    if (!successful_swap) {
        attempted_swap_vertices.push_back(swapping_cycles[i]);
        std::sort(attempted_swap_vertices.begin (), attempted_swap_vertices.end ());
        // Start over (reverse everybody out and delete from temporary sequence)
        deleteTempActions(sequence_index_start_push_swap+1);
        to_resolve.push_back(agt);
        }
    }

    /*
    std::cout<<"Agents at end of this operator :\n";
    for (int a=0;a<agents_.size(); a++)
        std::cout<<agents_[a].getID()<<":"<<agents_[a].getCurrent()<<"\n";*/
    // resolve BB
    if (successful_swap && !CB && agents_[y].getCurrent() != agents_[y].getGoal() && std::find (unpushable_.begin(),unpushable_.end(),agents_[y].getGoal())!=unpushable_.end())
    {

        resolve(x, y, shortest_path, unpushable_);
        /*
        std::cout<<"================RESOLVE ================\n";
        std::cout<<"Resolving #"<<agents_[y].getID()<<"\n";
        std::cout<<"Agent #";
        std::cout<<agents_[swap_agent].getID();
        std::cout<<" is added to to_resolve() \n";
        if (std::find (to_resolve.begin(),to_resolve.end(),swap_agent)==to_resolve.end())
                to_resolve.push_back(swap_agent);
        std::cout<<"================RESOLVE END================\n";*/

    }
    // resolve CB
    if (successful_swap && CB && agents_[y].getCurrent()!= agents_[y].getGoal() && (std::find(unpushable_.begin(),unpushable_.end(),agents_[y].getGoal())!=unpushable_.end())) {
            /*std::cout<<"Agent #";
            std::cout<<agents_[swap_agent].getID();
            std::cout<<" is added to to_resolve() \n";
            if (std::find (to_resolve.begin(),to_resolve.end(),swap_agent)==to_resolve.end())
                    to_resolve.push_back(swap_agent);*/
            resolve(agt, swap_agent, shortest_path, unpushable_);

    }
    std::cout<"\n\nRotate Done !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n";



    }

    if (agents_[agt].getCurrent() == agents_[agt].getGoal()) {
    addToUnpushable (agents_[agt].getCurrent());
    successful_swap = true;
    }
    if (agents_[swap_agent].getCurrent() == agents_[swap_agent].getGoal()) {
    addToUnpushable (agents_[swap_agent].getCurrent());
    }

    return successful_swap;
}


void PushAndSpin::addToUnpushable (int node) {
    bool found = false ;
    for (int i=0 ; i<unpushable_.size(); i++)
        if (unpushable_[i] == node)
            found = true;

    if (!found) {
        /*std::cout<<"Node#";
        std::cout<<node;
        std::cout<<" is addToUnpushable";
        std::cout<<std::endl;*/
        unpushable_.push_back (node);}
}

void PushAndSpin::deleteFromUnpushable (int node){
        int index =-1;
        for (int i=0 ; i<unpushable_.size() ; i++)
            if (unpushable_[i] == node)
                index = i ;

        if (index != -1)
            unpushable_.erase (unpushable_.begin(), unpushable_.begin()+index);

}

bool PushAndSpin::executePushAndSpinUtil(int i) {
    std::vector<unsigned int> shortest_path;

    if (agents_[i].getCurrent() == agents_[i].getGoal ()) {
        addToUnpushable (agents_[i].getCurrent());
        std::cout<<"Agent#"<<i<<" is already at its goal\n";
        return true;
    }

    // Push and Swap until agent i is at its goal
    while (agents_[i].getCurrent() != agents_[i].getGoal())
    {
        // Check if there is a "path of least resistance"
        shortest_path.clear();

        std::set<unsigned int> targets;
        targets.insert(agents_[i].getGoal());

        // THIS IS ME : EBTEHAL !! I have commented //graph_->BrFS(agents_[i].getCurrent(), targets, shortest_path, unpushable_);
        // because i want is find the shortest path ever, is it cannt push, it should swap
        //graph_->BrFS(agents_[i].getCurrent(), targets, shortest_path, unpushable_);
        std::vector<unsigned int> temp_unpushable = unpushable_;
        unpushable_.clear();

        for (std::set<unsigned int>::iterator o = occupied_.begin (); o != occupied_.end (); o++)
            unpushable_.push_back (*o);

        // try to find free shortest path
        graph_->AStar(agents_[i].getCurrent(), agents_[i].getGoal(), shortest_path, unpushable_); // find the path with least number of finished agents
        if (shortest_path.size() == 0) {// No path of least resistance.  Just use normal shortest path
            unpushable_.clear();
            unpushable_ = temp_unpushable;
            graph_->AStar(agents_[i].getCurrent(), agents_[i].getGoal(), shortest_path, unpushable_);
            } // if impossible, find the shortest and do Swap()
         // find the path with least number of finished agents
        if (shortest_path.size() == 0) // No path of least resistance.  Just use normal shortest path
            graph_->AStar(agents_[i].getCurrent(), agents_[i].getGoal(), shortest_path);    // if impossible, find the shortest and do Swap()

        if (shortest_path.size() == 0) // No path at all for agent i to get to its goal
            return false;

        // Push
        std::cout<<"\n\n****************\n";
        std::cout<<"We will push agent ID ";
        std::cout<<agents_[i].getID();
        std::cout<<"\n****************\n";
        int cur = agents_[i].getCurrent();
        std::cout<<std::endl;
        std::cout<<"shortest_path =[";
        for (int x=0; x<shortest_path.size(); x++)
            std::cout<<shortest_path[x]<<",";
        std::cout<<"]\n";

        if (push(i, shortest_path, unpushable_))
        {
            std::cout<<"\nPUSH IS DONE";
            // Agent pushed to its goal successfully
            if (agents_[i].getCurrent() == agents_[i].getGoal()) {
                addToUnpushable (agents_[i].getCurrent());
            }


        }
        else {

            while (shortest_path[0] != agents_[i].getCurrent())
                shortest_path.erase(shortest_path.begin ());

            unsigned int swap_agent = NO_AGENT;
            for (size_t ii = 0; ii < agents_.size () && swap_agent == NO_AGENT; ii++)
            {
                if (agents_[ii].getCurrent() == shortest_path[1] )
                    swap_agent = ii;
            }

            if (rotate (i,shortest_path))
            {   // Agent rotated to its goal successfully
                if (agents_[i].getCurrent() == agents_[i].getGoal()) {
                    addToUnpushable (agents_[i].getCurrent());
                }

            }
            else {
                std::cout<<"ROTATE FAILED !!!!!";
                std::cout<<std::endl;
                return false;
            }
        }
    }


    if (agents_[i].getCurrent() == agents_[i].getGoal())
    {
        addToUnpushable (agents_[i].getCurrent());
        actions_.insert (actions_.end(), temp_actions_.begin(), temp_actions_.end());
        // condense
        PushAndSpin::Plan p ;
        for (int x=0; x<temp_actions_.size(); x++) {
            bool sim=false;
            for (int y=0; y<simulatnous_moves_indexes.size(); y++)
                if (x>=simulatnous_moves_indexes[y].first && x<=simulatnous_moves_indexes[y].second ) {
                    //std::cout<<"<"<<simulatnous_moves_indexes[y].first<<","<<simulatnous_moves_indexes[y].second<<">\n";
                    for (int z=(simulatnous_moves_indexes[y].first); z<=simulatnous_moves_indexes[y].second; z++)// fill
                            p.push_back(temp_actions_[z]);
                    x=simulatnous_moves_indexes[y].second;
                    sim= true;
                    break;
                }
            if (!sim && x<temp_actions_.size()) {
                p.push_back(temp_actions_[x]);
            }
            std::pair<unsigned int,PushAndSpin::Plan > temp_path = std::make_pair (ID++,p);
            condense.push_back (temp_path);
            p.clear();

        }
        //
        simulatnous_moves_indexes.clear();
        temp_actions_.clear ();
    }

    else return false;



    return true;
}

// Push and Swap outer loop.  Iterate over all agents: push and then swap.  Repeat until at goal.
bool PushAndSpin::executePushAndSpin()
{

    occupied_.clear();
    for (size_t i = 0; i < agents_.size (); i++) {
        occupied_.insert(agents_[i].getCurrent());

        /*
        if (agents_[i].getCurrent() == agents_[i].getGoal ()) {
            addToUnpushable (agents_[i].getCurrent());
        }*/
    }
    // condense
    condense.clear();
    actions_.clear();
    temp_actions_.clear();

    bool solution=  true;


    std::vector <Agent> agent ;
    for (size_t i = 0; i < agents_.size (); i++)
        agent.push_back(agents_[i]);


    for (size_t i = 0; i < agents_.size (); i++)
    {
        if (executePushAndSpinUtil(i)){
            solution = true;
            //Resolving
            // Resolving CC and CB agents in LIFO manner
            for (int j=to_resolve.size()-1; j>=0; j--) {
                if (executePushAndSpinUtil(to_resolve[j]))
                    to_resolve.erase(to_resolve.begin()+j);
            }

            // TO STUDY ACTIONS_ each time an agent reach:
            /*
            std::cout<<"=================================|| Locations Now ||================================="<<std::endl;
            for (int x=0 ; x<actions_.size(); x++) {
                std::cout<<"Agent";
                std::cout<<actions_[x].first;
                std::cout<<" = [ ";
                for (int j= 0 ; j<actions_[x].second.size() ; j++) {
                    std::cout<<actions_[x].second[j];
                    std::cout<<" , ";
                }
                std::cout<<" ] ";
                std::cout<<std::endl;

            }*/
        }
        else {
            solution = false;
        }
    }




    return solution;
}

// This resolves the condition where agt swaps with swap_agt, who was already at its goal.
bool PushAndSpin::resolve(unsigned int agt, unsigned int swap_agt,
                          std::vector <unsigned int> shortest_path,
                          std::vector <unsigned int> _unpushable)
{
    unsigned int pre_push_index ;

    if (agents_[swap_agt].getCurrent() == agents_[swap_agt].getGoal() )
        return true;

    bool flag = false;

    // Try a simple push
    std::vector <unsigned int> tiny_path;
    if (agents_[agt].getCurrent() == shortest_path[1]) // called from swap
    {
        tiny_path.push_back (agents_[agt].getCurrent());
        if (shortest_path.size() > 2)
            tiny_path.push_back (shortest_path[2]);

        // unpushable is the set of agents at their goal except for swap_agt
        std::vector <unsigned int> temp_unpushable;
        for (size_t i = 0; i < _unpushable.size (); i++)
        {
            if (_unpushable[i] != agents_[swap_agt].getGoal ())
                temp_unpushable.push_back (_unpushable[i]);
        }

        // add back swap_agt at its current position
        if (std::find(_unpushable.begin(), _unpushable.end(), agents_[swap_agt].getCurrent()) == _unpushable.end())
            temp_unpushable.push_back (agents_[swap_agt].getCurrent());

        // Push agt one more step.  If success, move swap_agt
        pre_push_index = temp_actions_.size ();

        if (shortest_path.size () >= 3 && push(agt, tiny_path, temp_unpushable))
        {
            tiny_path.clear ();
            tiny_path.push_back (agents_[swap_agt].getCurrent());
            tiny_path.push_back (agents_[swap_agt].getGoal());

            if ( push(swap_agt, tiny_path, unpushable_))
            flag = true;

        }
    else
    {
            flag = false;
        // swap agt along path
        deleteTempActions(pre_push_index);

        tiny_path.clear ();
        tiny_path.push_back (agents_[swap_agt].getCurrent());
        tiny_path.push_back (agents_[swap_agt].getGoal());



        while (shortest_path[0] != agents_[agt].getCurrent())
            shortest_path.erase(shortest_path.begin());
        // Swap agt one more time
        /*
        if (rotate(agt, shortest_path))
        {
            if (push(swap_agt, tiny_path, unpushable_) || rotate(agt, shortest_path))
            {
                // At this point, swap_agt should be back at goal
                return true;
            }
        }*/
     }
    }

    if (!flag) {
        if (std::find (to_resolve.begin(),to_resolve.end(),swap_agt)==to_resolve.end()) {
            to_resolve.push_back(swap_agt);
        }
    }

    return flag;
}


bool PushAndSpin::solve()
{
    // Sanity checks
    if (agents_.size() == 0)
        return true;
    if (graph_ == NULL)
    {
        std::cout<< "Graph information is not set";
        return false;
    }
    if (!PAR && (graph_->numNodes() - agents_.size() < 2))
    {
        std::cout<< "There must be at least two empty vertices in the graph";
        return false;
    }

    // Make sure that the configuration is feasible
    for (size_t i = 0; i < agents_.size(); ++i)
    {
        agents_[i].restart();

        if (agents_[i].getStart() >= graph_->numNodes())
        {
            std::cout<< "Agent %u has start position %d which is not on the graph" <<i<<agents_[i].getStart();
            return false;
        }
        if (agents_[i].getGoal() >= graph_->numNodes())
        {
            std::cout<< "Agent %u has goal position %d which is not on the graph"<< i<<agents_[i].getGoal();
            return false;
        }
    }

    if (PAR) {
        return executePushAndSpin();
    }

//======================================================================================//
}
PushAndSpin::Plan PushAndSpin::getPlan() const
{
    return actions_;
}
PushAndSpin::Condense& PushAndSpin::getCondense()
{
    return condense;
}
void PushAndSpin::addTemporaryAction(unsigned int id, std::vector<unsigned int>& path)
{
    #ifndef NDEBUG
    for (size_t i = 0; i < path.size()-1; ++i)
        assert(graph_->edgeExists(path[i], path[i+1]));
    #endif


    // No need to initialize with the start locations .
    // Instead , each time we want to insert a move, we have to check in the previous list <actions,temp_actions>
    // If the agent's id exist, add the last node at begining of the second vector
    // if it doesnt exist, add the location at begining of the second vector
    if (path.size()==1) {
        bool found = false ;
        for (int i=temp_actions_.size()-1; !found && i>=0 ; i--){
            if (temp_actions_[i].first == id) {
                found = true;
                // add the last loc in the vector
                path.insert (path.begin(),temp_actions_[i].second[temp_actions_[i].second.size()-1]);
            }
        }

        // Doesnt exist in temp_actions , try to find it in actions
        for (int i=actions_.size()-1; !found && i>=0 ; i--){
            if (actions_[i].first == id) {
                found = true;
                    found = true;
                    // add the last loc in the vector
                    path.insert (path.begin(),actions_[i].second[actions_[i].second.size()-1]);

            }
        }


        // Not found, this is the first move ! add start loc before
        if (!found)
            for (int j=0 ; j<agents_.size(); j++)
                    if (agents_[j].getID() == id )
                       path.insert (path.begin(),agents_[j].getStart());

    }
    temp_actions_.push_back(std::make_pair(id, path));
}



// Try to get two empty vertices in the neighborhood of agt and swap_agt around swap_vertex
bool PushAndSpin::clear(unsigned int agt, unsigned int swap_agt, unsigned int swap_vertex)
{
    std::vector<unsigned int> neighbors;
    graph_->getNeighbors(swap_vertex, neighbors);

    std::vector<unsigned int> empty_neighbors;
    for (size_t i = 0; i < neighbors.size(); ++i)
    {
        if (occupied_.find(neighbors[i]) == occupied_.end())
            empty_neighbors.push_back(neighbors[i]);

        // we only need two
        if (empty_neighbors.size () == 2)
            break;
    }

    if (empty_neighbors.size() >= 2) // stage.1: already have 2 empty neighbors
        return true;

    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agt].getCurrent()));

    //////////////////// CLEARING ////////////////////
    int clear_start_index = temp_actions_.size ();

    std::vector<unsigned int> my_neighbors;
    unsigned int pre_push_index;
    std::vector<unsigned int> temp_path;

    std::vector<unsigned int> entire_neighborhood(neighbors.begin(), neighbors.end());
    entire_neighborhood.push_back(swap_vertex);

    // stage.2 : Going through all non-empty neighbors - Clear Phase 1
    for (size_t i = 0; i < neighbors.size() && empty_neighbors.size() < 2; i++)
    {
        // Do not try to clear from where the swapping agents are
        if (neighbors[i] == agents_[agt].getCurrent() || neighbors[i] == agents_[swap_agt].getCurrent())
            continue;

        // Finding the agent occupying this vertex
        unsigned int occupied_agt = NO_AGENT;
        for (size_t j = 0; j < agents_.size() && occupied_agt == NO_AGENT; j++)
        {
            if (agents_[j].getCurrent() == neighbors[i])
                occupied_agt = j;
        }

        // Make sure there is an agent at this neighbor vertex
        if (occupied_agt != NO_AGENT)
        {
            // Enumerate neighbor of neighbors
            graph_->getNeighbors(agents_[occupied_agt].getCurrent(), my_neighbors);
            bool successful_push = false;

            // Iterate through neighbors of neighbors, and try to push toward them
            for (size_t j = 0; j < my_neighbors.size() && !successful_push; ++j)
            {
                // Don't try to push into the neighborhood of swap_vtx
                if (std::find(entire_neighborhood.begin(), entire_neighborhood.end(), my_neighbors[j]) != entire_neighborhood.end()) continue;

                // Push agents_[occupied_agt] to his neighbor
                unsigned int start_position = agents_[occupied_agt].getCurrent();
                temp_path.clear();
                temp_path.push_back(start_position);
                temp_path.push_back(my_neighbors[j]);

                pre_push_index = temp_actions_.size();
                if (push(occupied_agt, temp_path, entire_neighborhood))  // successful push away from vtx
                {
                    empty_neighbors.push_back(start_position);
                    successful_push = true;

                    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agt].getCurrent()));
                }
                else
                    deleteTempActions(pre_push_index);
            }
        }
    }

    bool successful_clear = empty_neighbors.size() == 2;

    // stage.3 : Only one neighbor was freed - Clear Phase 2
    if (empty_neighbors.size () == 1)
    {
        // Push swapping robots away from vtx
        unsigned int agt_on_vtx, agt_on_nbr, neighbor_vtx;
        if (agents_[agt].getCurrent() == swap_vertex)
        {
            agt_on_vtx = agt;
            agt_on_nbr = swap_agt;
            neighbor_vtx = agents_[swap_agt].getCurrent();
        }
        else
        {
            agt_on_vtx = swap_agt;
            agt_on_nbr = agt;
            neighbor_vtx = agents_[agt].getCurrent();
        }

        assert(graph_->edgeExists(swap_vertex, neighbor_vtx));

        std::vector<unsigned int> reverse_path;
        reverse_path.push_back(swap_vertex);
        reverse_path.push_back(neighbor_vtx);

        int pre_push_index = temp_actions_.size ();
        if (push(agt_on_vtx, reverse_path, empty_neighbors))  // successfully pushed swapping agts backward one vertex.
        {
            // Pick one occupied neighbor and push through empty_neighbors[0]
            unsigned int occupied_agt = NO_AGENT;
            for (size_t i = 0; i < neighbors.size() && occupied_agt == NO_AGENT; ++i)
                if (neighbors[i] != neighbor_vtx && neighbors[i] != empty_neighbors[0])
                    for (size_t j = 0; j < agents_.size (); j++)
                        if (agents_[j].getCurrent() == neighbors[i])
                            occupied_agt = j;

            assert(occupied_agt != NO_AGENT);

            std::vector<unsigned int> push_path;
            push_path.push_back(agents_[occupied_agt].getCurrent());
            push_path.push_back(swap_vertex);
            push_path.push_back(empty_neighbors[0]);

            // Moving occupied robot to the empty neighbor
            addTemporaryAction(agents_[occupied_agt].getID(), push_path);
            occupied_.erase(agents_[occupied_agt].getCurrent());
            agents_[occupied_agt].setCurrent(empty_neighbors[0]);
            occupied_.insert(empty_neighbors[0]);

            // Moving swapping robots back to swap_vtx
            std::vector<unsigned int> tiny_path;
            tiny_path.push_back(agents_[agt_on_vtx].getCurrent());
            tiny_path.push_back(swap_vertex);
            occupied_.erase(agents_[agt_on_vtx].getCurrent());
            agents_[agt_on_vtx].setCurrent(swap_vertex);
            occupied_.insert(swap_vertex);
            addTemporaryAction(agents_[agt_on_vtx].getID(), tiny_path);

            tiny_path.clear ();
            tiny_path.push_back(agents_[agt_on_nbr].getCurrent());
            tiny_path.push_back(neighbor_vtx);
            occupied_.erase(agents_[agt_on_nbr].getCurrent());
            agents_[agt_on_nbr].setCurrent(neighbor_vtx);
            occupied_.insert(neighbor_vtx);
            addTemporaryAction(agents_[agt_on_nbr].getID(), tiny_path);

            //Check each neighbor of empty_neighbors[0] and try to push
            std::vector <unsigned int> my_neighbors;
            graph_->getNeighbors(empty_neighbors[0], my_neighbors);

            assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agt].getCurrent()));

            for (size_t i = 0; i < my_neighbors.size() && empty_neighbors.size() < 2; i++)
            {
                // Do not try to push to any vertex in the neighborhood of swap_vtx (or swap_vtx)
                if (std::find(entire_neighborhood.begin(), entire_neighborhood.end(), my_neighbors[i]) != entire_neighborhood.end()) continue;

                std::vector<unsigned int> test_path;
                test_path.push_back (agents_[occupied_agt].getCurrent());
                test_path.push_back (my_neighbors[i]);

                int pre_push_index2 = temp_actions_.size ();
                if (push(occupied_agt, test_path, entire_neighborhood))
                {
                    empty_neighbors.push_back (test_path[0]);
                    successful_clear = true;

                    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agt].getCurrent()));

                }
                else
                    deleteTempActions(pre_push_index2);
            }

            if (!successful_clear)
                deleteTempActions(pre_push_index);
        }
        else
        {
            deleteTempActions(pre_push_index);
        }
    }

    // sanity check
    if (!successful_clear)
        deleteTempActions(clear_start_index);

    return successful_clear;
}

// Uses a BrFS to find the closest empty cell to index 1 in the shortest_path.
// The path to the empty cell is saved in push_path.  Obstacles indicates vertices that we
// should not push through.
int PushAndSpin::computePushPath(unsigned int rbt, const std::vector<unsigned int>& shortest_path,
                                 std::vector<unsigned int>& push_path, const std::vector<unsigned int>& _obstacles)
{
    // Nowhere to push toward...
    if (shortest_path.size () < 2) {
        std::cout<<"computePushPath: shortest_path.size ()<2"<<std::endl;
        return -1;
    }

    std::vector<unsigned int> search_obstacles (_obstacles.begin (), _obstacles.end ());
    search_obstacles.push_back (shortest_path[0]);  // Search is not allowed to expand where the push robot currently is

    graph_->BrFS(shortest_path[1], occupied_, push_path, search_obstacles, true);

    // Make sure to add start to the end of the path
    if (push_path.size () > 0)
        push_path.insert(push_path.begin(), shortest_path[0]);

    return (push_path.size());
}

// Uses a breadth-first search to find the closest vertex of degree 3 or more
// (excluding previous_attempts) to the start vertex.  The path to the new
// swap vertex is saved to swap_path.
void PushAndSpin::computeSwapPath(unsigned int start, std::vector <unsigned int> &swap_path,
                                  const std::vector<unsigned int> &previous_attempts)
{
    std::vector<unsigned int> possible_swap_goals(swap_vertices_.size ());
    std::vector<unsigned int>::iterator iter = possible_swap_goals.begin();
    iter = std::set_difference (swap_vertices_.begin(), swap_vertices_.end(),
                                previous_attempts.begin(), previous_attempts.end(),
                                possible_swap_goals.begin());

    std::set<unsigned int> swap_goals(possible_swap_goals.begin(), iter);
    graph_->BrFS(start, swap_goals, swap_path);
}

void PushAndSpin::computeSwapVertices(void)
{
    // Precompute all vertices with degree > 2
    swap_vertices_.clear();
    for (size_t i = 0; i < graph_->numNodes(); i++)
    {
        if (graph_->getNumNeighbors(i) > 2)
            swap_vertices_.insert(i);
    }
}

// Removes all actions from temp_actions_ starting at start_index
void PushAndSpin::deleteTempActions(unsigned int start_index)
{
    if (temp_actions_.size() == 0)
        return;

    assert(temp_actions_.size() >= start_index);

    unsigned int ts_index = temp_actions_.size ()-1;
    while (ts_index != start_index-1)
    {
        const std::vector<unsigned int>& original_path = temp_actions_[ts_index].second;

        unsigned int agt_index = NO_AGENT;
        for (size_t i = 0; i < agents_.size() && agt_index == NO_AGENT; i++)
        {
            if (temp_actions_[ts_index].first == agents_[i].getID())
                agt_index = i;
        }

        assert(agents_[agt_index].getCurrent() == original_path.back ());
        occupied_.erase(agents_[agt_index].getCurrent());
        agents_[agt_index].setCurrent(original_path.front());
        occupied_.insert(original_path.front());
        --ts_index;
    }

    while (temp_actions_.size () != start_index)
        temp_actions_.pop_back ();

    assert(temp_actions_.size () == start_index);
}


// Performs a swap between agt and swap_agt at the given swap_vertex.
// This method assumes that agt and swap_agt are adjacent, one of
// them currently occupies swap_vertex, and there are at least two free
// vertices in the neighborhood of swap_vertex.
void PushAndSpin::executeSwap(unsigned int agt, unsigned int swap_agt,
                              unsigned int swap_vertex)
{
    std::vector <unsigned int> neighbors;
    graph_->getNeighbors(swap_vertex, neighbors);
    std::vector <unsigned int> empty_swap_neighbors;
    for (size_t i = 0; i < neighbors.size(); i++)
    {
        if (occupied_.find(neighbors[i]) == occupied_.end())
            empty_swap_neighbors.push_back(neighbors[i]);

        // we only need two
        if (empty_swap_neighbors.size () == 2)
            break;
    }

    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agt].getCurrent()));
    assert(agents_[agt].getCurrent() == swap_vertex || agents_[swap_agt].getCurrent() == swap_vertex);
    assert(empty_swap_neighbors.size () >= 2);

    // Make sure agt is on swap_vertex
    if (agents_[swap_agt].getCurrent() == swap_vertex)
        std::swap(swap_agt, agt);

    assert(agents_[agt].getCurrent() == swap_vertex);

    int agt1_start = agents_[agt].getCurrent();
    int agt2_start = agents_[swap_agt].getCurrent();

    assert(graph_->edgeExists(agt1_start, agt2_start));

    // swap
    // moving agent 1 to an empty swap neighbor
    std::vector <unsigned int> tiny_path;
    tiny_path.push_back (agents_[agt].getCurrent()); // swap_vertex
    tiny_path.push_back (empty_swap_neighbors[0]);
    addTemporaryAction(agents_[agt].getID(), tiny_path);
    agents_[agt].setCurrent(empty_swap_neighbors[0]);

    // moving agent 2 to a different empty swap neighbor
    tiny_path.clear ();
    tiny_path.push_back (agents_[swap_agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    tiny_path.push_back (empty_swap_neighbors[1]);
    addTemporaryAction(agents_[swap_agt].getID(), tiny_path);
    agents_[swap_agt].setCurrent(empty_swap_neighbors[1]);


    // Move agent 1 to agent 2's original position
    tiny_path.clear ();
    tiny_path.push_back (agents_[agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    tiny_path.push_back (agt2_start);
    addTemporaryAction(agents_[agt].getID(), tiny_path);
    agents_[agt].setCurrent(agt2_start);

    // Move robot 2 to robot 1's original start (swap_vertex);
    tiny_path.clear ();
    tiny_path.push_back (agents_[swap_agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    addTemporaryAction(agents_[swap_agt].getID(), tiny_path);
    agents_[swap_agt].setCurrent(agt1_start);
}

bool PushAndSpin::multipush(unsigned int ft, unsigned int bk,int n1,int n2)
{
   int start = -1;
    if (agents_[ft].getCurrent() > agents_[bk].getCurrent())
    {
        if (agents_[bk].getCurrent() >= n1)
            start = bk;
        else
            start = ft;
    }
    if (agents_[ft].getCurrent() < agents_[bk].getCurrent())
    {
        if (agents_[bk].getCurrent() <= n1)
            start = bk;
        else
            start = ft;
    }

    std::vector<unsigned int> unpushable;
    std::vector <unsigned int> p1;
    std::vector <unsigned int> p2;

    graph_->AStar(agents_[ft].getCurrent(), n1, p1);
    graph_->AStar(agents_[bk].getCurrent(), n2, p2);


    if ((std::find (p1.begin(),p1.end(),agents_[bk].getCurrent())!=p1.end()) && (std::find (p1.begin(),p1.end(),n2)!=p1.end()) )
        start = bk;
    if ((std::find (p1.begin(),p1.end(),agents_[bk].getCurrent())==p1.end()) && (std::find (p1.begin(),p1.end(),n2)!=p1.end()) )
        start = ft;
    if ((std::find (p2.begin(),p2.end(),agents_[ft].getCurrent())!=p2.end()) && (std::find (p2.begin(),p2.end(),n1)!=p2.end()) )
        start = ft;
    if ((std::find (p2.begin(),p2.end(),agents_[bk].getCurrent())==p2.end()) && (std::find (p2.begin(),p2.end(),n1)!=p2.end()) )
        start = bk;



    if (start ==ft) {
        if (!push (ft, p1, unpushable))
                return false;
        if (!push (bk, p2, unpushable))
                return false;
    }


    else if (start ==bk){
        if (!push (bk, p2, unpushable))
                return false;
        if (!push (ft, p1, unpushable))
                return false;
        }

    return true;
}

// "Pushes" agt along the "shortest_path", pushing agents that do not exist
// in 'unpushable' vertices out of the way when possible.
bool PushAndSpin::push(unsigned int agt, std::vector<unsigned int> shortest_path,
                       const std::vector<unsigned int>& unpushable)
{



    //std::cout<<"Push is called for agent started at:";
    //std::cout<<agents_[agt].getStart()<<std::endl;



    assert(shortest_path.size() > 0);
    unsigned int goal = shortest_path.back();

    while(agents_[agt].getCurrent() != goal)
    {
        // Move along the path while it is free
        unsigned int start = shortest_path[0];
        for (size_t i = 1; i < shortest_path.size(); ++i)
        {
            if (occupied_.find(shortest_path[i]) == occupied_.end())
                start = shortest_path[i];
            else
                break;
        }

        // Path was totally free
        // Adding this action to the action set
        if (start == goal)
            addTemporaryAction(agents_[agt].getID(), shortest_path);
        // Add the actions that moved along the empty portion of the path
        else
        {
            std::vector <unsigned int> partial_path;
            int index = 0;
            while (shortest_path[index] != start)
                partial_path.push_back(shortest_path[index++]);
            partial_path.push_back(shortest_path[index]);

            if (partial_path.size () > 1)
                addTemporaryAction(agents_[agt].getID(), partial_path);
        }

        occupied_.erase(agents_[agt].getCurrent());
        agents_[agt].setCurrent(start);
        occupied_.insert(agents_[agt].getCurrent());

        if (start == goal)
            return true;

        while (shortest_path[0] != agents_[agt].getCurrent())
            shortest_path.erase(shortest_path.begin());

        // Now, find the shortest path from an empty node to agt's start position
        // with NO unpushable agent along the way (these are "blocked")

        std::vector <unsigned int> shortest_push_path;

        computePushPath(agt, shortest_path, shortest_push_path, unpushable);


        if (shortest_push_path.empty()) {
            //std::cout<<"Empty\n\n";
            return false;
        }
        else
        // The path must have more than one vertex in it
        assert(shortest_push_path.size () != 1);
        // Pushing all agents along the path...
        for (int i = shortest_push_path.size()-2; i >= 0; --i)
        {

            bool found = false;
            for (size_t j = 0; j < agents_.size() && !found; j++)
            {

                if (agents_[j].getCurrent() == shortest_push_path[i])
                {

                    //std::cout<<"Move this robot#";
                    //std::cout<<agents_[j].getID()<<std::endl;
                    std::vector<unsigned int> tiny_path;
                    tiny_path.push_back(agents_[j].getCurrent());
                    tiny_path.push_back(shortest_push_path[i+1]);
                    addTemporaryAction(agents_[j].getID(), tiny_path);
                    agents_[j].setCurrent(shortest_push_path[i+1]);
                    found = true;
                }


            }
        }


        // The end of the path is the empty vertex.  Add this vertex as occupied.  Removed beginning of path from occupied.
        occupied_.erase(shortest_push_path.front());
        occupied_.insert(shortest_push_path.back());



        // Trim off part of path that we traveled
        if (agents_[agt].getCurrent() != goal)
        {
            while (shortest_path[0] != agents_[agt].getCurrent())
                shortest_path.erase(shortest_path.begin());
        }


    }



    return true;
}



// Change the order of agents in the agent list so that the agents with
// goals furthest away from the root of the MST of the graph appear first
void PushAndSpin::reorderAgents(void)
{
    // Root of MST will be swap_vertices_[0]
    if (swap_vertices_.size() == 0)
        return;

    std::vector<unsigned int> goals;
    for (size_t i = 0; i < agents_.size(); ++i)
        goals.push_back(agents_[i].getGoal());

    std::vector<unsigned int> predecessors;
    graph_->getMinSpanningTree(*(swap_vertices_.begin()), predecessors);

    std::map<unsigned int, double> distancemap;
    for (size_t i = 0; i < goals.size(); ++i)
    {
        double dist = 0.0;
        unsigned int v = goals[i];
        while (predecessors[v] != v)
        {
            dist += graph_->edgeWeight(v, predecessors[v]);
            v = predecessors[v];
        }
        distancemap[goals[i]] = dist;
    }

    std::vector<unsigned int> newOrdering;
    // Insertion sort into output list
    for (size_t i = 0; i < goals.size(); ++i)
    {
        // Output will contain indices into the input vector.
        newOrdering.push_back(int(i));

        // Insertion sort:
        for (int j = 0; j < (int)i; ++j)
        {
            // Vertices with larger distance from the root come first
            if (distancemap[goals[j]] <= distancemap[goals[i]])
            {
                // Shifting all members down one:
                for (int k = newOrdering.size()-2; k >= j; --k)
                    newOrdering[k+1] = newOrdering[k];
                newOrdering[j] = (int)i;

                break;
            }
        }
    }

    std::vector<Agent> newAgents(agents_.size(), Agent(0,0,0));
    for (size_t i = 0; i < agents_.size(); ++i)
        newAgents[i] = agents_[i];

    agents_ = newAgents;
}


void PushAndSpin::addTemporaryAction_(unsigned int id, const std::vector<unsigned int>& path)
{
    temp_actions_.push_back(std::make_pair(id, path));
}
// Reverses the set of actions used to swap agt with swap_agt.
// start_index is the index of the action set before the swap actions were performed.
void PushAndSpin::reversePaths_cc (unsigned int agt, unsigned int swap_agt, int &start_index, int &time, int &end, bool flage)
{
    unsigned int ts_index = end ;
    bool s = false , a = false ;
    int end_simultanous=-1,start_simultanous=-1;

    while (ts_index != start_index)
    {

        std::vector<unsigned int> original_path = temp_actions_[ts_index].second;
        std::vector<unsigned int> reverse_path (original_path.rbegin(), original_path.rend());
        assert(original_path.size() != 0);
        if (temp_actions_[ts_index].first == agents_[agt].getID())
        {
            //========
            for (int y=0; y<simulatnous_moves_indexes.size(); y++){
                if ( ts_index <= simulatnous_moves_indexes[y].second && start_simultanous == -1 && end_simultanous ==-1 && ts_index >= simulatnous_moves_indexes[y].first){
                    start_simultanous = temp_actions_.size();
                }
            }
            //========
            addTemporaryAction_(agents_[swap_agt].getID(), reverse_path);
            agents_[swap_agt].setCurrent(original_path.front());
        }

        else if (temp_actions_[ts_index].first == agents_[swap_agt].getID())
        {
            //========
            for (int y=0; y<simulatnous_moves_indexes.size(); y++){
                if ( ts_index <= simulatnous_moves_indexes[y].second && start_simultanous == -1 && end_simultanous ==-1 && ts_index >= simulatnous_moves_indexes[y].first){
                    start_simultanous = temp_actions_.size();
                }
            }
            //========
            addTemporaryAction_(agents_[agt].getID(), reverse_path);
            agents_[agt].setCurrent(original_path.front());
        }
        else {
            unsigned int agt_id = temp_actions_[ts_index].first;
            unsigned int agt_index = NO_AGENT;
            for (unsigned int i = 0; i < agents_.size () && agt_index == NO_AGENT; i++)
            {

                if (agents_[i].getID() == agt_id)
                    agt_index = i;
            }
            assert(agt_index != NO_AGENT);

            //========
            for (int y=0; y<simulatnous_moves_indexes.size(); y++){
                if ( ts_index <= simulatnous_moves_indexes[y].second && start_simultanous == -1 && end_simultanous ==-1 && ts_index >= simulatnous_moves_indexes[y].first){
                    start_simultanous = temp_actions_.size();
                }
            }
            //========
            addTemporaryAction(agt_id, reverse_path);
            agents_[agt_index].setCurrent(original_path.front());
        }
        //=======
        for (int y=0; y<simulatnous_moves_indexes.size(); y++){
            if ( ts_index == simulatnous_moves_indexes[y].first &&  start_simultanous !=-1 ) {
                end_simultanous = temp_actions_.size()-1;
                simulatnous_moves_indexes.push_back(std::make_pair(start_simultanous,end_simultanous));
                start_simultanous=-1;
                end_simultanous=-1;
            }
        }
        //========
        --ts_index;
    }

    /*
    int cntr=0;
    std::cout<<"Temp action after = \n";
    for (int i=0; i<temp_actions_.size(); i++) {
        std::cout<<cntr++;
        std::cout<<")";
        std::cout<<temp_actions_[i].first;
        std::cout<<":[";
        for (int j=0; j<temp_actions_[i].second.size(); j++) {
            std::cout<<temp_actions_[i].second[j];
            std::cout<<",";
        }
        std::cout<<"]\n";
    }*/

    reset_occupied () ;
}


void PushAndSpin::reversePaths_cc_bb (unsigned int agt, unsigned int swap_agt, int &start_index, int &time, int &end, bool flage)
{
    /*
    int cntr =1;
    std::cout<<"Temp action before = \n";
    for (int i=0; i<temp_actions_.size(); i++) {
        std::cout<<cntr++;
        std::cout<<")";
        std::cout<<temp_actions_[i].first;
        std::cout<<":[";
        for (int j=0; j<temp_actions_[i].second.size(); j++) {
            std::cout<<temp_actions_[i].second[j];
            std::cout<<",";
        }
        std::cout<<"]\n";
    }*/

    unsigned int ts_index = end ;
    bool s = false , a = false ;

    while (flage && (ts_index != start_index)) {
        std::vector<unsigned int> original_path = temp_actions_[ts_index].second;
        std::vector<unsigned int> reverse_path (original_path.rbegin(), original_path.rend());

        assert(original_path.size() != 0);
        if (temp_actions_[ts_index].first == agents_[agt].getID() )
        {
            if (ts_index <=time) {          // reverse their moves when they are going to swapping cycle
            //std::cout<<"We will add REV(swap_agt) through ";
            //std::cout<<original_path.front()<<std::endl;
            addTemporaryAction(agents_[swap_agt].getID(), reverse_path);
            occupied_.erase(agents_[swap_agt].getCurrent());
            agents_[swap_agt].setCurrent(original_path.front());
            occupied_.insert(original_path.front());
            a = true ;
            }
        }

        else if (temp_actions_[ts_index].first == agents_[swap_agt].getID())
        {
            if (ts_index <=time) {   // reverse their moves when they are going to swapping cycle
            //std::cout<<"We will add REV(swap_agt) through ";
            //std::cout<<original_path.front()<<std::endl;
            addTemporaryAction(agents_[agt].getID(), reverse_path);
            occupied_.erase(agents_[agt].getCurrent());
            agents_[agt].setCurrent(original_path.front());
            occupied_.insert(original_path.front());
            s = true ;
            }
        }
        ts_index --;
    }


    ts_index = end ;

    while (ts_index != start_index)
    {

        std::vector<unsigned int> original_path = temp_actions_[ts_index].second;
        std::vector<unsigned int> reverse_path (original_path.rbegin(), original_path.rend());
        assert(original_path.size() != 0);

        // Make sure to exchange paths for agt and swap_agt
        if (temp_actions_[ts_index].first == agents_[agt].getID())
        ;

        else if (temp_actions_[ts_index].first == agents_[swap_agt].getID())
        ;
        else {
            unsigned int agt_id = temp_actions_[ts_index].first;
            unsigned int agt_index = NO_AGENT;
            for (unsigned int i = 0; i < agents_.size () && agt_index == NO_AGENT; i++)
            {

                if (agents_[i].getID() == agt_id)
                    agt_index = i;
            }
            assert(agt_index != NO_AGENT);
            addTemporaryAction(agt_id, reverse_path);
            agents_[agt_index].setCurrent(original_path.front());
        }

        --ts_index;
    }
    reset_occupied () ;
}

// Top level swap method.  Attempts to swap rbt with the robot adjacent to it along its
// shortest path.  Invokes the clear, multipush, and reverse actions for a successful swap.
bool PushAndSpin::swap(unsigned int agt, const std::vector <unsigned int> &shortest_path,
                       const std::vector<unsigned int>& unpushable)
{
    assert(shortest_path.size () > 1);

    unsigned int swap_agent = NO_AGENT;
    for (size_t i = 0; i < agents_.size () && swap_agent == NO_AGENT; i++)
    {
        if (agents_[i].getCurrent() == shortest_path[1])
            swap_agent = i;
    }

    assert(agents_[agt].getCurrent() == shortest_path[0]);
    assert(swap_agent != NO_AGENT);
    assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agent].getCurrent()));

    std::vector <unsigned int> attempted_swap_vertices;
    bool successful_swap = false;

    // Iterating over all possible swap vertices
    while (!successful_swap && attempted_swap_vertices.size () != swap_vertices_.size ())
    {
        // Finding the swap vertex
        std::vector <unsigned int> swap_path;
        computeSwapPath(agents_[agt].getCurrent(), swap_path, attempted_swap_vertices);

        if (swap_path.size () == 0) // No paths to any swap vertices
            return false;

        unsigned int swap_vertex = swap_path.back();
        assert(swap_vertices_.find(swap_path.back()) != swap_vertices_.end());

        unsigned int sequence_index_start_push_swap = temp_actions_.size()-1;

        // Pushing to swap vertex and clearing two neighbors
        //ITS ME !!! EBTEHAL
        if (/*multipush(agt, swap_agent, swap_path) &&*/
            clear(agt, swap_agent, swap_vertex))
        {
            assert(graph_->edgeExists(agents_[agt].getCurrent(), agents_[swap_agent].getCurrent()));

            // Swapping the two agents' positions
            executeSwap(agt, swap_agent, swap_vertex);
            successful_swap = true;

            // Reversing paths to maintain invariant
            //reversePaths(agt, swap_agent, sequence_index_start_push_swap,temp_actions_.size()-5);
        }
        else
        {
            // mark this swap vertex as a failure
            attempted_swap_vertices.push_back(swap_vertex);
            std::sort(attempted_swap_vertices.begin (), attempted_swap_vertices.end ());

            // Start over (reverse everybody out and delete from temporary sequence)
            deleteTempActions(sequence_index_start_push_swap+1);
        }
    }

    // If we displaced a higher priority agent with the swap, need to resolve
    if (successful_swap && agt > swap_agent)
    {
        assert(shortest_path.size () >= 2);
        assert(shortest_path[1] == agents_[agt].getCurrent());
        assert(agents_[swap_agent].getCurrent() == shortest_path[0]);
        return resolve(agt, swap_agent, shortest_path, unpushable);
    }
    return successful_swap;
}
