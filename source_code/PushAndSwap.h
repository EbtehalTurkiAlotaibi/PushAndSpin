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

#ifndef PAS_PUSH_AND_SWAP_HPP
#define PAS_PUSH_AND_SWAP_HPP

#include <map>
#include <vector>
#include <set>
#include <cassert>

#include "Graph.h"
#include "Agent.h"

class PushAndSwap
    {
        public:
            // Definition of a Path: A pair containing an agent ID and a vector of vertices
            typedef std::pair<unsigned int, std::vector<unsigned int> > Path;
            // Definition of a Plan: A vector of Paths
            typedef std::vector<Path> Plan;

            // Default constructor
            PushAndSwap ();
            // Parametrized constructor.  Takes a list of agents and a pointer to the graph structure
            PushAndSwap (const std::vector<Agent>& agents, Graph* graph);

            virtual ~PushAndSwap();

            // Solve the instance defined by a set of agents and a graph
            virtual bool solve();
            // Retrieve the most recent plan after calling solve
            virtual Plan getPlan() const;
            // Post-process a plan by removing redundant actions
            static Plan smooth(const Plan& plan);
            std::vector<Agent> getAgents () { return agents_; }
            int getReversedmoves() { return reversedmoves; }

        protected:
            // Adds a Path to the temporary actions list
            void addTemporaryAction(unsigned int id, const std::vector<unsigned int>& path);
            // Clear method.  Attempts to free two vertices in the neighborhood of vtx so that
            // agt and swap_agt can swap
            bool clear(unsigned int agt, unsigned int swap_agt, unsigned int vtx);
            // Computes a path to the nearest empty vertex using breadth-first search
            void computePushPath(unsigned int rbt, const std::vector<unsigned int> &shortest_path,
                                std::vector<unsigned int> &push_path, const std::vector<unsigned int> &_obstacles);
            // Computes a path to the nearest vertex with degree >=3 using breadth-first search
            void computeSwapPath(unsigned int start, std::vector <unsigned int> &swap_path,
                                 const std::vector<unsigned int> &previous_attempts);
            // Computes the set of vertices in the graph with degree >= 3
            void computeSwapVertices(void);
            // Deletes all actions from the temporary actions list starting at start_index and moves all agents
            // back to where they were before start_index
            void deleteTempActions(unsigned int start_index);
            // Top level search loop.
            bool executePushAndSwap(void);
            // Executes the swap primitive between agt and swap_agt.  Presumes clear was successful.
            void executeSwap(unsigned int agt, unsigned int swap_agt, unsigned int swap_vertex);
            // Pushes agent agt along shortest_path, avoiding _unpushable.
            // If tag_along >= 0, this agent will be moved with agt.  Only_empty denotes that
            // agt should only move along empty vertices in the path (no pushing others).
            virtual bool push(unsigned int agt, std::vector<unsigned int> shortest_path,
                              const std::vector <unsigned int> &_unpushable,
                              unsigned int tag_along = -1, bool only_empty = false);
            // Execute a compound push actions with agt and swap_agt along the specified path
            bool pushToSwapVertex(unsigned int agt, unsigned int swap_agt,
                                  std::vector<unsigned int>& path);
            // Reorder the agents based on depth of their goals in a MST of the graph
            void reorderAgents(void);
            // Reverse the paths (after a swap) beginning at start_index, exchanging those for agt and swap_agt.
            void reversePaths(unsigned int agt, unsigned int swap_agt, unsigned int start_index);
            // Resolve an instance where agt swapped with swap_agt, where swap_agt was unpushable.
            virtual bool resolve(unsigned int agt, unsigned int swap_agt, std::vector <unsigned int> shortest_path,
                                 std::vector <unsigned int> _unpushable);
            // Attempt a swap for agt along its path shortest_path
            virtual bool swap(unsigned int agt, const std::vector<unsigned int> &shortest_path);

            // The list of agents to plan for
            std::vector<Agent> agents_;
            // Pointer to the graph the agent's plan on
            Graph* graph_;

            // Sets that are helpful for primitives
            std::set<unsigned int> occupied_;
            std::vector<unsigned int> unpushable_;
            std::set<unsigned int> swap_vertices_;

            int reversedmoves;
            // Temporary list of actions (used for swap)
            Plan temp_actions_;
            // Set of actions which have been performed during search
            Plan actions_;
    };

#endif
