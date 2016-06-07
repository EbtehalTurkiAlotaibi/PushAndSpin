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

#ifndef PASp_PUSH_AND_SPIN_HPP
#define PASp_PUSH_AND_SPIN_HPP

#include <map>
#include <vector>
#include <set>
#include <cassert>
#include <stack>
#include <list>

#include "Graph.h"
#include "Agent.h"

    class PushAndSpin
    {
        public:
            // Definition of a Path: A pair containing an agent ID and a vector of vertices
            typedef std::pair<unsigned int, std::vector<unsigned int> > Path;
            // Definition of a Plan: A vector of Paths
            typedef std::vector<Path> Plan;
            // Definition of a Condense: A vector of Plans
            typedef std::vector <std::pair<unsigned int,PushAndSpin::Plan > > Condense;
            bool PAR;
            int ID;
            double extra_computation_time;

            struct node { int id ; bool visited ; bool done; std::vector<unsigned int> nbrs; };


            // Default constructor
            PushAndSpin ();
            // Parametrized constructor.  Takes a list of agents and a pointer to the graph structure
            PushAndSpin(const std::vector<Agent>& agents, Graph* graph, bool &PAR_);

            virtual ~PushAndSpin();

            // Solve the instance defined by a set of agents and a graph
            virtual bool solve();
            // Retrieve the most recent plan after calling solve
            virtual Plan getPlan() const;
            // Post-process a plan by removing redundant actions
            static Plan smooth(const Plan& plan);
            virtual Condense& getCondense() ;
            std::vector<Agent> getAgents () { return agents_; }
            int getReversedmoves() { return reversedmoves; }
            double get_rot_time() { return rot_time; }

        protected:
            // Adds a Path to the temporary actions list
            void addTemporaryAction(unsigned int id, std::vector<unsigned int>& path);
            // Clear method.  Attempts to free two vertices in the neighborhood of vtx so that
            // agt and swap_agt can swap
            bool clear(unsigned int agt, unsigned int swap_agt, unsigned int vtx);
            // Computes a path to the nearest empty vertex using breadth-first search
            int computePushPath(unsigned int rbt, const std::vector<unsigned int> &shortest_path,
                                std::vector<unsigned int> &push_path, const std::vector<unsigned int> &_obstacles);
            // Computes a path to the nearest vertex with degree >=3 using breadth-first search
            void computeSwapPath(unsigned int start, std::vector <unsigned int> &swap_path,
                                 const std::vector<unsigned int> &previous_attempts);
            // Computes the set of vertices in the graph with degree >= 3
            void computeSwapVertices(void);
            // Deletes all actions from the temporary actions list starting at start_index and moves all agents
            // back to where they were before start_index
            void deleteTempActions(unsigned int start_index);
            bool executePushAndSpinUtil(int i);
            // Top level search loop.
            bool executePushAndSpin(void);
            // Executes the swap primitive between agt and swap_agt.  Presumes clear was successful.
            void executeSwap(unsigned int agt, unsigned int swap_agt, unsigned int swap_vertex);
            // Pushes the two (adjacent) agents along the path together
            virtual bool multipush(unsigned int ft, unsigned int bk,int n1,int n2);
            // Pushes agent agt along shortest_path, avoiding _unpushable.
            virtual bool push(unsigned int agt, std::vector<unsigned int> shortest_path,
                              const std::vector <unsigned int> &_unpushable);
            // Reorder the agents based on target depth ordering
            void reorderAgents(void);
            // Reverse the paths (after a swap) beginning at start_index, exchanging those for agt and swap_agt.
            void reversePaths_cc_bb (unsigned int agt, unsigned int swap_agt, int &start_index, int &time, int &end, bool flage);
            // Reverse the paths (after a swap) beginning at start_index, exchanging those for agt and swap_agt.
            void reversePaths_cc (unsigned int agt, unsigned int swap_agt, int &start_index, int &time, int &end, bool flage);

            // Resolve an instance where agt swapped with swap_agt, where swap_agt was unpushable.
            void reversePaths_cb (unsigned int agt, unsigned int swap_agt, int &start_index, int &time, int &end, std::vector<unsigned int>cycle);
            virtual bool resolve(unsigned int agt, unsigned int swap_agt, std::vector <unsigned int> shortest_path,
                                 std::vector <unsigned int> _unpushable);
            // Attempt a swap for agt along its path shortest_path
            virtual bool rotate(unsigned int agt, const std::vector<unsigned int> &shortest_path);
            // Attempt a swap for agt along its path shortest_path
            virtual bool swap(unsigned int agt, const std::vector<unsigned int> &shortest_path,
                              const std::vector<unsigned int>& unpushable);

            bool moveAgent(unsigned int agt, std::vector<unsigned int> shortest_path, const std::vector<unsigned int>& unpushable);

            void addToUnpushable (int node) ;
            void addToTempUnpushable (int node) ;
            void deleteFromUnpushable (int node) ;
            void deleteFromTempUnpushable (int node) ;
            void computeSwapCycle(unsigned int s, unsigned int sa, std::vector < std::vector <unsigned int> >&swapping_cycles,
                                               const std::vector <std::vector  <unsigned int> > &previous_attempts,std::vector<unsigned int> &entering_nodes);

            int DFS (std::vector<node>& vertex, int root,int s, int sa,int& occupied,std::vector <unsigned int> temp_path);
            void DFSUtil (node parent, std::vector<node>& vertex, int& available,int s,int sa,int& occupied,std::vector <unsigned int> temp_path) ;
            bool stepForward (int agt, int node_in_graph) ;
            bool executeRotate (unsigned int agt,unsigned int swap_agent,unsigned int entering_nodes,std::vector <unsigned int >swapping_cycles, int rot_num);
            bool ElementaryCycles () ;
            bool isCyclic();
            int stacking_cycle (int agt, int swap_agt, int direction, int entering_node, std::vector <unsigned int> swapping_cycles,int back_agent);
            bool clear_entering_node(int agt, int entering_node, std::vector <unsigned int> swapping_cycle);
            void set_cycle_type(std::vector <unsigned int> cycle) ;
            void print_cycle ();
            bool clear_to_cycle(int agnt,int swap_agnt,int entering_node,std::vector <unsigned int> swapping_cycle);
            bool IsBlocked (std::vector<unsigned int> cycle) ;
            void set_entering_nodes (int s, int sa) ;
            void set_initial_locs() ;
            bool cc_case (unsigned int agt, unsigned int swap_agent,const std::vector <unsigned int> &shortest_path, int c_index);
            bool bb_case (unsigned int front, unsigned int bk,const std::vector <unsigned int> &shortest_path,std::vector <unsigned int >swapping_cycles,unsigned int entering_nodes, int direction );
            bool cb_case (unsigned int front, unsigned int bk,const std::vector <unsigned int> &shortest_path,std::vector <unsigned int >swapping_cycles,unsigned int entering_nodes, int direction  );
            //bool bc_case (unsigned int agt, unsigned int swap_agent,const std::vector <unsigned int> &shortest_path);

            // The list of agents to plan for
            std::vector<Agent> agents_;
            // Pointer to the graph the agent's plan on
            Graph* graph_;
            void reset_occupied () ;
            // Sets that are helpful for primitives
            std::set<unsigned int> occupied_;
            std::vector<unsigned int> unpushable_;
            std::vector<unsigned int> temp_unpushable_;
            std::set<unsigned int> swap_vertices_;
            std::vector < std::vector<unsigned int> > cycles_index;
            std::vector <unsigned int> entering_node_index;


            // Temporary list of actions (used for swap)
            Plan temp_actions_;
            // Set of actions which have been performed during search
            std::vector <std::pair <unsigned int,unsigned int> > simulatnous_moves_indexes;
            Plan actions_;
            // Definition of a Condense: A vector of Plans
            Condense condense;
            int reversedmoves;
            double rot_time;

            // Adds a Path to the temporary actions list
            void addTemporaryAction_(unsigned int id, const std::vector<unsigned int>& path);
    };


#endif
